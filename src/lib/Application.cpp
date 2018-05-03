/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <glib.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Application.h"
#include "Window.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Palette.h"
#include "Timer.h"
#include "Point.h"
#include "StringRef.h"

#include "../../3rdparty/libtermkey/termkey.h"
#include "../../3rdparty/json-parser/json.h"
#include "../../3rdparty/folly/sorted_vector_types.h"

namespace ncxmms2 {

class ApplicationPrivate
{
public:
    ApplicationPrivate() :
        useColors(true),
        mainLoop(nullptr),
        termKey(nullptr),
        termKeyReadTimeoutId(0),
        stdinPollSource(0),
        mouseEnabled(false),
        mouseDoubleClickInterval(300),
        mouseDoubleClickTimeExpired(true),
        mouseDoubleClickHaveReleaseEvent(false),
        mouseDoubleClickButton(-1),
        mouseDoubleClickPosition(-1, -1),
        mainWindow(nullptr),
        grabbedFocusWindow(nullptr),
        runningOnXTerm(false),
        colorSchemeTree(nullptr) {}

    bool useColors;

    GMainLoop *mainLoop;

    TermKey *termKey;
    int termKeyReadTimeoutId;
    int stdinPollSource;
    static gboolean stdinEvent(GIOChannel *iochan, GIOCondition cond, gpointer data);
    static gboolean readKeyAfterTimeout(gpointer data);
    void sendKeyPressedEvent(const TermKeyKey& key);

    bool mouseEnabled;
    Timer mouseDoubleClickTimer;
    int mouseDoubleClickInterval;
    bool mouseDoubleClickTimeExpired;
    bool mouseDoubleClickHaveReleaseEvent;
    int mouseDoubleClickButton;
    Point mouseDoubleClickPosition;
    void sendMouseEvent(const TermKeyKey& key);
    void mouseEnable();
    void mouseDisable();

    static void signalHandler(int signal);
    static void resizeSignalHandler(int signal);

    Window *mainWindow;
    Window *grabbedFocusWindow;

    bool runningOnXTerm;
    
    json_value *colorSchemeTree;
    std::map<std::string, std::shared_ptr<const Palette>> paletteMap;
    void parseObjectPalette(const json_value *paletteTree, Palette *palette,
                            const std::map<std::string, int>& rolesMap);
};
} // ncxmms2

namespace {

json_value * JsonFindObject(const json_value *root, const char *name)
{
    if (!root || root->type != json_object)
        return nullptr;
    
    auto *obj = &root->u.object;
    for (unsigned int i = 0; i < obj->length; ++i) {
        auto *value = &obj->values[i];
        if (!std::strcmp(value->name, name) && value->value->type == json_object)
            return value->value;
    }
    
    return nullptr;
}

const char * JsonObjectGetString(const json_value *object, const char *key)
{
    assert(object->type == json_object);

    auto *obj = &object->u.object;
    for (unsigned int i = 0; i < obj->length; ++i) {
        auto *value = &obj->values[i];
        if (!std::strcmp(value->name, key) && value->value->type == json_string)
            return value->value->u.string.ptr;
    }
    
    return nullptr;
}

}

using namespace ncxmms2;

Application *Application::inst = nullptr;

#define CHECK_INST                                                                     \
    do {                                                                               \
        if (!inst) {                                                                   \
            throw std::logic_error(std::string(__PRETTY_FUNCTION__)                    \
                                   .append(": There is no instance of application!")); \
        }                                                                              \
    } while (0)

void Application::init(bool useColors, bool mouseEnable)
{
    delete inst;
    inst = new Application(useColors, mouseEnable);
}

void Application::shutdown()
{
    delete inst;
    inst = nullptr;
}

Application::Application(bool useColors, bool mouseEnable) :
    d(new ApplicationPrivate())
{
    setlocale(LC_ALL, "");

    // Mouse support
    d->mouseEnabled = mouseEnable;
    if (d->mouseEnabled) {
        d->mouseEnable();
    }

    // Initialize libtermkey
    d->termKey = termkey_new(STDIN_FILENO, 0);
    if (!d->termKey)
        throw std::runtime_error("Can't allocate memory for TermKey!");
    termkey_set_flags(d->termKey, TERMKEY_FLAG_UTF8);

    //Init ncurses
    initscr();
    curs_set(FALSE);
    raw();

    d->useColors = useColors;
    if (has_colors() && useColors) {
        start_color();
        use_default_colors();
    }

    //Install signal handler
    signal(SIGINT,   ApplicationPrivate::signalHandler);
    signal(SIGTERM,  ApplicationPrivate::signalHandler);
    signal(SIGTSTP,  ApplicationPrivate::signalHandler);
    signal(SIGWINCH, ApplicationPrivate::resizeSignalHandler);

    //Creat mainloop
    d->mainLoop = g_main_loop_new(NULL, FALSE);

    //Polling stdin
    GIOChannel *iochan = g_io_channel_unix_new(STDIN_FILENO);
    d->stdinPollSource = g_io_add_watch(iochan, G_IO_IN, ApplicationPrivate::stdinEvent, NULL);
    g_io_channel_unref(iochan);

    d->mouseDoubleClickTimer.setSingleShot(true);
    d->mouseDoubleClickTimer.timeout_Connect([this](){
        d->mouseDoubleClickTimeExpired = true;
        d->mouseDoubleClickTimer.stop();
    });
    
    const char *term = std::getenv("TERM");
    if (term)
        d->runningOnXTerm = std::strstr(term, "xterm") != nullptr;
}

void Application::run()
{
    CHECK_INST;

    if (!inst->d->mainWindow)
        throw std::logic_error(std::string(__PRETTY_FUNCTION__).append(": There is no instance of main window!"));

    g_main_loop_run(inst->d->mainLoop);
}

void Application::quit()
{
    CHECK_INST;
    
    if (g_main_loop_is_running(inst->d->mainLoop))
        g_main_loop_quit(inst->d->mainLoop);
}

void Application::setMainWindow(Window* window)
{
    CHECK_INST;
    inst->d->mainWindow = window;
}

void Application::grabFocus(Window *window)
{
    CHECK_INST;
    inst->d->grabbedFocusWindow = window;
    window->focusAcquired();
}

void Application::releaseFocus()
{
    CHECK_INST;
    Window *win = inst->d->grabbedFocusWindow;
    if (win) {
        inst->d->grabbedFocusWindow = nullptr;
        win->focusLost();
    }
}

bool Application::useColors()
{
    CHECK_INST;
    return inst->d->useColors;
}

Size Application::terminalSize()
{
    return Size(COLS, LINES);
}

void Application::setMouseDoubleClickInterval(int msec)
{
    CHECK_INST;
    inst->d->mouseDoubleClickInterval = msec;
}

void Application::setColorSchemeFile(const std::string& file)
{
    CHECK_INST;
    
    gchar *contents;
    size_t length;
    GError *error = NULL;
    g_file_get_contents(file.c_str(), &contents, &length, &error);
    if (error) {
        std::string errorMsg = std::string("Unable to read color scheme file: ").append(error->message);
        g_error_free(error);
        throw std::runtime_error(errorMsg);
    }
    
    json_settings settings = {0, 0, nullptr, nullptr, nullptr, 0};
    settings.settings |= json_enable_comments;
    char jsonError[json_error_max + 1];
    jsonError[json_error_max] = '\0';
    inst->d->colorSchemeTree = json_parse_ex(&settings, contents, length, jsonError);
    g_free(contents);
    if (!inst->d->colorSchemeTree) {
        std::string errorMsg = std::string("Parsing color scheme file failed: ").append(jsonError);
        throw std::runtime_error(errorMsg);
    }
}

std::shared_ptr<const Palette> Application::getPalette(const std::string&                    className,
                                                       const std::shared_ptr<const Palette>& oldPalette)
{
    return getPalette(className, oldPalette, std::map<std::string, int>());
}

std::shared_ptr<const Palette> Application::getPalette(const std::string&                     className,
                                                       const std::shared_ptr<const Palette>&  oldPalette,
                                                       const std::map<std::string, int>&      userRolesMap)
{
    CHECK_INST;
    ApplicationPrivate *p = inst->d.get();

    auto it = p->paletteMap.find(className);
    if (it != p->paletteMap.end()) // Already have palette in map
        return it->second;         // no need to parse color scheme again

    auto *classPalette = JsonFindObject(p->colorSchemeTree, className.c_str());
    if (!classPalette) {
        if (oldPalette) // Return empty ptr, if we have no custom palette
            return std::shared_ptr<const Palette>();

        // This is a special case where Window  gets it initial palette
        std::shared_ptr<const Palette> palette = std::make_shared<const Palette>();
        p->paletteMap[className] = palette;
        return palette;
    }

    // At this point we have custom palette, old palette is used as initial
    // (custom palette doesn'thave to change all roles) or creat new if emtpty pointer is passed
    std::unique_ptr<Palette> palette = oldPalette
                                       ? make_unique<Palette>(*oldPalette)
                                       : make_unique<Palette>();
    
    static const std::map<std::string, int> standartRolesMap =
    {
        {"Text",            Palette::RoleText           },
        {"Background",      Palette::RoleBackground     },
        {"Selection",       Palette::RoleSelection      },
        {"Highlight",       Palette::RoleHighlight      },
        {"HighlightedText", Palette::RoleHighlightedText}
    };
    p->parseObjectPalette(classPalette, palette.get(), standartRolesMap);
    p->parseObjectPalette(classPalette, palette.get(), userRolesMap);
    
    std::shared_ptr<const Palette> sharedPalette(palette.release());
    p->paletteMap[className] = sharedPalette;
    return sharedPalette;
}

void ApplicationPrivate::parseObjectPalette(const json_value *paletteTree, Palette *palette,
                                            const std::map<std::string, int>& rolesMap)
{
    static const struct PaletteGroup {const char *name; int group;} paletteGroupes[] =
    {
        {"Active",   Palette::GroupActive  },
        {"Inactive", Palette::GroupInactive},
        {nullptr, -1}
    };

    static const folly::sorted_vector_map<StringRef, Color> colorNamesMap
    {
        {"Default", ColorDefault},
        {"Black",   ColorBlack  },
        {"Red",     ColorRed    },
        {"Green",   ColorGreen  },
        {"Yellow",  ColorYellow },
        {"Blue",    ColorBlue   },
        {"Magenta", ColorMagenta},
        {"Cyan",    ColorCyan   },
        {"White",   ColorWhite  }
    };

    const PaletteGroup *paletteGroup = paletteGroupes;
    while (paletteGroup->name) {
        auto *paletteGroupObject = JsonFindObject(paletteTree, paletteGroup->name);
        if (!paletteGroupObject)
            continue;

        for (const auto& role : rolesMap) {
            const char *colorName = JsonObjectGetString(paletteGroupObject, role.first.c_str());
            if (!colorName)
                continue;
            auto it = colorNamesMap.find(colorName);
            if (it != colorNamesMap.end()) {
                palette->setColor((Palette::ColorGroup)paletteGroup->group,
                                  role.second,
                                  it->second);
            }
        }
        ++paletteGroup;
    }
}

void Application::setTerminalWindowTitle(const std::string& title)
{
    CHECK_INST;
    if (inst->d->runningOnXTerm) {
        std::printf("\033]2;%s\007", title.c_str());
        std::fflush(stdout);
    }
}

gboolean ApplicationPrivate::stdinEvent(GIOChannel *iochan, GIOCondition cond, gpointer data)
{
    NCXMMS2_UNUSED(iochan);
    NCXMMS2_UNUSED(cond);
    NCXMMS2_UNUSED(data);

    ApplicationPrivate *p = Application::inst->d.get();

    if(p->termKeyReadTimeoutId) {
        g_source_remove(p->termKeyReadTimeoutId);
        p->termKeyReadTimeoutId = 0;
    }

    termkey_advisereadable(p->termKey);

    TermKeyResult res;
    TermKeyKey key;
    while((res = termkey_getkey(p->termKey, &key)) == TERMKEY_RES_KEY) {
        switch (key.type) {
            case TERMKEY_TYPE_UNICODE:
            case TERMKEY_TYPE_KEYSYM:
            case TERMKEY_TYPE_FUNCTION:
                p->sendKeyPressedEvent(key);
                break;

            case TERMKEY_TYPE_MOUSE:
                p->sendMouseEvent(key);
                break;

            default:
                break;
        }
    }

    if(res == TERMKEY_RES_AGAIN)
        p->termKeyReadTimeoutId = g_timeout_add(termkey_get_waittime(p->termKey),
                                                readKeyAfterTimeout, NULL);

    return TRUE;
}

gboolean ApplicationPrivate::readKeyAfterTimeout(gpointer data)
{
    NCXMMS2_UNUSED(data);

    ApplicationPrivate *p = Application::inst->d.get();
    TermKeyKey key;
    if(termkey_getkey_force(p->termKey, &key) == TERMKEY_RES_KEY) {
        switch (key.type) {
            case TERMKEY_TYPE_UNICODE:
            case TERMKEY_TYPE_KEYSYM:
            case TERMKEY_TYPE_FUNCTION:
                p->sendKeyPressedEvent(key);
                break;

            case TERMKEY_TYPE_MOUSE:
                p->sendMouseEvent(key);
                break;

            default:
                break;
        }
    }

    p->termKeyReadTimeoutId = 0;
    return FALSE;
}

void ApplicationPrivate::sendKeyPressedEvent(const TermKeyKey& key)
{
    Window *win = grabbedFocusWindow ? grabbedFocusWindow : mainWindow;
    win->keyPressedEvent(KeyEvent(key));
}

void ApplicationPrivate::sendMouseEvent(const TermKeyKey& key)
{
    TermKeyMouseEvent event;
    int button;
    int x, y;

    termkey_interpret_mouse(termKey, &key, &event, &button, &y, &x);
    MouseEvent::Type eventType;
    switch (event) {
        case TERMKEY_MOUSE_PRESS:
            if (   !mouseDoubleClickTimeExpired && mouseDoubleClickHaveReleaseEvent
                && button == mouseDoubleClickButton
                && mouseDoubleClickPosition.x() == x && mouseDoubleClickPosition.y() == y) {
                eventType = MouseEvent::Type::ButtonDoubleClick;
                mouseDoubleClickTimeExpired = true;
            } else {
                eventType = MouseEvent::Type::ButtonPress;
                mouseDoubleClickTimer.startMs(mouseDoubleClickInterval);
                mouseDoubleClickTimeExpired = false;
                mouseDoubleClickHaveReleaseEvent = false;
                mouseDoubleClickButton = button;
                mouseDoubleClickPosition.setX(x);
                mouseDoubleClickPosition.setY(y);
            }
            break;

        case TERMKEY_MOUSE_RELEASE:
            eventType = MouseEvent::Type::ButtonRelease;
            if (!mouseDoubleClickTimeExpired)
                mouseDoubleClickHaveReleaseEvent = true;
            break;

        default:
            return;
    }

    // It seems that the minimum coordinates are (1,1), not (0,0).
    // Fix that before sending MouseEvent to main window.
    --x;
    --y;

    if (grabbedFocusWindow) {
        // If mouse is over the window which grabbed focus, send MouseEvent to it doing global
        // to local coordinates translation, else release focus.
        Point grabbedWindowPosition = grabbedFocusWindow->globalPosition();
        if (   x >= grabbedWindowPosition.x() && x < grabbedWindowPosition.x() + grabbedFocusWindow->cols()
            && y >= grabbedWindowPosition.y() && y < grabbedWindowPosition.y() + grabbedFocusWindow->lines()) {
            grabbedFocusWindow->mouseEvent(
                MouseEvent(eventType,
                           Point(x - grabbedWindowPosition.x(), y - grabbedWindowPosition.y()),
                           button)
            );
        } else {
            Application::releaseFocus();
            mainWindow->mouseEvent(MouseEvent(eventType, mainWindow->toLocalCoordinates(Point(x, y)), button));
        }
    } else {
        mainWindow->mouseEvent(MouseEvent(eventType, mainWindow->toLocalCoordinates(Point(x, y)), button));
    }
}

void ApplicationPrivate::mouseEnable()
{
    // XTerm control sequence for enabling mouse support,
    // see http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
    std::printf("\033[?1000h");
    std::fflush(stdout);
}

void ApplicationPrivate::mouseDisable()
{
    std::printf("\033[?1000l");
    std::fflush(stdout);
}

void ApplicationPrivate::signalHandler(int signal)
{
    switch(signal) {
        case SIGINT:
        case SIGTERM:
            if (Application::inst)
                g_main_loop_quit(Application::inst->d->mainLoop);
            break;

        default:
            break;
    }
}

void ApplicationPrivate::resizeSignalHandler(int signal)
{
    NCXMMS2_UNUSED(signal);

    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char*)&size);
    resizeterm(size.ws_row, size.ws_col);

    if (Application::inst && Application::inst->d->mainWindow)
        Application::inst->d->mainWindow->resize(Size(size.ws_col, size.ws_row));
}

Application::~Application()
{
    delete d->mainWindow;

    if (d->termKeyReadTimeoutId)
        g_source_remove(d->termKeyReadTimeoutId);

    g_source_remove(d->stdinPollSource);

    if (g_main_loop_is_running(d->mainLoop))
        g_main_loop_quit(d->mainLoop);

    g_main_loop_unref(d->mainLoop);

    endwin();
    termkey_destroy(d->termKey);
    if (d->mouseEnabled)
        d->mouseDisable();
    
    json_value_free(d->colorSchemeTree);
}


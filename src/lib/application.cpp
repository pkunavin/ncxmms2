/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <map>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "application.h"
#include "window.h"
#include "keyevent.h"
#include "palette.h"

#include "../../3rdparty/libtermkey/termkey.h"

namespace ncxmms2 {

class ApplicationPrivate
{
public:
    ApplicationPrivate() :
        useColors(true),
        mainLoop(nullptr),
        termKey(nullptr),
        termKeyReadTimeoutId(0),
        mainWindow(nullptr),
        grabbedFocusWindow(nullptr) {}

    bool useColors;

    GMainLoop *mainLoop;

    TermKey *termKey;
    int termKeyReadTimeoutId;
    int stdinPollSource;
    static gboolean stdinEvent(GIOChannel *iochan, GIOCondition cond, gpointer data);
    static gboolean readKeyAfterTimeout(gpointer data);
    void sendKeyPressedEvent(const TermKeyKey& key);

    static void signalHandler(int signal);
    static void resizeSignalHandler(int signal);

    Window *mainWindow;
    Window *grabbedFocusWindow;

    boost::property_tree::ptree colorSchemeTree;
    std::map<std::string, std::shared_ptr<Palette>> paletteMap;
    void parseColorSchemeTree(const boost::property_tree::ptree&  colorSchemeTree,
                              Palette                            *palette,
                              const std::map<std::string, int>&   rolesMap);
};
} // ncxmms2

using namespace ncxmms2;

Application *Application::inst = nullptr;

#define CHECK_INST                                                                     \
    do {                                                                               \
        if (!inst) {                                                                   \
            throw std::logic_error(std::string(__PRETTY_FUNCTION__)                    \
                                   .append(": There is no instance of application!")); \
        }                                                                              \
    } while (0)

void Application::init(bool useColors)
{
    delete inst;
    inst = new Application(useColors);
}

void Application::shutdown()
{
    delete inst;
    inst = nullptr;
}

Application::Application(bool useColors) : d(new ApplicationPrivate())
{
    setlocale(LC_ALL, "");

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
}

void Application::run()
{
    CHECK_INST;

    if (!inst->d->mainWindow)
        throw std::logic_error(std::string(__PRETTY_FUNCTION__).append(": There is no instance of main window!"));

    g_main_loop_run(inst->d->mainLoop);
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
}

void Application::releaseFocus()
{
    CHECK_INST;
    inst->d->grabbedFocusWindow = nullptr;
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

void Application::setColorSchemeFile(const std::string& file)
{
    CHECK_INST;
    try
    {
        boost::property_tree::json_parser::read_json(file, inst->d->colorSchemeTree);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& error)
    {
        throw std::runtime_error(
                std::string("Parsing color scheme file failed: ").append(error.what())
              );
    }
}

std::shared_ptr<Palette> Application::getPalette(const std::string&                className,
                                                 const std::shared_ptr<Palette>&   oldPalette,
                                                 const std::map<std::string, int>& userRolesMap)
{
    CHECK_INST;
    ApplicationPrivate *p = inst->d.get();

    auto it = p->paletteMap.find(className);
    if (it != p->paletteMap.end()) // Already have palette in map
        return it->second;         // no need to parse color scheme again

    const auto classPalette = p->colorSchemeTree.find(className);
    if (classPalette == p->colorSchemeTree.not_found()) {
        if (oldPalette)                        // Return empty ptr, if we have no custom
            return std::shared_ptr<Palette>(); // palette

        std::shared_ptr<Palette> palette = std::make_shared<Palette>(); // This is a special
        p->paletteMap[className] = palette;                             // case where Window
        return palette;                                                 // gets it initial palette
    }

    // At this point we have custom palette, old palette is used as initial (custom palette doesn't
    // have to change all roles) or creat new if emtpty pointer is passed
    std::shared_ptr<Palette> palette = oldPalette
                                       ? std::make_shared<Palette>(*oldPalette)
                                       : std::make_shared<Palette>();
    p->paletteMap[className] = palette;

    static const std::map<std::string, int> standartRolesMap =
    {
        {"Text",            Palette::RoleText},
        {"Background",      Palette::RoleBackground},
        {"Selection",       Palette::RoleSelection},
        {"Highlight",       Palette::RoleHighlight},
        {"HighlightedText", Palette::RoleHighlightedText}
    };
    p->parseColorSchemeTree(classPalette->second, palette.get(), standartRolesMap);
    p->parseColorSchemeTree(classPalette->second, palette.get(), userRolesMap);

    return palette;
}

void ApplicationPrivate::parseColorSchemeTree(const boost::property_tree::ptree&  colorSchemeTree,
                                              Palette                            *palette,
                                              const std::map<std::string, int>&   rolesMap)
{
    static const struct PaletteGroup {const char *name; int group;} paletteGroupes[] =
    {
        {"Active",   Palette::GroupActive},
        {"Inactive", Palette::GroupInactive},
        {nullptr, -1}
    };

    static const std::map<std::string, Color> colorNamesMap =
    {
        {"Black",   ColorBlack},
        {"Red",     ColorRed},
        {"Green",   ColorGreen},
        {"Yellow",  ColorYellow},
        {"Blue",    ColorBlue},
        {"Magenta", ColorMagenta},
        {"Cyan",    ColorCyan},
        {"White",   ColorWhite}
    };

    const PaletteGroup *paletteGroup = paletteGroupes;
    while (paletteGroup->name) {
        std::string key(paletteGroup->name);
        key.push_back('.');
        const size_t keyStrSize = key.size();
        for (auto roleIt = rolesMap.begin(), roleItEnd = rolesMap.end(); roleIt != roleItEnd; ++roleIt) {
            key.resize(keyStrSize);
            key.append(roleIt->first);
            std::string colorName = colorSchemeTree.get(key, std::string());
            auto it = colorNamesMap.find(colorName);
            if (it != colorNamesMap.end()) {
                palette->setColor((Palette::ColorGroup)paletteGroup->group,
                                  roleIt->second,
                                  it->second);
            }
        }
        ++paletteGroup;
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
    while((res = termkey_getkey(p->termKey, &key)) == TERMKEY_RES_KEY)
        p->sendKeyPressedEvent(key);

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
    if(termkey_getkey_force(p->termKey, &key) == TERMKEY_RES_KEY)
        p->sendKeyPressedEvent(key);

    p->termKeyReadTimeoutId = 0;
    return FALSE;
}

void ApplicationPrivate::sendKeyPressedEvent(const TermKeyKey& key)
{
    Window *win = grabbedFocusWindow ? grabbedFocusWindow : mainWindow;
    win->keyPressedEvent(KeyEvent(key));
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
}


/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <sys/ioctl.h>

#include <map>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "application.h"
#include "window.h"
#include "keyevent.h"
#include "colors.h"

namespace ncxmms2 {

class ApplicationPrivate
{
public:
    ApplicationPrivate() : mainWindow(nullptr), stealedFocusWindow(nullptr) {}


    GMainLoop *mainLoop;

    int stdinPollSource;
    static gboolean stdinEvent(GIOChannel *iochan, GIOCondition cond, gpointer data);

    static void signalHandler(int signal);
    static void resizeSignalHandler(int signal);

    Window *mainWindow;
    Window *stealedFocusWindow;
};
} // ncxmms2

using namespace ncxmms2;

Application *Application::inst = nullptr;

#define CHECK_INST if (!inst) {throw std::logic_error(std::string(__PRETTY_FUNCTION__).append(": There is no instance of application!"));}

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

    //Init ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    if (has_colors() && useColors) {
        start_color();
        use_default_colors();

        std::map<int, std::pair<int, int>> colorPairs =
        {
            {ColorBlack,   {COLOR_BLACK,   -1}},
            {ColorRed,     {COLOR_RED,     -1}},
            {ColorGreen,   {COLOR_GREEN,   -1}},
            {ColorYellow,  {COLOR_YELLOW,  -1}},
            {ColorBlue,    {COLOR_BLUE,    -1}},
            {ColorMagenta, {COLOR_MAGENTA, -1}},
            {ColorCyan,    {COLOR_CYAN,    -1}},
            {ColorWhite,   {COLOR_WHITE,   -1}},
        };

        for (auto it = colorPairs.begin(), end = colorPairs.end(); it != end; ++it) {
            init_pair((*it).first, (*it).second.first, (*it).second.second);
        }
    }

    refresh();

    //Install signal handler
    signal(SIGINT,   ApplicationPrivate::signalHandler);
    signal(SIGTERM,  ApplicationPrivate::signalHandler);
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

void Application::stealFocus(Window *window)
{
    CHECK_INST;
    inst->d->stealedFocusWindow = window;
}

void Application::releaseFocus()
{
    CHECK_INST;
    inst->d->stealedFocusWindow = nullptr;
}

Size Application::terminalSize()
{
    return Size(LINES, COLS);
}

gboolean ApplicationPrivate::stdinEvent(GIOChannel* iochan, GIOCondition cond, gpointer data)
{
    static_assert(sizeof(wint_t) == sizeof(KeyEvent::key_t), "KeyEvent::key_t is too small for storing UTF chars!");
    wint_t key;
    const auto res = get_wch(&key);
    if (res != ERR) {
        ApplicationPrivate *p = Application::inst->d.get();
        Window *win = p->stealedFocusWindow ? p->stealedFocusWindow : p->mainWindow;
        if (key == 127) {// Fix backpspace key
            win->keyPressedEvent(KeyEvent(KeyEvent::KeyBackspace, true));
        } else {
            win->keyPressedEvent(KeyEvent(key, res == KEY_CODE_YES));
        }
    }
    return TRUE;
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
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char*)&size);
    resizeterm(size.ws_row, size.ws_col);

    cbreak();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    refresh();

    if (Application::inst && Application::inst->d->mainWindow)
        Application::inst->d->mainWindow->resizeEvent(Size(size.ws_row, size.ws_col));
}

Application::~Application()
{
    delete d->mainWindow;

    if (g_main_loop_is_running(d->mainLoop))
        g_main_loop_quit(d->mainLoop);

    g_source_remove(d->stdinPollSource);
    g_main_loop_unref(d->mainLoop);
    endwin();
}


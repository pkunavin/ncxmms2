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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../lib/window.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class HeaderWindow;
class StackedWindow;
class StatusArea;

class MainWindow : public Window
{
public:
    MainWindow(xmms2::Client *xmmsClient);
    ~MainWindow();

    enum StackedWindows
    {
        StackedHelpBrowser,
        StackedPlaylistWindow,
        StackedLocalFileBrowser,
        StackedServerSideBrowser,
        StackedMedialibBrowser,
        StackedPlaylistsBrowser,
        StackedEqualizerWindow,
        StackedSongInfoWindow
    };

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);

protected:
    virtual void resizeChildren(const Size& size);

private:
    xmms2::Client *m_xmmsClient;
    HeaderWindow *m_headerWindow;
    StackedWindow *m_stackedWindow;
    StatusArea *m_statusArea;
    StackedWindows m_lastVisibleScreen;

    void setVisibleScreen(StackedWindows win);
    void handleStackedWindowNameChanged(StackedWindows win, const std::string& title);
    void showSongInfo(int id);
    void showLastVisibleScreen();
};
} // ncxmms2


#endif // MAINWINDOW_H

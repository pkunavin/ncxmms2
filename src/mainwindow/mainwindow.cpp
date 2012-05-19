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

#include <map>
#include <stdexcept>

#include "mainwindow.h"
#include "../statusarea/statusarea.h"
#include "../activeplaylistwindow/activeplaylistwindow.h"
#include "../localfilesystembrowser/localfilesystembrowser.h"
#include "../playlistsbrowser/playlistsbrowser.h"
#include "../headerwindow/headerwindow.h"
#include "../hotkeys.h"

#include "../lib/application.h"
#include "../lib/rectangle.h"
#include "../lib/keyevent.h"
#include "../lib/stackedwindow.h"

using namespace ncxmms2;

MainWindow::MainWindow(Xmms::Client *xmmsClient) :
    Window(Rectangle(0, 0, Application::terminalSize().cols(), Application::terminalSize().lines())),
    m_xmmsClient(xmmsClient)
{
    m_headerWindow = new HeaderWindow(0, 0, cols(), this);
    m_statusArea = new StatusArea(m_xmmsClient,
                                  0, lines() - StatusArea::LinesNumber, cols(), this);

    const Rectangle stackedWindowRect(0,
                                      HeaderWindow::LinesNumber,
                                      cols(),
                                      lines() - StatusArea::LinesNumber - HeaderWindow::LinesNumber);
    m_stackedWindow = new StackedWindow(stackedWindowRect, this);
    m_stackedWindow->setFocus();

    const Rectangle stackedSubWinRect(0, 0, stackedWindowRect.cols(), stackedWindowRect.lines());
    const std::map<StackedWindows, Window*> stakedWindows =
    {
        {StackedPlaylistWindow,         new ActivePlaylistWindow  (xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedLocalFileBrowserWindow, new LocalFileSystemBrowser(xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedPlaylistsBrowser,       new PlaylistsBrowser      (xmmsClient, stackedSubWinRect, m_stackedWindow)}
    };

    for (auto it = stakedWindows.begin(), it_end = stakedWindows.end(); it != it_end; ++it) {
        StackedWindows stackedWindow = (*it).first;
        Window *window = (*it).second;
        m_stackedWindow->addWindow(window);
        window->titleChanged_Connect(&MainWindow::handleStackedWindowTitleChanged, this, stackedWindow, _1);
    }
    setVisibleWindow(StackedPlaylistWindow);
}

void MainWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case Hotkeys::PlaylistScreen:
            setVisibleWindow(StackedPlaylistWindow);
            break;

        case Hotkeys::LocalFileSystemBrowserScreen:
            setVisibleWindow(StackedLocalFileBrowserWindow);
            break;

        case Hotkeys::PlaylistsBrowserScreen:
            setVisibleWindow(StackedPlaylistsBrowser);
            break;

        case Hotkeys::PlaybackToggle:
            if (m_statusArea->playbackStatus() == Xmms::Playback::PLAYING) {
                m_xmmsClient->playback.pause();
            } else {
                m_xmmsClient->playback.start();
            }
            break;

        case Hotkeys::PlaybackStop:
            m_xmmsClient->playback.stop();
            break;

        case Hotkeys::PlaybackNext:
            m_xmmsClient->playlist.setNextRel(1);
            m_xmmsClient->playback.tickle();
            break;

        case Hotkeys::PlaybackPrev:
            m_xmmsClient->playlist.setNextRel(-1);
            m_xmmsClient->playback.tickle();
            break;

        case Hotkeys::PlaybackSeekForward:
            m_xmmsClient->playback.seekMsRel(1000);
            break;

        case Hotkeys::PlaybackSeekBackward:
            m_xmmsClient->playback.seekMsRel(-1000);
            break;

        case Hotkeys::Quit:
            Application::shutdown();
            break;

        default : Window::keyPressedEvent(keyEvent);
    }
}

void MainWindow::resize(const Size& size)
{
    Window::resize(size);
    m_headerWindow->resize(Size(size.cols(), m_headerWindow->lines()));
    m_stackedWindow->resize(Size(size.cols(), size.lines() - m_statusArea->lines() - m_headerWindow->lines()));
    m_statusArea->move(0, size.lines() - m_statusArea->lines());
    m_statusArea->resize(Size(size.cols(), m_statusArea->lines()));
}

void MainWindow::setVisibleWindow(StackedWindows win)
{
    m_stackedWindow->setCurrentIndex(win);
    m_headerWindow->setHeaderTitle(m_stackedWindow->window(win)->title());
}

void MainWindow::handleStackedWindowTitleChanged(StackedWindows win, const std::string& title)
{
    if (m_stackedWindow->currentIndex() == win)
        m_headerWindow->setHeaderTitle(title);
}

MainWindow::~MainWindow()
{

}


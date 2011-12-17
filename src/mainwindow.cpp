/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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
#include "statusarea.h"
#include "playlistwindow.h"
#include "localfilesystembrowser.h"
#include "playlistsbrowser.h"
#include "headerwindow.h"
#include "hotkeys.h"

#include "lib/application.h"
#include "lib/stackedwindow.h"

using namespace ncxmms2;

MainWindow::MainWindow(Xmms::Client *xmmsClient) :
    Window(Application::terminalSize().lines(), Application::terminalSize().cols(), 0, 0),
    m_xmmsClient(xmmsClient),
    m_minimumCols(std::string("[Stopped] ...[xx::xx::xx/xx::xx::xx]").size())
{
    const int statusAreaLines = 2;
    const int headerWindowLines = 2;

    if (lines() < statusAreaLines + 1 + headerWindowLines || cols() < m_minimumCols)
        throw std::runtime_error("Terminal too small!");

    m_headerWindow = new HeaderWindow(headerWindowLines, cols(), 0, 0, this);
    m_stackedWindow = new StackedWindow(lines() - statusAreaLines-headerWindowLines, cols(), headerWindowLines, 0, this);
    m_statusArea = new StatusArea(m_xmmsClient, statusAreaLines, cols(), lines() - statusAreaLines, 0, this);

    m_stackedWindow->setFocus();

    const int screenLines = m_stackedWindow->lines();
    const int screenCols = m_stackedWindow->cols();
    const std::map<StackedWindows, Window*> stakedWindows =
    {
        {StackedPlaylistWindow,         new PlaylistWindow("_active", xmmsClient, screenLines, screenCols, 0, 0, m_stackedWindow)},
        {StackedLocalFileBrowserWindow, new LocalFileSystemBrowser(xmmsClient, screenLines, screenCols, 0, 0, m_stackedWindow)},
        {StackedPlaylistsBrowser,       new PlaylistsBrowser(xmmsClient, screenLines, screenCols, 0, 0, m_stackedWindow)}
    };

    for (auto it = stakedWindows.begin(), it_end = stakedWindows.end(); it != it_end; ++it) {
        StackedWindows stackedWindow = (*it).first;
        Window *window = (*it).second;
        m_stackedWindow->addWindow(window);
        window->setTitleChangedCallback(boost::bind(&MainWindow::handleStackedWindowTitleChanged, this, stackedWindow, _1));
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

void MainWindow::resizeEvent(const Size& size)
{
    if (size.lines() < m_statusArea->lines() + 1 + m_headerWindow->lines() || size.cols() < m_minimumCols)
        throw std::runtime_error("Terminal too small!");

    Window::resizeEvent(size);
    m_headerWindow->resizeEvent(Size(m_headerWindow->lines(), size.cols()));
    m_stackedWindow->resizeEvent(Size(size.lines() - m_statusArea->lines() - m_headerWindow->lines(), size.cols()));
    m_statusArea->move(size.lines() - m_statusArea->lines(), 0);
    m_statusArea->resizeEvent(Size(m_statusArea->lines(), size.cols()));
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


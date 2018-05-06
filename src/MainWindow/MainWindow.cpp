/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <stdexcept>

#include "MainWindow.h"
#include "../XmmsUtils/Client.h"
#include "../StatusArea/StatusArea.h"
#include "../HelpBrowser/HelpBrowser.h"
#include "../ActivePlaylistWindow/ActivePlaylistWindow.h"
#include "../FileSystemBrowser/LocalFileSystemBrowser.h"
#include "../FileSystemBrowser/ServerSideBrowser.h"
#include "../EqualizerWindow/EqualizerWindow.h"
#include "../MedialibBrowser/MedialibBrowser.h"
#include "../PlaylistsBrowser/PlaylistsBrowser.h"
#include "../SongInfoWindow/SongInfoWindow.h"
#include "../HeaderWindow/HeaderWindow.h"
#include "../Hotkeys.h"

#include "../lib/Application.h"
#include "../lib/Rectangle.h"
#include "../lib/KeyEvent.h"
#include "../lib/MouseEvent.h"
#include "../lib/StackedWindow.h"

#include "../../3rdparty/folly/sorted_vector_types.h"

using namespace ncxmms2;

MainWindow::MainWindow(xmms2::Client *xmmsClient) :
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
    const folly::sorted_vector_map<StackedWindows, Window*> stakedWindows
    {
        {StackedHelpBrowser,       new HelpBrowser           (              stackedSubWinRect, m_stackedWindow)},
        {StackedPlaylistWindow,    new ActivePlaylistWindow  (m_xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedLocalFileBrowser,  new LocalFileSystemBrowser(m_xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedServerSideBrowser, new ServerSideBrowser     (m_xmmsClient, stackedSubWinRect, m_stackedWindow)},        
        {StackedMedialibBrowser,   new MedialibBrowser       (m_xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedPlaylistsBrowser,  new PlaylistsBrowser      (m_xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedEqualizerWindow,   new EqualizerWindow       (m_xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedSongInfoWindow,    new SongInfoWindow        (m_xmmsClient, stackedSubWinRect, m_stackedWindow)}
    };

    for (const auto& pair : stakedWindows) {
        StackedWindows stackedWindow = pair.first;
        Window *window = pair.second;
        m_stackedWindow->addWindow(window);
        window->nameChanged_Connect(&MainWindow::handleStackedWindowNameChanged, this, stackedWindow, std::placeholders::_1);
    }
    setVisibleScreen(StackedPlaylistWindow);
    
    auto *plsView = static_cast<PlaylistView*>(m_stackedWindow->window(StackedPlaylistWindow));
    plsView->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    auto *plsBrowser = static_cast<PlaylistsBrowser*>(m_stackedWindow->window(StackedPlaylistsBrowser));
    plsBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    auto *medialibBrowser = static_cast<MedialibBrowser*>(m_stackedWindow->window(StackedMedialibBrowser));
    medialibBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);

    auto *localFsBrowser = static_cast<FileSystemBrowser*>(m_stackedWindow->window(StackedLocalFileBrowser));
    localFsBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);

    auto *serverFsBrowser = static_cast<FileSystemBrowser*>(m_stackedWindow->window(StackedServerSideBrowser));
    serverFsBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    auto *songInfoWin = static_cast<SongInfoWindow*>(m_stackedWindow->window(StackedSongInfoWindow));
    songInfoWin->hideRequested_Connect(&MainWindow::showLastVisibleScreen, this);
}

void MainWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case Hotkeys::Screens::Help::Activate:
            setVisibleScreen(StackedHelpBrowser);
            break;

        case Hotkeys::Screens::ActivePlaylist::Activate:
            setVisibleScreen(StackedPlaylistWindow);
            break;

        case Hotkeys::Screens::LocalFileSystemBrowser::Activate:
            setVisibleScreen(StackedLocalFileBrowser);
            break;
            
        case Hotkeys::Screens::ServerSideBrowser::Activate:
            setVisibleScreen(StackedServerSideBrowser);
            break;

        case Hotkeys::Screens::Equalizer::Activate:
            setVisibleScreen(StackedEqualizerWindow);
            break;

        case Hotkeys::Screens::MedialibBrowser::Activate:
            setVisibleScreen(StackedMedialibBrowser);
            break;

        case Hotkeys::Screens::PlaylistsBrowser::Activate:
            setVisibleScreen(StackedPlaylistsBrowser);
            break;
            
        case Hotkeys::Playback::Toggle:
            if (m_statusArea->playbackStatus() == xmms2::PlaybackStatus::Playing) {
                m_xmmsClient->playbackPause();
            } else {
                m_xmmsClient->playbackStart();
            }
            break;

        case Hotkeys::Playback::Stop:
            m_xmmsClient->playbackStop();
            break;

        case Hotkeys::Playback::Next:
            m_xmmsClient->playlistSetNextRel(1);
            m_xmmsClient->playbackTickle();
            break;

        case Hotkeys::Playback::Prev:
            m_xmmsClient->playlistSetNextRel(-1);
            m_xmmsClient->playbackTickle();
            break;

        case Hotkeys::Playback::SeekForward:
            m_xmmsClient->playbackSeekMsRel(1000);
            break;

        case Hotkeys::Playback::SeekBackward:
            m_xmmsClient->playbackSeekMsRel(-1000);
            break;

        case Hotkeys::Quit:
            Application::quit();
            break;

        default : Window::keyPressedEvent(keyEvent);
    }
}

void MainWindow::mouseEvent(const MouseEvent& ev)
{
    if (m_stackedWindow->pointInWindow(ev.position())) {
        m_stackedWindow->mouseEvent(MouseEvent(ev.type(),
                                               m_stackedWindow->toLocalCoordinates(ev.position()),
                                               ev.button()));
    } else if (m_statusArea->pointInWindow(ev.position())) {
        m_statusArea->mouseEvent(MouseEvent(ev.type(),
                                            m_statusArea->toLocalCoordinates(ev.position()),
                                            ev.button()));
    }
}

void MainWindow::resizeChildren(const Size& size)
{
    m_headerWindow->resize(Size(size.cols(), m_headerWindow->lines()));
    m_stackedWindow->resize(Size(size.cols(), size.lines() - m_statusArea->lines() - m_headerWindow->lines()));
    m_statusArea->move(0, size.lines() - m_statusArea->lines());
    m_statusArea->resize(Size(size.cols(), m_statusArea->lines()));
}

void MainWindow::setVisibleScreen(StackedWindows win)
{
    m_stackedWindow->setCurrentIndex(win);
    m_headerWindow->setHeaderTitle(m_stackedWindow->window(win)->name());
    m_lastVisibleScreen = win;
}

void MainWindow::handleStackedWindowNameChanged(StackedWindows win, const std::string& title)
{
    if (m_stackedWindow->currentIndex() == win)
        m_headerWindow->setHeaderTitle(title);
}

void MainWindow::showSongInfo(int id)
{
    auto *songInfoWin = static_cast<SongInfoWindow*>(m_stackedWindow->window(StackedSongInfoWindow));
    songInfoWin->showSongInfo(id);
    m_stackedWindow->setCurrentIndex(StackedSongInfoWindow);
    m_headerWindow->setHeaderTitle(songInfoWin->name());
}

void MainWindow::showLastVisibleScreen()
{
    setVisibleScreen(m_lastVisibleScreen);
}

MainWindow::~MainWindow()
{

}


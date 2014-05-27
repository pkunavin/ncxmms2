/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <boost/cast.hpp>

#include "mainwindow.h"
#include "../statusarea/statusarea.h"
#include "../helpbrowser/helpbrowser.h"
#include "../activeplaylistwindow/activeplaylistwindow.h"
#include "../localfilesystembrowser/localfilesystembrowser.h"
#include "../equalizerwindow/equalizerwindow.h"
#include "../medialibbrowser/medialibbrowser.h"
#include "../playlistsbrowser/playlistsbrowser.h"
#include "../songinfowindow/songinfowindow.h"
#include "../headerwindow/headerwindow.h"
#include "../hotkeys.h"

#include "../lib/application.h"
#include "../lib/rectangle.h"
#include "../lib/keyevent.h"
#include "../lib/mouseevent.h"
#include "../lib/stackedwindow.h"

#include "../../3rdparty/folly/sorted_vector_types.h"

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
    const folly::sorted_vector_map<StackedWindows, Window*> stakedWindows
    {
        {StackedHelpBrowser,            new HelpBrowser           (            stackedSubWinRect, m_stackedWindow)},
        {StackedPlaylistWindow,         new ActivePlaylistWindow  (xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedLocalFileBrowserWindow, new LocalFileSystemBrowser(xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedMedialibBrowser,        new MedialibBrowser       (xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedPlaylistsBrowser,       new PlaylistsBrowser      (xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedEqualizerWindow,        new EqualizerWindow       (xmmsClient, stackedSubWinRect, m_stackedWindow)},
        {StackedSongInfoWindow,         new SongInfoWindow        (xmmsClient, stackedSubWinRect, m_stackedWindow)}
    };

    for (const auto& pair : stakedWindows) {
        StackedWindows stackedWindow = pair.first;
        Window *window = pair.second;
        m_stackedWindow->addWindow(window);
        window->nameChanged_Connect(&MainWindow::handleStackedWindowNameChanged, this, stackedWindow, std::placeholders::_1);
    }
    setVisibleScreen(StackedPlaylistWindow);
    
    PlaylistView *plsView =
             boost::polymorphic_downcast<PlaylistView*>(m_stackedWindow->window(StackedPlaylistWindow));
    plsView->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    PlaylistsBrowser *plsBrowser =
             boost::polymorphic_downcast<PlaylistsBrowser*>(m_stackedWindow->window(StackedPlaylistsBrowser));
    plsBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    MedialibBrowser *medialibBrowser =
             boost::polymorphic_downcast<MedialibBrowser*>(m_stackedWindow->window(StackedMedialibBrowser));
    medialibBrowser->showSongInfo_Connect(&MainWindow::showSongInfo, this);
    
    SongInfoWindow *songInfoWin =
            boost::polymorphic_downcast<SongInfoWindow*>(m_stackedWindow->window(StackedSongInfoWindow));
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
            setVisibleScreen(StackedLocalFileBrowserWindow);
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
            if (m_statusArea->playbackStatus() == Xmms::Playback::PLAYING) {
                m_xmmsClient->playback.pause();
            } else {
                m_xmmsClient->playback.start();
            }
            break;

        case Hotkeys::Playback::Stop:
            m_xmmsClient->playback.stop();
            break;

        case Hotkeys::Playback::Next:
            m_xmmsClient->playlist.setNextRel(1);
            m_xmmsClient->playback.tickle();
            break;

        case Hotkeys::Playback::Prev:
            m_xmmsClient->playlist.setNextRel(-1);
            m_xmmsClient->playback.tickle();
            break;

        case Hotkeys::Playback::SeekForward:
            m_xmmsClient->playback.seekMsRel(1000);
            break;

        case Hotkeys::Playback::SeekBackward:
            m_xmmsClient->playback.seekMsRel(-1000);
            break;

        case Hotkeys::Quit:
            Application::shutdown();
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
    SongInfoWindow *songInfoWin =
            boost::polymorphic_downcast<SongInfoWindow*>(m_stackedWindow->window(StackedSongInfoWindow));
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


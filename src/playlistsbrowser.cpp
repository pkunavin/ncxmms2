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

#include "playlistsbrowser.h"
#include "playlistslistview.h"
#include "playlistwindow.h"

#include "lib/painter.h"

using namespace ncxmms2;

PlaylistsBrowser::PlaylistsBrowser(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent) :
    Window(lines, cols, yPos, xPos, parent)
{
    setTitle("Playlists browser");

    m_plsListView = new PlaylistsListView(xmmsClient, lines, PlaylistsListViewCols, 0, 0, this);
    m_plsListView->setFocus();

    m_plsViewer = new PlaylistWindow(xmmsClient, lines, cols - PlaylistsListViewCols - 1, 0, PlaylistsListViewCols + 1, this);
    m_plsViewer->setHideCurrentItemInterval(0);
    m_plsViewer->hideCurrentItem();

    m_plsListView->setCurrentItemChangedCallback(boost::bind(&PlaylistsBrowser::setPlsViewerPlaylist, this, _1));
}

void PlaylistsBrowser::showEvent()
{
    Painter painter(this);
    painter.drawVLine(PlaylistsListViewCols, 0, lines());
    painter.flush();
}

void PlaylistsBrowser::setPlsViewerPlaylist(int item)
{
    m_plsViewer->setPlaylist(m_plsListView->playlist(item));
}

void PlaylistsBrowser::resizeEvent(const Size& size)
{
    Window::resizeEvent(size);
    m_plsListView->resizeEvent(Size(size.lines(), PlaylistsListViewCols));
    m_plsViewer->resizeEvent(Size(size.lines(), size.cols() - PlaylistsListViewCols - 1));
}

void PlaylistsBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyRight:
            if (!m_plsViewer->hasFocus()) {
                m_plsViewer->setFocus();
                m_plsViewer->showCurrentItem();
            }
            break;

        case KeyEvent::KeyLeft:
            if (!m_plsListView->hasFocus()) {
                m_plsListView->setFocus();
                m_plsViewer->hideCurrentItem();
            }
            break;

        default: Window::keyPressedEvent(keyEvent);
    }
}

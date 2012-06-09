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

#include "playlistsbrowser.h"
#include "playlistslistview.h"
#include "../playlistview/playlistview.h"

#include "../lib/painter.h"
#include "../lib/keyevent.h"
#include "../lib/size.h"
#include "../lib/rectangle.h"

using namespace ncxmms2;

PlaylistsBrowser::PlaylistsBrowser(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    Window(rect, parent)
{
    setTitle("Playlists browser");

    const Rectangle plsListViewRect(0, 0, PlaylistsListViewCols, rect.lines());
    m_plsListView = new PlaylistsListView(xmmsClient, plsListViewRect, this);
    m_plsListView->setMinumumCols(PlaylistsListViewCols);
    m_plsListView->setMaximumCols(PlaylistsListViewCols);
    m_plsListView->setFocus();

    const Rectangle plsViewerRect(PlaylistsListViewCols + 1, 0,
                                  rect.cols() - PlaylistsListViewCols - 1, rect.lines());
    m_plsViewer = new PlaylistView(xmmsClient, plsViewerRect, this);
    m_plsViewer->setHideCurrentItemInterval(0);
    m_plsViewer->hideCurrentItem();

    m_plsListView->currentItemChanged_Connect(&PlaylistsBrowser::setPlsViewerPlaylist, this);
}

void PlaylistsBrowser::paint(const Rectangle& rect)
{
    Painter painter(this);
    painter.drawVLine(PlaylistsListViewCols, 0, lines());
    painter.flush();
}

void PlaylistsBrowser::setPlsViewerPlaylist(int item)
{
    m_plsViewer->setPlaylist(item != -1
                             ? m_plsListView->playlist(item)
                             : std::string());
}

void PlaylistsBrowser::resize(const Size& size)
{
    Window::resize(size);
    m_plsListView->resize(Size(PlaylistsListViewCols, size.lines()));
    m_plsViewer->resize(Size(size.cols() - PlaylistsListViewCols - 1, size.lines()));
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

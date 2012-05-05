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

#include <boost/lexical_cast.hpp>

#include "activeplaylistwindow.h"
#include "../playlistview/playlistmodel.h"
#include "../settings.h"
#include "../utils.h"

using namespace ncxmms2;

ActivePlaylistWindow::ActivePlaylistWindow(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    PlaylistView(xmmsClient, rect, parent),
    m_autoScrollToActiveSong(true)
{
    xmmsClient->playlist.currentActive()(
        Xmms::bind(&ActivePlaylistWindow::getActivePlaylist, this)
    );
    xmmsClient->playlist.broadcastLoaded()(
        Xmms::bind(&ActivePlaylistWindow::getActivePlaylist, this)
    );

    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    plsModel->activeSongPositionChanged_Connect(&ActivePlaylistWindow::scrollToActiveSong, this);

    plsModel->playlistRenamed_Connect(&ActivePlaylistWindow::updateWindowTitle, this);
    plsModel->totalDurationChanged_Connect(&ActivePlaylistWindow::updateWindowTitle, this);

    //Settings
    m_autoScrollToActiveSong = Settings::value("ActivePlaylistScreen", "autoScrollToActiveSong", true);
}

void ActivePlaylistWindow::updateWindowTitle()
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());

    std::string titleString("Playlist: ");
    titleString.append(plsModel->playlist());

    if (plsModel->itemsCount() > 0) {
        titleString.append(" (");
        titleString.append(boost::lexical_cast<std::string>(plsModel->itemsCount()));
        titleString.append(" tracks");
        if (plsModel->totalDuration() > 0) {
            titleString.append(", ");
            titleString.append(Utils::getTimeStringFromInt(plsModel->totalDuration()));
            titleString.append(" total playtime");
        }
        titleString.push_back(')');
    }
    setTitle(titleString);
}


bool ActivePlaylistWindow::getActivePlaylist(const std::string& playlist)
{
    setPlaylist(playlist);
    updateWindowTitle();
    return true;
}

void ActivePlaylistWindow::scrollToActiveSong(int item)
{
    if (m_autoScrollToActiveSong && isCurrentItemHidden())
        scrollToItem(item);
}

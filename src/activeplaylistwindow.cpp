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

#include "activeplaylistwindow.h"

using namespace ncxmms2;

ActivePlaylistWindow::ActivePlaylistWindow(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent) :
    PlaylistWindow(xmmsClient, lines, cols, yPos, xPos, parent)
{
    xmmsClient->playlist.currentActive()(Xmms::bind(&ActivePlaylistWindow::getActivePlaylist, this));
    xmmsClient->playlist.broadcastLoaded()(Xmms::bind(&ActivePlaylistWindow::getActivePlaylist, this));
}

bool ActivePlaylistWindow::getActivePlaylist(const std::string& playlist)
{
    setPlaylist(playlist);
    return true;
}

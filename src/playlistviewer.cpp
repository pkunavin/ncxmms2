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

#include <xmmsclient/xmmsclient++.h>
#include "playlistviewer.h"

using namespace ncxmms2;

PlaylistViewer::PlaylistViewer(Xmms::Client* xmmsClient, int lines, int cols, int yPos, int xPos, Window* parent) :
	PlaylistWindow(std::string(), xmmsClient, lines, cols, yPos, xPos, parent),
	m_xmmsClient(xmmsClient)
{
	setHideCurrentItemInterval(0);
	
	m_xmmsClient->playlist.currentActive()(Xmms::bind(&PlaylistViewer::getCurrentPlaylist, this));
	m_xmmsClient->playlist.broadcastLoaded()(Xmms::bind(&PlaylistViewer::getCurrentPlaylist, this));
}

void PlaylistViewer::itemEntered(int item)
{
	if (playlist()!=m_currentPlaylist) 
		m_xmmsClient->playlist.load(playlist());
	
	PlaylistWindow::itemEntered(item);
}

bool PlaylistViewer::getCurrentPlaylist(const std::string& playlist)
{
	m_currentPlaylist=playlist;
	return true;
}

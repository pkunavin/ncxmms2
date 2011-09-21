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


#ifndef PLAYLISTVIEWER_H
#define PLAYLISTVIEWER_H

#include "playlistwindow.h"

namespace ncxmms2
{
	class PlaylistViewer : public PlaylistWindow
	{
	public:
		PlaylistViewer(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent=0);
		
	protected:
		virtual void itemEntered(int item);
		
	private:
		Xmms::Client *m_xmmsClient;
		std::string m_currentPlaylist;
		bool getCurrentPlaylist(const std::string& playlist);
		
	};
}

#endif // PLAYLISTVIEWER_H

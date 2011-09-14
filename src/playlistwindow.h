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

#ifndef PLAYLISTWINDOW_H
#define PLAYLISTWINDOW_H

#include <vector>
#include <string>
#include <boost/unordered_map.hpp>
#include <xmmsclient/xmmsclient++.h>

#include "song.h"
#include "lib/abstractitemview.h"

namespace ncxmms2
{
	class PlaylistWindow : public AbstractItemView
	{
	public:
		PlaylistWindow(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent=0);
		~PlaylistWindow();
		
		virtual void keyPressedEvent(const KeyEvent& keyEvent);
		
	protected:
		virtual void drawItem(int item);
		virtual int itemsCount() const;
		virtual void itemEntered(int item);
	
	private:
		Xmms::Client *m_xmmsClient;
	
		boost::unordered_map<int, Song> m_songInfos;
		std::vector<int> m_idList;
		std::string m_activePlaylist;
		int m_currentPosition;
		Xmms::Playback::Status m_playbackStatus;
		
		void updateWindowTitle();
		int m_totalDuration;
		
		// Callbacks
		bool getActivePlaylist(const std::string& playlist);
		bool getEntries(const Xmms::List<int>& list);
		bool getSongInfo(int position, const Xmms::PropDict& info);
		bool processPlaylistChange(const Xmms::Dict& change);
		bool getCurrentPosition(const Xmms::Dict& position);
		bool handlePlaylistRename(const Xmms::Dict& change);
		bool handleSongInfoUpdate(const int& id);
		bool getPlaybackStatus(const Xmms::Playback::Status& status);
	};
}

#endif // PLAYLISTWINDOW_H

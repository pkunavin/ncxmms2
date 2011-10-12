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

#ifndef PLAYBACKSTATUSWINDOW_H
#define PLAYBACKSTATUSWINDOW_H

#include <xmmsclient/xmmsclient++.h>

#include "song.h"
#include "lib/timer.h"
#include "lib/window.h"

namespace ncxmms2
{
	class PlaybackStatusWindow : public Window
	{
	public:
		PlaybackStatusWindow(Xmms::Client *client, int lines, int cols, int yPos, int xPos, Window *parent=0);
	
		void showMessage(const std::string& message);
		
		Xmms::Playback::Status playbackStatus() const;
		
	protected:
		virtual void showEvent();
		
	private:
		Xmms::Client *m_xmmsClient;
		Xmms::Playback::Status m_playbackStatus;
		Song m_currentSong;
		int m_playbackPlaytime;
		
		bool m_isShowingMessage;
		std::string m_message;
		Timer m_messageTimer;
		void hideMessage();
		
		// Callbacks
		bool getPlaybackStatus(const Xmms::Playback::Status& status);
		bool getCurrentId(const int& id);
		bool getCurrentIdInfo(const Xmms::PropDict& info);
		bool getPlaytime(const int& playtime);
		bool handleIdInfoChanged(const int& id);
	};
}

#endif // PLAYBACKSTATUSWINDOW_H

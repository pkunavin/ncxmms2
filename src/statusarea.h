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

#ifndef STATUSAREA_H
#define STATUSAREA_H

#include <xmmsclient/xmmsclient++/playback.h>
#include "song.h"
#include "lib/window.h"
#include "lib/timer.h"

namespace ncxmms2 
{
	class PlaybackProgressBar;
	class StackedWindow;
	
	class StatusArea : public Window
	{
	public:
		StatusArea(Xmms::Client *client, int lines, int cols, int yPos, int xPos, Window *parent=0);
		~StatusArea();
		
		static void showMessage(const std::string& message);
		
		Xmms::Playback::Status playbackStatus() const;
		
		void resizeEvent(const Size &size);
	
	private:
		static StatusArea *inst;
		
		PlaybackProgressBar *m_playbackProgressBar;
		
		enum StackedWindows
		{
			StackedPlaybackStatusWindow,
			StackedMessageWindow
		};
		
		StackedWindow *m_stackedWindow;
		Timer m_timer;
		
		void _showMessage(const std::string& message);
	};
	
}

#endif // STATUSAREA_H

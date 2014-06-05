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

#ifndef PLAYBACKSTATUSWINDOW_H
#define PLAYBACKSTATUSWINDOW_H

#include <xmmsclient/xmmsclient++.h>
#include "../song.h"
#include "../songdisplayformatparser.h"
#include "../lib/window.h"

namespace ncxmms2 {

class PlaybackStatusWindow : public Window
{
public:
    PlaybackStatusWindow(Xmms::Client *client, int xPos, int yPos, int cols, Window *parent = nullptr);

    Xmms::Playback::Status playbackStatus() const;

    // Signals
    NCXMMS2_SIGNAL(playtimeChanged, int)
    NCXMMS2_SIGNAL(currentSongChanged, const Song&)

protected:
    virtual void paint(const Rectangle& rect);

private:
    Xmms::Client *m_xmmsClient;
    Xmms::Playback::Status m_playbackStatus;
    Song m_currentSong;
    int m_playbackPlaytime;
    bool m_useTerminalWindowTitle;
    SongDisplayFormatParser m_songDisplayFormatter;
    SongDisplayFormatParser m_terminalWindowTitleFormatter;
    
    void updateTerminalWindowTitle();
     
    // Callbacks
    bool getPlaybackStatus(const Xmms::Playback::Status& status);
    bool getCurrentId(const int& id);
    bool getCurrentIdInfo(const Xmms::PropDict& info);
    bool getPlaytime(const int& playtime);
    bool handleIdInfoChanged(const int& id);
};
} // ncxmms2

#endif // PLAYBACKSTATUSWINDOW_H

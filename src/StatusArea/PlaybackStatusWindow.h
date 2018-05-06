/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#include "../XmmsUtils/Client.h"
#include "../Song.h"
#include "../SongDisplayFormatParser.h"
#include "../lib/Window.h"

namespace ncxmms2 {

class PlaybackStatusWindow : public Window
{
public:
    PlaybackStatusWindow(xmms2::Client *client, int xPos, int yPos, int cols, Window *parent = nullptr);

    xmms2::PlaybackStatus playbackStatus() const;

    // Signals
    NCXMMS2_SIGNAL(currentSongChanged, const Song&)

protected:
    virtual void paint(const Rectangle& rect);

private:
    xmms2::Client *m_xmmsClient;
    xmms2::PlaybackStatus m_playbackStatus;
    Song m_currentSong;
    std::string m_playbackPlaytime;
    bool m_useTerminalWindowTitle;
    SongDisplayFormatParser m_songDisplayFormatter;
    SongDisplayFormatParser m_terminalWindowTitleFormatter;
    
    void updateTerminalWindowTitle();
     
    // Callbacks
    void getPlaybackStatus(const xmms2::Expected<xmms2::PlaybackStatus>& status);
    void getCurrentId(const xmms2::Expected<int>& id);
    void getCurrentIdInfo(const xmms2::Expected<xmms2::PropDict>& info);
    void getPlaytime(const xmms2::Expected<int>& playtime);
    void handleIdInfoChanged(const xmms2::Expected<int>& id);
};
} // ncxmms2

#endif // PLAYBACKSTATUSWINDOW_H

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

#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include "../xmmsutils/client.h"
#include "../listviewappintegrated/listviewappintegrated.h"

namespace ncxmms2 {

class PlaylistView : public ListViewAppIntegrated
{
public:
    PlaylistView(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    void setPlaylist(const std::string& playlist);

    void setDisplayFormat(const std::string& format);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

    // Signals:
    NCXMMS2_SIGNAL(showSongInfo, int)
    
private:
    xmms2::Client *m_xmmsClient;
    std::string m_activePlaylist;
    xmms2::PlaybackStatus m_playbackStatus;

    void getActivePlaylist(StringRef playlist);
    void getPlaybackStatus(xmms2::PlaybackStatus status);
    void onItemEntered(int item);
    void addPath(const std::string& path);
    void addFile(const std::string& path);
    void addUrl(const std::string& url);
    void selectSongsByRegExp();
    void unselectSongsByRegExp();
    void removeSelectedSongs();
    void moveSelectedSongs();
};

} // ncxmms2

#endif // PLAYLISTVIEW_H

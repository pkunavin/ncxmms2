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

#include <xmmsclient/xmmsclient++.h>
#include "../listviewappintegrated/listviewappintegrated.h"

namespace ncxmms2 {

class PlaylistView : public ListViewAppIntegrated
{
public:
    PlaylistView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    void setPlaylist(const std::string& playlist);

    void setDisplayFormat(const std::string& format);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

private:
    Xmms::Client *m_xmmsClient;
    std::string m_activePlaylist;
    Xmms::Playback::Status m_playbackStatus;

    bool getActivePlaylist(const std::string& playlist);
    bool getPlaybackStatus(const Xmms::Playback::Status& status);
    void onItemEntered(int item);
    void addPath(const std::string& path);
    void addFile(const std::string& path);
    void addUrl(const std::string& url);
};

} // ncxmms2

#endif // PLAYLISTVIEW_H

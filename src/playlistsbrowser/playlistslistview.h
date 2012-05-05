/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef PLAYLISTSLISTVIEW_H
#define PLAYLISTSLISTVIEW_H

#include <vector>
#include <xmmsclient/xmmsclient++.h>

#include "../lib/abstractitemview.h"
#include "../lib/lineedit.h"

namespace ncxmms2 {

class PlaylistsListView : public AbstractItemView
{
public:
    PlaylistsListView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent &keyEvent);

    std::string playlist(int item) const;

protected:
    virtual void drawItem(int item);
    virtual int itemsCount() const;
    virtual void itemEntered(int item);

private:
    Xmms::Client *m_xmmsClient;
    std::vector<std::string> m_playlists;
    std::string m_currentPlaylist;

    // Callbacks
    bool getPlaylists(const Xmms::List<std::string>& playlists);
    bool getCurrentPlaylist(const std::string& playlist);
    bool handlePlaylistsChange(const Xmms::Dict& change);

    void createPlaylist(const std::string& playlist);
    void renamePlaylist(const std::string& oldName, const std::string& newName);
};
} // ncxmms2

#endif // PLAYLISTSLISTVIEW_H

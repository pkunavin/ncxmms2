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

#ifndef PLAYLISTSLISTMODEL_H
#define PLAYLISTSLISTMODEL_H

#include <vector>
#include "../XmmsUtils/Result.h"
#include "../lib/ListModel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class PlaylistsListModel : public ListModel
{
public:
    PlaylistsListModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;
    
    void init();
    
    const std::string& playlist(int item) const;
    const std::string& currentPlaylist() const;
    bool playlistExists(const std::string& playlist) const;
    int indexOf(const std::string& playlist) const;

    // Signals
    NCXMMS2_SIGNAL(playlistAdded, const std::string&, int)

private:
    xmms2::Client *m_xmmsClient;
    std::vector<std::string> m_playlists;
    std::string m_currentPlaylist;

    // Callbacks
    void getPlaylists(const xmms2::Expected<xmms2::List<StringRef>>& playlists);
    void getCurrentPlaylist(const xmms2::Expected<StringRef>& playlist);
    void handlePlaylistsChange(const xmms2::CollectionChangeEvent& change);
};
} // ncxmms2

#endif // PLAYLISTSLISTMODEL_H

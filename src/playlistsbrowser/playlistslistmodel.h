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

#ifndef PLAYLISTSLISTMODEL_H
#define PLAYLISTSLISTMODEL_H

#include <vector>
#include "../lib/listmodel.h"

namespace Xmms {
class Client;
class Dict;
template <class T> class List;
}

namespace ncxmms2 {

class PlaylistsListModel : public ListModel
{
public:
    PlaylistsListModel(Xmms::Client *xmmsClient, Object *parent = nullptr);

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    const std::string& playlist(int item) const;
    bool playlistExists(const std::string& playlist) const;

private:
    Xmms::Client *m_xmmsClient;
    std::vector<std::string> m_playlists;
    std::string m_currentPlaylist;

    // Callbacks
    bool getPlaylists(const Xmms::List<std::string>& playlists);
    bool getCurrentPlaylist(const std::string& playlist);
    bool handlePlaylistsChange(const Xmms::Dict& change);
};
} // ncxmms2

#endif // PLAYLISTSLISTMODEL_H

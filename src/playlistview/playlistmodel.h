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

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <vector>
#include <string>
#include <unordered_map>

#include "../song.h"
#include "../xmmsutils/types.h"
#include "../lib/listmodel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class PlaylistModel : public ListModel
{
public:
    PlaylistModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    void setPlaylist(const std::string& playlist);
    const std::string& playlist() const;

    virtual int itemsCount() const;
    virtual void data(int item, ListModelItemData *itemData) const;

    const Song& song(int item) const;
    int currentSongItem() const;

    int totalDuration() const;

    void setLazyLoadPlaylist(bool enable);
    
    // Signals
    NCXMMS2_SIGNAL(playlistRenamed)
    NCXMMS2_SIGNAL(activeSongPositionChanged, int)
    NCXMMS2_SIGNAL(totalDurationChanged)

private:
    xmms2::Client *m_xmmsClient;

    bool m_lazyLoadPlaylist;
    
    std::unordered_map<int, Song> m_songInfos;
    std::vector<int> m_idList;
    std::string m_playlist;
    int m_currentPosition;

    int m_totalDuration;

    // Callbacks
    void getEntries(const xmms2::List<int>& entries);
    void getEntriesOrder(const xmms2::List<int>& entries);
    void getSongInfo(int position, const xmms2::PropDict& info);
    void processPlaylistChange(const xmms2::PlaylistChangeEvent& change);
    void getCurrentPosition(const xmms2::Dict& position);
    void handlePlaylistRename(const xmms2::CollectionChangeEvent& change);
    void handleSongInfoUpdate(int id);
};
} // ncxmms2

#endif // PLAYLISTMODEL_H

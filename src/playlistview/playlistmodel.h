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
#include <boost/unordered_map.hpp>

#include "../song.h"
#include "../lib/listmodel.h"

namespace Xmms {
class Client;
class Dict;
class PropDict;
template <class T> class List;
}

namespace ncxmms2 {

class PlaylistModel : public ListModel
{
public:
    PlaylistModel(Xmms::Client *xmmsClient, Object *parent = nullptr);

    void setPlaylist(const std::string& playlist);
    const std::string& playlist() const;

    virtual int itemsCount() const;
    virtual void data(int item, ListModelItemData *itemData) const;

    const Song& song(int item) const;
    int currentSongItem() const;

    int totalDuration() const;

    // Signals
    NCXMMS2_SIGNAL(playlistRenamed)
    NCXMMS2_SIGNAL(activeSongPositionChanged, int)
    NCXMMS2_SIGNAL(totalDurationChanged)

private:
    Xmms::Client *m_xmmsClient;

    boost::unordered_map<int, Song> m_songInfos;
    std::vector<int> m_idList;
    std::string m_playlist;
    int m_currentPosition;

    int m_totalDuration;

    // Callbacks
    bool getEntries(const Xmms::List<int>& list);
    bool getSongInfo(int position, const Xmms::PropDict& info);
    bool processPlaylistChange(const Xmms::Dict& change);
    bool getCurrentPosition(const Xmms::Dict& position);
    bool handlePlaylistRename(const Xmms::Dict& change);
    bool handleSongInfoUpdate(const int& id);
};
} // ncxmms2

#endif // PLAYLISTMODEL_H

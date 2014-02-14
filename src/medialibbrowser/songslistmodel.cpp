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

#include <xmmsclient/xmmsclient++.h>
#include <assert.h>

#include "songslistmodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

SongsListModel::SongsListModel(Xmms::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    m_sortingOrder = {"tracknr", "id"};
}

void SongsListModel::setAlbumByArtist(const std::string& artist, const std::string& album)
{
    m_artist = artist;
    m_album = album;
    refresh();
}

int SongsListModel::id(int item) const
{
    assert(item >= 0 && (size_t)item < m_songs.size());
    return m_songs[item].id;
}

const std::string &SongsListModel::title(int item) const
{
    assert(item >= 0 && (size_t)item < m_songs.size());
    return m_songs[item].title;
}

const std::list<std::string>& SongsListModel::sortingOrder() const
{
    return m_sortingOrder;
}

void SongsListModel::data(int item, ListModelItemData *itemData) const
{
    itemData->textPtr = &m_songs[item].title;
}

int SongsListModel::itemsCount() const
{
    return m_songs.size();
}

void SongsListModel::refresh()
{
    if (!m_artist.empty() && !m_album.empty()) { //FIXME: handle empty artist and album
        Xmms::Coll::Universe     allMedia;
        Xmms::Coll::Equals       allByArtist(allMedia, "artist", m_artist, true);
        const Xmms::Coll::Equals albumByArtist(allByArtist, "album", m_album, true);

        const std::list<std::string> fetch = {"id", "title"};

        m_xmmsClient->collection.queryInfos(albumByArtist, fetch, m_sortingOrder)(
            boost::bind(&SongsListModel::getSongsList, this, m_artist, m_album, _1)
        );
    }

    m_songs.clear();
    reset();
}

bool SongsListModel::getSongsList(const std::string& artist,
                                  const std::string& album,
                                  const Xmms::List<Xmms::Dict>& list)
{
    if (artist != m_artist || album != m_album)
        return true;

    m_songs.clear();
    for (auto it = list.begin(), it_end = list.end(); it != it_end; ++it) {
        try {
            const int id = (*it).get<int>("id");
            const std::string title = (*it).get<std::string>("title"); // TODO: Handle empty title
            m_songs.push_back(SongData(id, title));
        }
        catch (...) {
            continue;
        }
    }

    reset();
    return true;
}

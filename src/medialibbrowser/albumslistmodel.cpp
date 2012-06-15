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

#include <xmmsclient/xmmsclient++.h>
#include <assert.h>

#include "albumslistmodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

AlbumsListModel::AlbumsListModel(Xmms::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    m_sortingOrder = {"date", "album"};
}

void AlbumsListModel::setArtist(const std::string& artist)
{
    m_artist = artist;
    refresh();
}

const std::string &AlbumsListModel::artist() const
{
    return m_artist;
}

const std::string& AlbumsListModel::album(int item) const
{
    assert(item >= 0 && (size_t)item < m_albums.size());
    return m_albums[item];
}

const std::list<std::string>& AlbumsListModel::sortingOrder() const
{
    return m_sortingOrder;
}

void AlbumsListModel::data(int item, ListModelItemData *itemData) const
{
    itemData->textPtr = &m_albums[item];
}

int AlbumsListModel::itemsCount() const
{
    return m_albums.size();
}

void AlbumsListModel::refresh()
{
    if (!m_artist.empty()) { //FIXME: handle empty artist
        Xmms::Coll::Universe     allMedia;
        const Xmms::Coll::Equals allByArtist(allMedia, "artist", m_artist, true);

        const std::list<std::string>   fetch = {"album"};
        const std::list<std::string> groupBy = {"album"};

        m_xmmsClient->collection.queryInfos(allByArtist, fetch, m_sortingOrder, 0, 0, groupBy)(
            boost::bind(&AlbumsListModel::getAlbumsList, this, m_artist, _1)
        );
    }

    m_albums.clear();
    reset();
}

bool AlbumsListModel::getAlbumsList(const std::string& artist, const Xmms::List<Xmms::Dict>& list)
{
    if (artist != m_artist)
        return true;

    m_albums.clear();
    for (auto it = list.begin(), it_end = list.end(); it != it_end; ++it) {
        try {
            m_albums.push_back((*it).get<std::string>("album"));
        }
        catch (...) {
            continue;
        }
    }

    reset();
    return true;
}

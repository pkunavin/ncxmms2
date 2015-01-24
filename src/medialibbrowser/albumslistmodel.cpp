/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <assert.h>

#include "albumslistmodel.h"
#include "../statusarea/statusarea.h"
#include "../xmmsutils/client.h"
#include "../log.h"

#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

AlbumsListModel::AlbumsListModel(xmms2::Client *xmmsClient, Object *parent) :
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

const std::vector<std::string>& AlbumsListModel::sortingOrder() const
{
    return m_sortingOrder;
}

void AlbumsListModel::data(int item, ListModelItemData *itemData) const
{
    static const std::string unknownAlbum = "Unknown album";
    itemData->textPtr = !m_albums[item].empty() ? &m_albums[item] : &unknownAlbum;
}

int AlbumsListModel::itemsCount() const
{
    return m_albums.size();
}

void AlbumsListModel::refresh()
{
    m_albums.clear();
    
    xmms2::Collection albums = xmms2::Collection::allByArtist(m_artist);
    const std::vector<std::string>    fetch = {"album"};
    const std::vector<std::string>& groupBy = fetch;
    
    m_xmmsClient->collectionQueryInfos(albums, fetch, m_sortingOrder, groupBy)(
        &AlbumsListModel::getAlbumsList, this, m_artist, std::placeholders::_1);
    
    reset();
}

void AlbumsListModel::getAlbumsList(const std::string& artist,
                                    const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
{
    if (list.isError()) {
        StatusArea::showMessage("Failed to get albums for \"%s\": %s!", artist, list.error().toString());
        NCXMMS2_LOG_ERROR("%s", list.error().toString().c_str());
        return;
    }
    
    if (artist != m_artist)
        return;

    m_albums.clear();
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        StringRef album = dict.value<StringRef>("album", "");
        m_albums.emplace_back(album.c_str());
    }
    reset();
}

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

#include "SongsListModel.h"
#include "../StatusArea/StatusArea.h"
#include "../XmmsUtils/Client.h"
#include "../Log.h"

#include "../lib/ListModelItemData.h"

using namespace ncxmms2;

SongsListModel::SongsListModel(xmms2::Client *xmmsClient, Object *parent) :
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

const std::vector<std::string>& SongsListModel::sortingOrder() const
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
    xmms2::Collection songs = xmms2::Collection::albumByArtist(m_artist, m_album);
    const std::vector<std::string> fetch = {"id", "title", "url"};
    
    m_xmmsClient->collectionQueryInfos(songs, fetch, m_sortingOrder)(
        &SongsListModel::getSongsList, this, m_artist, m_album, std::placeholders::_1);
    
    m_songs.clear();
    reset();
}

void SongsListModel::getSongsList(const std::string& artist,
                                  const std::string& album,
                                  const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
{
    if (list.isError()) {
        StatusArea::showMessage("Failed to get songs of \"%s\":\"%s\": %s!", artist, album, list.error());
        NCXMMS2_LOG_ERROR("%s", list.error());
        return;
    }
    
    if (artist != m_artist || album != m_album)
        return;

    m_songs.clear();
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        std::string title = dict.value<std::string>("title");
        if (NCXMMS2_UNLIKELY(title.empty())) {
            StringRef url = dict.value<StringRef>("url");
            if (url.isNull())
                continue;
            title = xmms2::getFileNameFromUrl(xmms2::decodeUrl(url.c_str()));
        }
        int id = dict.value<int>("id");
        if (NCXMMS2_UNLIKELY(id == 0))
            continue;
        m_songs.push_back({id, std::move(title)});
    }

    reset();
}

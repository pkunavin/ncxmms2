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

#include "ArtistsListModel.h"
#include "../XmmsUtils/Client.h"
#include "../Log.h"

#include "../lib/ListModelItemData.h"

using namespace ncxmms2;

ArtistsListModel::ArtistsListModel(xmms2::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    
}

const std::string& ArtistsListModel::artist(int item) const
{
    assert(item >= 0 && (size_t)item < m_artists.size());
    return m_artists[item];
}

void ArtistsListModel::data(int item, ListModelItemData *itemData) const
{
    static const std::string unknownArtist = "Unknown artist";
    itemData->textPtr = !m_artists[item].empty() ? &m_artists[item] : &unknownArtist;
}

int ArtistsListModel::itemsCount() const
{
    return m_artists.size();
}

void ArtistsListModel::refresh()
{
    m_artists.clear();
    reset();

    const xmms2::Collection allMedia = xmms2::Collection::universe();
    const std::vector<std::string>  fetch = {"artist"};
    const std::vector<std::string>& order = fetch;
    const std::vector<std::string>& groupBy = fetch;

    m_xmmsClient->collectionQueryInfos(allMedia, fetch, order, groupBy)(&ArtistsListModel::getArtistsList, this);
}

void ArtistsListModel::getArtistsList(const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
{
    if (list.isError()) {
        NCXMMS2_LOG_ERROR("%s", list.error());
        return;
    }
    
    m_artists.clear();
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        StringRef artist = dict.value<StringRef>("artist", "");
        m_artists.emplace_back(artist.c_str());
    }
    reset();
}

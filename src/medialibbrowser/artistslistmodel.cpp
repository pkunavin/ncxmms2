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
#include <list>

#include "artistslistmodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

ArtistsListModel::ArtistsListModel(Xmms::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    refresh();
}

const std::string& ArtistsListModel::artist(int item) const
{
    assert(item >= 0 && (size_t)item < m_artists.size());
    return m_artists[item];
}

void ArtistsListModel::data(int item, ListModelItemData *itemData) const
{
    itemData->textPtr = &m_artists[item];
}

int ArtistsListModel::itemsCount() const
{
    return m_artists.size();
}

void ArtistsListModel::refresh()
{
    m_artists.clear();
    reset();

    const Xmms::Coll::Universe allMedia;
    const std::list<std::string>   fetch = {"artist"};
    const std::list<std::string>   order = {"artist"};
    const std::list<std::string> groupBy = {"artist"};

    m_xmmsClient->collection.queryInfos(allMedia, fetch, order, 0, 0, groupBy)(
        Xmms::bind(&ArtistsListModel::getArtistsList, this)
    );
}

bool ArtistsListModel::getArtistsList(const Xmms::List<Xmms::Dict>& list)
{
    m_artists.clear();
    for (auto it = list.begin(), it_end = list.end(); it != it_end; ++it) {
        try {
            m_artists.push_back((*it).get<std::string>("artist"));
        }
        catch (...) {
            continue;
        }
    }

    reset();
    return true;
}

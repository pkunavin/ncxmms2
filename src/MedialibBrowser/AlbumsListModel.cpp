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

#include <assert.h>

#include "AlbumsListModel.h"
#include "../StatusArea/StatusArea.h"
#include "../XmmsUtils/Client.h"
#include "../Log.h"

#include "../lib/ListModelItemData.h"

using namespace ncxmms2;

AlbumsListModel::AlbumsListModel(xmms2::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient),
    m_filterTag(Song::Tag::Artist)
{
    m_sortingOrder = {"date", "artist", "album"};
}

void AlbumsListModel::setFilterByTag(Song::Tag tag, const std::string& tagValue)
{
    m_filterTag = tag;
    m_filterTagValue = tagValue;
    refresh();
}

const std::string& AlbumsListModel::album(int item) const
{
    assert(item >= 0 && (size_t)item < m_albums.size());
    return m_albums[item].album;
}

xmms2::Collection AlbumsListModel::getAlbumsCollection(Song::Tag tag, const std::string& tagValue)
{
    xmms2::Collection allMedia = xmms2::Collection::universe();
    if (tagValue.empty()) {
        xmms2::Collection hasFilterTagColl(xmms2::Collection::Type::Has);
        hasFilterTagColl.setAttribute("field", Song::getTagKey(tag).c_str());
        hasFilterTagColl.addOperand(allMedia);

        xmms2::Collection hasNoFilterTagColl(xmms2::Collection::Type::Complement);
        hasNoFilterTagColl.addOperand(hasFilterTagColl);

        xmms2::Collection coll(xmms2::Collection::Type::Intersection);
        coll.addOperand(hasNoFilterTagColl);
        coll.addOperand(allMedia);
        return coll;
    }

    xmms2::Collection coll(xmms2::Collection::Type::Match); //NOTE: Don't khow why Type::Equals doesn't work
    coll.setAttribute("field", Song::getTagKey(tag).c_str());
    coll.setAttribute("case-sensitive", "true");
    coll.setAttribute("value", tagValue);
    coll.addOperand(allMedia);
    return coll;
}

const std::vector<std::string>& AlbumsListModel::sortingOrder() const
{
    return m_sortingOrder;
}

void AlbumsListModel::data(int item, ListModelItemData *itemData) const
{
    static const std::string unknown = "<Unknown>";
    if (m_filterTag == Song::Tag::Year || m_filterTag == Song::Tag::Genre) {
        itemData->text.append("[");
        itemData->text.append(!m_albums[item].artist.empty() ? m_albums[item].artist : unknown);
        itemData->text.append("] ");
        itemData->text.append(!m_albums[item].album.empty() ? m_albums[item].album : unknown);
    } else {
        itemData->textPtr = !m_albums[item].album.empty() ? &m_albums[item].album : &unknown;
    }
}

int AlbumsListModel::itemsCount() const
{
    return m_albums.size();
}

void AlbumsListModel::refresh()
{
    m_albums.clear();
    
    const std::vector<std::string>    fetch = {"artist", "album"};
    const std::vector<std::string>& groupBy = {"album"};
    
    m_xmmsClient->collectionQueryInfos(getAlbumsCollection(), fetch, m_sortingOrder, groupBy)(
        &AlbumsListModel::getAlbumsList, this);
    
    reset();
}

xmms2::Collection AlbumsListModel::getAlbumsCollection() const
{
    return getAlbumsCollection(m_filterTag, m_filterTagValue);
}

void AlbumsListModel::getAlbumsList(const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
{
    if (list.isError()) {
        StatusArea::showMessage("Failed to get albums %s!", list.error());
        NCXMMS2_LOG_ERROR("%s", list.error());
        return;
    }

    m_albums.clear();
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        StringRef artist = dict.value<StringRef>("artist", "");
        StringRef album = dict.value<StringRef>("album", "");
        m_albums.push_back({artist.c_str(), album.c_str()});
    }
    reset();
}

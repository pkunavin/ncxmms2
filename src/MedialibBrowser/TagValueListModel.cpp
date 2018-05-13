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

#include "TagValueListModel.h"
#include "../XmmsUtils/Client.h"
#include "../Log.h"

#include "../lib/ListModelItemData.h"

using namespace ncxmms2;

TagValueListModel::TagValueListModel(xmms2::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient),
    m_tag(Song::Tag::Artist)
{

}

void TagValueListModel::setTag(Song::Tag tag)
{
    if (m_tag == tag)
        return;
    m_tag = tag;
    m_tagValues.clear();
    reset();
}

const std::string &TagValueListModel::tagValue(int item) const
{
    assert(item >= 0 && (size_t)item < m_tagValues.size());
    return m_tagValues[item];
}

void TagValueListModel::data(int item, ListModelItemData *itemData) const
{
    static const std::string unknown = "<Unknown>";
    itemData->textPtr = !m_tagValues[item].empty() ? &m_tagValues[item] : &unknown;
}

int TagValueListModel::itemsCount() const
{
    return m_tagValues.size();
}

void TagValueListModel::refresh()
{
    m_tagValues.clear();
    reset();

    const xmms2::Collection allMedia = xmms2::Collection::universe();
    const std::vector<std::string>  fetch = {Song::getTagKey(m_tag).c_str()};
    const std::vector<std::string>& order = fetch;
    const std::vector<std::string>& groupBy = fetch;

    m_xmmsClient->collectionQueryInfos(allMedia, fetch, order, groupBy)(&TagValueListModel::getTagValueList, this);
}

void TagValueListModel::getTagValueList(const xmms2::Expected<xmms2::List<xmms2::Dict>> &list)
{
    if (list.isError()) {
        NCXMMS2_LOG_ERROR("%s", list.error());
        return;
    }

    m_tagValues.clear();
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        StringRef artist = dict.value<StringRef>(Song::getTagKey(m_tag).c_str(), "");
        m_tagValues.emplace_back(artist.c_str());
    }
    reset();
}

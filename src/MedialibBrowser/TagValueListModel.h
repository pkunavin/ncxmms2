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

#ifndef TAGVALUELISTMODEL_H
#define TAGVALUELISTMODEL_H

#include <vector>
#include "../Song.h"
#include "../XmmsUtils/Result.h"
#include "../lib/ListModel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class TagValueListModel : public ListModel
{
public:
    TagValueListModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    void setTag(Song::Tag tag);

    Song::Tag tag() const {return m_tag;}

    const std::string& tagValue(int item) const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    xmms2::Client *m_xmmsClient;
    Song::Tag m_tag;
    std::vector<std::string> m_tagValues;

    void getTagValueList(const xmms2::Expected<xmms2::List<xmms2::Dict>>& list);
};
} // ncxmms2

#endif // TAGVALUELISTMODEL_H

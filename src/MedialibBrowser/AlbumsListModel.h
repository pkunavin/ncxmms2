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

#ifndef ALBUMSLISTMODEL_H
#define ALBUMSLISTMODEL_H

#include <vector>
#include "../Song.h"
#include "../XmmsUtils/Result.h"
#include "../lib/ListModel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class AlbumsListModel : public ListModel
{
public:
    AlbumsListModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    void setFilterByTag(Song::Tag tag, const std::string & tagValue);

    const std::string& album(int item) const;

    static xmms2::Collection getAlbumsCollection(Song::Tag tag, const std::string & tagValue);
    xmms2::Collection getAlbumsCollection() const;

    const std::vector<std::string>& sortingOrder() const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    xmms2::Client *m_xmmsClient;

    Song::Tag m_filterTag;
    std::string m_filterTagValue;

    std::vector<std::string> m_sortingOrder;

    struct AlnumData
    {
        std::string artist;
        std::string album;
    };
    std::vector<AlnumData> m_albums;

    void getAlbumsList(const xmms2::Expected<xmms2::List<xmms2::Dict>>& list);
};
} // ncxmms2

#endif // ALBUMSLISTMODEL_H

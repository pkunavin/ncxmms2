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

#ifndef ARTISTSLISTMODEL_H
#define ARTISTSLISTMODEL_H

#include <vector>
#include "../xmmsutils/result.h"
#include "../lib/listmodel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class ArtistsListModel : public ListModel
{
public:
    ArtistsListModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    const std::string& artist(int item) const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    xmms2::Client *m_xmmsClient;
    std::vector<std::string> m_artists;
    void getArtistsList(const xmms2::Expected<xmms2::List<xmms2::Dict>>& list);
};
} // ncxmms2

#endif // ARTISTSLISTMODEL_H

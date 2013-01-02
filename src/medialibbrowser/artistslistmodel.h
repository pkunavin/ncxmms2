/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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
#include "../lib/listmodel.h"

namespace Xmms {
class Client;
class Dict;
template <class T> class List;
}

namespace ncxmms2 {

class ArtistsListModel : public ListModel
{
public:
    ArtistsListModel(Xmms::Client *xmmsClient, Object *parent = nullptr);

    const std::string& artist(int item) const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    Xmms::Client *m_xmmsClient;
    std::vector<std::string> m_artists;
    bool getArtistsList(const Xmms::List<Xmms::Dict>& list);
};
} // ncxmms2

#endif // ARTISTSLISTMODEL_H

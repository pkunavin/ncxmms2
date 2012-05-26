/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef SONGSLISTMODEL_H
#define SONGSLISTMODEL_H

#include <vector>
#include <list>
#include "../lib/listmodel.h"

namespace Xmms {
class Client;
class Dict;
template <class T> class List;
}

namespace ncxmms2 {

class SongsListModel : public ListModel
{
public:
    SongsListModel(Xmms::Client *xmmsClient, Object *parent = nullptr);

    void setAlbumByArtist(const std::string& artist, const std::string& album);

    int id(int item) const;
    const std::string& title(int item) const;

    const std::list<std::string>& sortingOrder() const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

private:
    Xmms::Client *m_xmmsClient;

    class SongData
    {
    public:
        SongData(int _id, const std::string& _title) :
            id(_id),
            title(_title){}

        int id;
        std::string title;
    };

    std::list<std::string> m_sortingOrder;
    std::vector<SongData> m_songs;

    bool getSongsList(const Xmms::List<Xmms::Dict>& list);
};
} // ncxmms2

#endif // SONGSLISTMODEL_H

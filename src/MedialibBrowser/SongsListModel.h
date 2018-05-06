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

#ifndef SONGSLISTMODEL_H
#define SONGSLISTMODEL_H

#include <vector>
#include "../XmmsUtils/Result.h"
#include "../lib/ListModel.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class SongsListModel : public ListModel
{
public:
    SongsListModel(xmms2::Client *xmmsClient, Object *parent = nullptr);

    void setAlbumByArtist(const std::string& artist, const std::string& album);

    int id(int item) const;
    const std::string& title(int item) const;

    const std::vector<std::string>& sortingOrder() const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    xmms2::Client *m_xmmsClient;
    std::string m_artist;
    std::string m_album;

    struct SongData
    {
        int id;
        std::string title;
    };

    std::vector<std::string> m_sortingOrder;
    std::vector<SongData> m_songs;

    void getSongsList(const std::string& artist,
                      const std::string& album,
                      const xmms2::Expected<xmms2::List<xmms2::Dict>>& list);
};
} // ncxmms2

#endif // SONGSLISTMODEL_H

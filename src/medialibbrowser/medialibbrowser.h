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

#ifndef MEDIALIBBROWSER_H
#define MEDIALIBBROWSER_H

#include "../lib/window.h"

namespace Xmms {
class Client;
class Dict;
template <class T> class List;
}

namespace ncxmms2 {

class ListView;

class MedialibBrowser : public Window
{
public:
    MedialibBrowser(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resize(const Size& size);

protected:
    virtual void paint(const Rectangle& rect);

private:
    Xmms::Client *m_xmmsClient;

    ListView *m_artistsListView;
    ListView *m_albumsListView;
    ListView *m_songsListView;

    void setAlbumsListViewArtist(int item);
    void setSongsListViewAlbum(int item);

    void activePlaylistAddSong(int item, bool beQuiet = false);
    void activePlaylistAddAlbum(int item, bool beQuiet = false);
    void activePlaylistAddArtist(int item, bool beQuiet = false);
    bool activePlaylistAddAlbums(const std::string& artist, const Xmms::List<Xmms::Dict>& list, bool beQuiet = false);
};
} // ncxmms2

#endif // MEDIALIBBROWSER_H

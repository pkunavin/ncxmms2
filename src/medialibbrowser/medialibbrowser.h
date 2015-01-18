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

#ifndef MEDIALIBBROWSER_H
#define MEDIALIBBROWSER_H

#include "../xmmsutils/result.h"
#include "../lib/window.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}
class ListView;

class MedialibBrowser : public Window
{
public:
    MedialibBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    
    // Signals:
    NCXMMS2_SIGNAL(showSongInfo, int)

protected:
    virtual void resizeChildren(const Size& size);
    virtual void paint(const Rectangle& rect);
    virtual void showEvent();

private:
    xmms2::Client *m_xmmsClient;

    ListView *m_artistsListView;
    ListView *m_albumsListView;
    ListView *m_songsListView;

    void setAlbumsListViewArtist(int item);
    void setSongsListViewAlbum(int item);

    void activePlaylistAddSong(int item, bool beQuiet = false);
    void activePlaylistAddAlbum(int item, bool beQuiet = false);
    void activePlaylistAddArtist(int item, bool beQuiet = false);
    void activePlaylistAddAlbums(const std::string& artist,
                                 const xmms2::Expected<xmms2::List<xmms2::Dict>>& list,
                                 bool beQuiet = false);
};
} // ncxmms2

#endif // MEDIALIBBROWSER_H

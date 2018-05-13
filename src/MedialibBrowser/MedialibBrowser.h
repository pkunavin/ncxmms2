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

#ifndef MEDIALIBBROWSER_H
#define MEDIALIBBROWSER_H

#include "../Song.h"
#include "../XmmsUtils/Result.h"
#include "../lib/Window.h"

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

    ListView *m_primaryTagListView;
    ListView *m_albumsListView;
    ListView *m_songsListView;

    enum
    {
      HeaderLines = 2
    };

    void askPrimaryListViewTag();
    void setPrimaryListViewTag(Song::Tag tag);
    std::string getPrimaryListViewName() const;
    std::string getAlbumsListViewName() const;

    void setAlbumsListViewFilterTag(int item);
    void setSongsListViewAlbum(int item);

    void activePlaylistAddSong(int item, bool beQuiet = false);
    void activePlaylistAddAlbum(int item, bool beQuiet = false);
    void activePlaylistAddByPrimaryTag(int item, bool beQuiet = false);
    void activePlaylistAddAlbums(Song::Tag primaryTag, const std::string& primaryTagValue,
                                 const xmms2::Expected<xmms2::List<xmms2::Dict>>& list, bool beQuiet = false);
    
    void activePlaylistPlaySong(int item);
    void activePlaylistPlayAlbum(int item);
    void activePlaylistPlayAlbumColl(const xmms2::Collection& songs, const std::vector<std::string>& sortingOrder);
    void activePlaylistAddAlbumColl(const xmms2::Collection& songs, const std::vector<std::string>& sortingOrder);
    void activePlaylistPlayByPrimaryTag(int item);
};
} // ncxmms2

#endif // MEDIALIBBROWSER_H

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

#ifndef PLAYLISTSBROWSER_H
#define PLAYLISTSBROWSER_H

#include "../lib/Window.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class PlaylistsListView;
class PlaylistView;

class PlaylistsBrowser : public Window
{
public:
    PlaylistsBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

    // Signals:
    NCXMMS2_SIGNAL(showSongInfo, int)
    
protected:
    virtual void resizeChildren(const Size& size);
    virtual void paint(const Rectangle& rect);

private:
    enum {PlaylistsListViewCols = 20};

    PlaylistsListView *m_plsListView;
    PlaylistView *m_plsViewer;
    
    void setPlsViewerPlaylist(int item);
    void emitShowSongInfo(int id);
};
} // ncxmms2

#endif // PLAYLISTSBROWSER_H

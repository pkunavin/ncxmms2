/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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

#include "../lib/window.h"

namespace Xmms {
class Client;
}

namespace ncxmms2 {

class PlaylistsListView;
class PlaylistView;

class PlaylistsBrowser : public Window
{
public:
    PlaylistsBrowser(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

protected:
    virtual void resizeChildren(const Size& size);
    virtual void paint(const Rectangle& rect);

private:
    enum {PlaylistsListViewCols = 20};

    void setPlsViewerPlaylist(int item);

    PlaylistsListView *m_plsListView;
    PlaylistView *m_plsViewer;
};
} // ncxmms2

#endif // PLAYLISTSBROWSER_H

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

#ifndef PLAYLISTSLISTVIEW_H
#define PLAYLISTSLISTVIEW_H

#include "../lib/listview.h"

namespace Xmms {
class Client;
}

namespace ncxmms2 {

class PlaylistsListView : public ListView
{
public:
    PlaylistsListView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

    const std::string& playlist(int item) const;

private:
    Xmms::Client *m_xmmsClient;

    void loadPlaylist(int item);
    void createPlaylist(const std::string& playlist);
    void renamePlaylist(const std::string& oldName, const std::string& newName);
};
} // ncxmms2

#endif // PLAYLISTSLISTVIEW_H

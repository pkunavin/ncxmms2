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

#ifndef SONGINFOWINDOW_H
#define SONGINFOWINDOW_H

#include "../lib/textview.h"

namespace ncxmms2 {

namespace xmms2 {
    class Client;
    class PropDict;
}

class SongInfoWindow : public TextView
{
public:
    SongInfoWindow(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);
    
    void showSongInfo(int id);
    
    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    
    // Signals:
    NCXMMS2_SIGNAL(hideRequested)
    
private:
    xmms2::Client *m_xmmsClient;
    int m_id;
    
    void getSongInfo(const ncxmms2::xmms2::PropDict& info);
    void handleSongInfoUpdate(int id);
};
} // ncxmms2

#endif // SONGINFOWINDOW_H

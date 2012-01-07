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

#ifndef HOTKEYS_H
#define HOTKEYS_H

namespace ncxmms2 {
namespace Hotkeys {
enum
{
    Quit = 'q',

    // Screens switching
    PlaylistScreen               = '2',
    LocalFileSystemBrowserScreen = '3',
    PlaylistsBrowserScreen       = '6',

    // Playback control
    PlaybackToggle       = 'P',
    PlaybackStop         = 's',
    PlaybackNext         = '>',
    PlaybackPrev         = '<',
    PlaybackSeekForward  = 'f',
    PlaybackSeekBackward = 'b'
};
} // Hotkeys
} // ncxmms2
#endif // HOTKEYS_H

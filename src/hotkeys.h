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

#ifndef HOTKEYS_H
#define HOTKEYS_H

#include "lib/keyevent.h"

namespace ncxmms2 {
namespace Hotkeys {

enum
{
    Quit = 'q'
};

namespace Playback {
enum
{
    Toggle       = 'P',
    Stop         = 's',
    Next         = '>',
    Prev         = '<',
    SeekForward  = 'f',
    SeekBackward = 'b'
};
}

namespace PlaylistView {
enum
{
    RemoveEntry              = KeyEvent::KeyDelete,
    ClearPlaylist            = 'c',
    ShufflePlaylist          = 'S',
    GoToCurrentlyPlayingSong = 'o',
    MoveSelectedSongs        = 'm',
    AddFileOrDirectory       = KeyEvent::ModifierCtrl | 'o',
    AddUrl                   = KeyEvent::ModifierCtrl | 'u',
    ShowSongInfo             = 'i'
};
}

namespace Screens {

namespace Help {
enum
{
    Activate = '1'
};
}

namespace ActivePlaylist {
enum
{
    Activate = '2'
};
}

namespace LocalFileSystemBrowser {
enum
{
    Activate = '3',

    AddFileOrDirectoryToActivePlaylist = ' ',
    GoUp                               = KeyEvent::KeyLeft,
    ChangeDirectory                    = 'g',
    ReloadDirectory                    = 'R'
};
}

namespace MedialibBrowser {
enum
{
    Activate = '5',

    AddItemToActivePlaylist = ' ',
    Refresh                 = 'R',
    ShowSongInfo            = 'i'
};
}

namespace PlaylistsBrowser {
enum
{
    Activate = '6',

    CreateNewPlaylist           = 'n',
    RemovePlaylist              = KeyEvent::KeyDelete,
    RenamePlaylist              = 'r',
    GoToCurrentlyActivePlaylist = 'o'
};
}
namespace Equalizer {
enum
{
    Activate = '7'
};
}

} // Screens
} // Hotkeys
} // ncxmms2
#endif // HOTKEYS_H

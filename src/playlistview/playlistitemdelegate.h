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

#ifndef PLAYLISTITEMDELEGATE_H
#define PLAYLISTITEMDELEGATE_H

#include "../lib/listmodelitemdelegate.h"
#include "../lib/palette.h"

namespace ncxmms2 {

class PlaylistModel;

class PlaylistItemDelegate : public ListModelItemDelegate
{
public:
    PlaylistItemDelegate(const PlaylistModel *model);

    virtual void paint(Painter *painter, const ListItemPaintOptions& options, int item);

    enum
    {
        RoleArtist = Palette::RoleUser,
        RoleTitle,
        RoleDuration
    };
};
}

#endif // PLAYLISTITEMDELEGATE_H

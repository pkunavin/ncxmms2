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

#ifndef LISTITEMPAINTOPTIONS_H
#define LISTITEMPAINTOPTIONS_H

#include "rectangle.h"

namespace ncxmms2 {

class Palette;

enum ListItemStateFlags
{
    ListItemStateRegular = 0x00000000,
    ListItemStateCurrent = 0x00000001,
    ListItemStateSelected = 0x00000002
};

class ListItemPaintOptions
{
public:
    ListItemPaintOptions(const Palette    *_palette,
                         const Rectangle&  _rect,
                         int               _state,
                         bool              _hasFocus) :
        palette(_palette),
        rect(_rect),
        state(_state),
        hasFocus(_hasFocus){}

    const Palette *palette;
    Rectangle rect;
    int state;
    bool hasFocus;
};
} // ncxmms2

#endif // LISTITEMPAINTOPTIONS_H

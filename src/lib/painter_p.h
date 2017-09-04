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

#ifndef PAINTER_P_H
#define PAINTER_P_H

#include <curses.h>
#include <stdint.h>
#include <map>

#include "colors.h"

#include "../../3rdparty/folly/sorted_vector_types.h"

namespace ncxmms2 {

class Window;

class PainterPrivate
{
public:
    PainterPrivate(Window *window_, WINDOW *cursesWin_) :
        color(ColorDefault),
        backgroundColor(ColorDefault),
        window(window_),
        cursesWin(cursesWin_) {}

    Color color;
    Color backgroundColor;

    Window *window;
    WINDOW *cursesWin;

    static int colorPairsNumber;
    static folly::sorted_vector_map<uint32_t, int> colorPairsMap;
    static int getColorPair(Color foreground, Color background);
    static int getCursesColor(Color color);
};
} // ncxmss2

#endif // PAINTER_P_H

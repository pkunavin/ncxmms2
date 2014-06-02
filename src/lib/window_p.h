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

#ifndef WINDOW_P_H
#define WINDOW_P_H

#include <curses.h>
#include <vector>
#include <limits>
#include <memory>

#include "point.h"
#include "size.h"
#include "painter_p.h"

namespace ncxmms2 {

class Window;
class Palette;

class WindowPrivate
{
public:
    WindowPrivate(const Point& position_, const Size& size_, Window *parent_) :
        cursesWin(nullptr),
        parent(parent_),
        focusedWindow(nullptr),
        position(position_),
        size(size_),
        minimumSize(1, 1),
        maximumSize(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
        isVisible(false),
        painterPrivate(nullptr, nullptr),
        isPainterPrivateAlreadyInUse(false) {}

    WINDOW *cursesWin;
    Window *parent;
    std::vector<Window*> childrenWins;
    Window *focusedWindow;

    Point position;
    Size size;

    Size minimumSize;
    Size maximumSize;

    bool isVisible;

    std::string title;

    PainterPrivate painterPrivate;
    bool isPainterPrivateAlreadyInUse;

    std::shared_ptr<const Palette> palette;

    static bool doingResize;
    void checkSize(const Size& size);
};
} // ncxmms2

#endif // WINDOW_P_H

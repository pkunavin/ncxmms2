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

#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#include <memory>
#include "Point.h"

namespace ncxmms2 {

class MouseEventPrivate;

class MouseEvent
{
public:
    enum class Type
    {
        ButtonPress,
        ButtonRelease,
        ButtonDoubleClick
    };

    enum
    {
        ButtonLeft  = 1,
        ButtonRight = 3,
        ButtonMid   = 2,

        WheelUp   = 4,
        WheelDown = 5
    };

    MouseEvent(Type type, const Point& position, int button);
    ~MouseEvent();

    Type type() const;
    const Point& position() const;
    int button() const;

private:
    std::unique_ptr<MouseEventPrivate> d;
};
} // ncxmms2

#endif // MOUSEEVENT_H

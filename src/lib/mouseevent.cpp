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

#include "mouseevent.h"

namespace ncxmms2 {

class MouseEventPrivate
{
public:
    MouseEventPrivate(MouseEvent::Type type_, const Point& position_, int button_) :
        type(type_),
        position(position_),
        button(button_) {}

    MouseEvent::Type type;
    Point position;
    int button;
};
} // ncxmms2

using namespace ncxmms2;

MouseEvent::MouseEvent(Type type, const Point& position, int button) :
    d(new MouseEventPrivate(type, position, button))
{

}

MouseEvent::~MouseEvent()
{

}

MouseEvent::Type MouseEvent::type() const
{
    return d->type;
}

const Point& MouseEvent::position() const
{
    return d->position;
}

int MouseEvent::button() const
{
    return d->button;
}

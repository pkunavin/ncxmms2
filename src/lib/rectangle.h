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

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"
#include "size.h"

namespace ncxmms2 {

class Rectangle
{
public:
    Rectangle(const Point& position, const Size& size) :
        m_position(position),
        m_size(size) {}

    Rectangle(int x, int y, int cols, int lines) :
        m_position(x, y),
        m_size(cols, lines) {}

    int x() const          {return m_position.x();}
    int y() const          {return m_position.y();}
    int cols() const       {return m_size.cols();}
    int lines() const      {return m_size.lines();}

    Point position() const {return m_position;}
    Size size() const      {return m_size;}

private:
    Point m_position;
    Size m_size;
};

} // ncxmms2

#endif // RECTANGLE_H

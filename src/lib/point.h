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

#ifndef POINT_H
#define POINT_H

namespace ncxmms2 {

class Point
{
public:
    Point(int x, int y) : m_x(x), m_y(y) {}

    void setX(int x) {m_x = x;}
    void setY(int y) {m_y = y;}

    int x() const {return m_x;}
    int y() const {return m_y;}

private:
    int m_x;
    int m_y;
};
} // ncxmms2

#endif // POINT_H

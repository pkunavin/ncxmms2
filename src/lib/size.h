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

#ifndef SIZE_H
#define SIZE_H

namespace ncxmms2 {

class Size
{
public:
    Size(int cols, int lines) : m_cols(cols), m_lines(lines) {}

    void setCols(int cols)   {m_cols = cols;}
    void setLines(int lines) {m_lines = lines;}

    int cols() const   {return m_cols;}
    int lines() const  {return m_lines;}

private:
    int m_cols;
    int m_lines;
};
} // ncxmms2

#endif // SIZE_H

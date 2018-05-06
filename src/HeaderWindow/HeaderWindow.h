/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef HEADERWINDOW_H
#define HEADERWINDOW_H

#include "../lib/Window.h"

namespace ncxmms2 {

class HeaderWindow : public Window
{
public:
    HeaderWindow(int xPos, int yPos, int cols, Window *parent = nullptr);

    void setHeaderTitle(const std::string& string);

    enum
    {
        TitleLine,
        SplitterLine,

        LinesNumber
    };

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::string m_headerTitle;
};
} // ncxmms2

#endif // HEADERWINDOW_H

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

#include "headerwindow.h"

#include "../lib/painter.h"
#include "../lib/rectangle.h"

using namespace ncxmms2;

HeaderWindow::HeaderWindow(int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, LinesNumber), parent)
{

}

void HeaderWindow::setHeaderTitle(const std::string& string)
{
    m_headerTitle = string;
    update();
}

void HeaderWindow::paint(const Rectangle& rect)
{
    Painter painter(this);
    painter.clearLine();
    painter.setBold(true);
    painter.squeezedPrint(m_headerTitle, cols());
    painter.drawHLine(0, SplitterLine, cols());
    painter.flush();
}




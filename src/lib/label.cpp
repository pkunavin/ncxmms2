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

#include "label.h"
#include "painter.h"
#include "rectangle.h"

namespace ncxmms2 {

class LabelPrivate
{
public:
    std::string text;
};
} // ncxmms2

using namespace ncxmms2;

Label::Label(int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent),
    d(new LabelPrivate())
{
    loadPalette("Label");
}

void Label::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearWindow();
    painter.squeezedPrint(d->text, cols());
    painter.flush();
}

void Label::setText(const std::string& text)
{
    d->text = text;
    update();
}

const std::string& Label::text() const
{
    return d->text;
}


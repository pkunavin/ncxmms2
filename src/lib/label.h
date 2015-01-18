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

#ifndef LABEL_H
#define LABEL_H

#include "window.h"

namespace ncxmms2 {

class LabelPrivate;

class Label : public Window
{
public:
    Label(int xPos, int yPos, int cols, Window *parent = nullptr);

    void setText(const std::string& text);
    const std::string& text() const;

protected:
    void paint(const Rectangle& rect);

private:
    std::unique_ptr<LabelPrivate> d;
};
} // ncxmms2

#endif // LABEL_H

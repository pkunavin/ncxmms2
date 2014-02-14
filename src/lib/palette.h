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

#ifndef PALETTE_H
#define PALETTE_H

#include <vector>
#include "colors.h"

namespace ncxmms2 {

class Palette
{
public:
    Palette();

    enum ColorGroup
    {
        GroupActive,
        GroupInactive,

        ColorGroupsNumber
    };

    enum ColorRole
    {
        RoleText,
        RoleBackground,
        RoleSelection,
        RoleHighlight,
        RoleHighlightedText,

        RoleUser
    };

    Color color(ColorGroup group, int role) const;
    Color color(ColorGroup group, int role, Color defaultColor) const;
    bool hasColor(ColorGroup group, int role) const;

    void setColor(ColorGroup group, int role, Color color);

private:
    std::vector<Color> m_colors;
};

} // ncxmms2

#endif // PALETTE_H

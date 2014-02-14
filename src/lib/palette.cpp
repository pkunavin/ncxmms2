/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <assert.h>
#include <stddef.h>

#include "palette.h"

using namespace ncxmms2;

Palette::Palette()
{
    m_colors.reserve(ColorGroupsNumber * RoleUser);

    // Default colors
    setColor(Palette::GroupActive, Palette::RoleText,            ColorDefault);
    setColor(Palette::GroupActive, Palette::RoleBackground,      ColorDefault);
    setColor(Palette::GroupActive, Palette::RoleSelection,       ColorMagenta);
    setColor(Palette::GroupActive, Palette::RoleHighlight,       ColorBlue);
    setColor(Palette::GroupActive, Palette::RoleHighlightedText, ColorWhite);

    setColor(Palette::GroupInactive, Palette::RoleText,            ColorDefault);
    setColor(Palette::GroupInactive, Palette::RoleBackground,      ColorDefault);
    setColor(Palette::GroupInactive, Palette::RoleSelection,       ColorMagenta);
    setColor(Palette::GroupInactive, Palette::RoleHighlight,       ColorWhite);
    setColor(Palette::GroupInactive, Palette::RoleHighlightedText, ColorBlack);
}

Color Palette::color(Palette::ColorGroup group, int role) const
{
    assert(size_t(ColorGroupsNumber * role + group) < m_colors.size());
    return m_colors[ColorGroupsNumber * role + group];
}


Color Palette::color(Palette::ColorGroup group, int role, Color defaultColor) const
{
    return size_t(ColorGroupsNumber * role + group) < m_colors.size()
           ? m_colors[ColorGroupsNumber * role + group]
           : defaultColor;
}

void Palette::setColor(Palette::ColorGroup group, int role, Color color)
{
    if (m_colors.size() < size_t(ColorGroupsNumber * (role + 1)))
        m_colors.resize(ColorGroupsNumber * (role + 1));

    m_colors[ColorGroupsNumber * role + group] = color;
}

bool Palette::hasColor(Palette::ColorGroup group, int role) const
{
    return size_t(ColorGroupsNumber * role + group) < m_colors.size();
}

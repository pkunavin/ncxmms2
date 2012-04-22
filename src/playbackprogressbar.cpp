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

#include "playbackprogressbar.h"
#include "lib/painter.h"

using namespace ncxmms2;

PlaybackProgressBar::PlaybackProgressBar(int lines, int cols, int yPos, int xPos, Window *parent) :
    Window(lines, cols, yPos, xPos, parent),
    m_value(0),
    m_maxValue(0)
{

}

void PlaybackProgressBar::setValue(int value)
{
    m_value = value;
    update();
}

void PlaybackProgressBar::setMaxValue(int maxValue)
{
    m_maxValue = maxValue;
    update();
}

void PlaybackProgressBar::paint(const Rectangle& rect)
{
    Painter painter(this);
    painter.drawHLine(0, 0, cols());
    if (m_maxValue > 0) {
        const int pos = ((double)m_value / (double)m_maxValue) * cols();
        painter.setBold(true);
        painter.drawHLine(0, 0, pos);
    }
    painter.flush();
}

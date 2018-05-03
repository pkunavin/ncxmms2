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

#include "PlaybackProgressBar.h"

#include "../lib/Painter.h"
#include "../lib/Rectangle.h"
#include "../lib/MouseEvent.h"

using namespace ncxmms2;

PlaybackProgressBar::PlaybackProgressBar(int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent),
    m_value(0),
    m_maxValue(0)
{
    loadPalette("PlaybackProgressBar");
    setMinumumLines(1);
    setMaximumLines(1);
}

void PlaybackProgressBar::setValue(int value)
{
    value = m_maxValue > 0 ? ((double)value / (double)m_maxValue) * cols() : 0;
    if (m_value != value) {
        m_value = value;
        update();
    }
}

void PlaybackProgressBar::setMaxValue(int maxValue)
{
    m_maxValue = maxValue;
    update();
}

void PlaybackProgressBar::mouseEvent(const MouseEvent& ev)
{
    if (ev.button() == MouseEvent::ButtonLeft && ev.type() == MouseEvent::Type::ButtonPress) {
        if (m_maxValue > 0)
            progressChangeRequested(((double)ev.position().x() / (double)cols()) * m_maxValue);
    }
}

void PlaybackProgressBar::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);
    
    Painter painter(this);
    painter.drawHLine(0, 0, cols());
    if (m_maxValue > 0) {
        painter.setBold(true);
        painter.drawHLine(0, 0, m_value);
    }
    painter.flush();
}

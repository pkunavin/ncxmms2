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

#include "equalizerpreampwindow.h"
#include "../utils.h"

#include "../lib/application.h"
#include "../lib/rectangle.h"
#include "../lib/painter.h"
#include "../lib/keyevent.h"
#include "../lib/mouseevent.h"
#include "../lib/palette.h"

using namespace ncxmms2;

EqualizerPreampWindow::EqualizerPreampWindow(int x, int y, int lines, Window *parent) :
    Window(Rectangle(x, y, 6, lines), parent),
    m_preamp(0)
{
    loadPalette("EqualizerPreampWindow");
}

void EqualizerPreampWindow::setPreamp(int preamp)
{
    m_preamp = preamp;
    update();
}

void EqualizerPreampWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:
            if (m_preamp < 20)
                preampChangeRequested(m_preamp + 1);
            break;
            
        case KeyEvent::KeyDown:
            if (m_preamp > -20)
                preampChangeRequested(m_preamp - 1);
            break;
            
        default:
            break;
    }
}

void EqualizerPreampWindow::mouseEvent(const MouseEvent& ev)
{
    if (ev.type() != MouseEvent::Type::ButtonPress)
        return;
    
    switch (ev.button()) {
        case MouseEvent::ButtonLeft:
        {
            const double dBRange = 40.0; // : -20dB .. +20dB
            const int preamp = (dBRange / (lines() - 2.0))
                               * ((lines() - 2.0) / 2.0 - ev.position().y());
            if (preamp >= -20 && preamp <= 20)
                preampChangeRequested(preamp);
            break;
        }
            
        case MouseEvent::WheelUp:
            if (m_preamp < 20)
                preampChangeRequested(m_preamp + 1);
            break;
            
        case MouseEvent::WheelDown:
            if (m_preamp > -20)
                preampChangeRequested(m_preamp - 1);
            break;
            
        default:
            break;
    }
}

void EqualizerPreampWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);
    
    Painter painter(this);
    painter.clearWindow();
    painter.printString(Utils::format(" %ddB", m_preamp));
    
    if (hasFocus())
        painter.setBold(true);
    const double dBRange = 40.0; // : -20dB .. +20dB
    const int y = (lines() - 2.0) / 2.0 - (lines() - 2.0) * (m_preamp / dBRange);
    painter.drawVLine(2, y + 1, lines() - 2 - y);
    
    painter.setBold(false);
    if (hasFocus()) {
        if (Application::useColors()) {
            painter.setColorPair(palette().color(Palette::GroupActive, Palette::RoleHighlightedText),
                                 palette().color(Palette::GroupActive, Palette::RoleHighlight));
        } else {
            painter.setReverse(true);
        }
    }
    painter.move(0, lines() - 1);
    painter.printString("Preamp");
    painter.flush();
}

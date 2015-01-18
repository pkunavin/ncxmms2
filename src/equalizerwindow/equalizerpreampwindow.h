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

#ifndef EQUALIZERPREAMPWINDOW_H
#define EQUALIZERPREAMPWINDOW_H

#include "../lib/window.h"

namespace ncxmms2 {

class EqualizerPreampWindow : public Window
{
public:
    EqualizerPreampWindow(int x, int y, int lines, Window *parent = nullptr);
    
    void setPreamp(int preamp);
    
    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);
    
    // Signals
    NCXMMS2_SIGNAL(preampChangeRequested, int)
    
protected:
    virtual void paint(const Rectangle& rect);
    
private:
    int m_preamp;
};
} // ncxmms2

#endif // EQUALIZERPREAMPWINDOW_H

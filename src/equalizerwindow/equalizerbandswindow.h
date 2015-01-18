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

#ifndef EQUALIZERBANDSWINDOW_H
#define EQUALIZERBANDSWINDOW_H

#include <vector>
#include "../lib/window.h"

namespace ncxmms2 {

class EqualizerBandsWindow : public Window
{
public:
    EqualizerBandsWindow(const Rectangle& rect, Window *parent = nullptr);

    int bandsNumber() const;
    void setBandsNumber(int number);
    void setBandGain(int band, int gain);
    
    bool legacyModeEnabled() const;
    void setLegacyModeEnabled();
    
    int selectedBand() const;
    
    enum {
        LegacyBandsNumber = 10
    };
    
    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);
    
    // Signals
    NCXMMS2_SIGNAL(bandGainChangeRequested, int, int)

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::vector<int> m_bandsGain;
    int m_selectedBand;
    bool m_legacyModeEnabled;
    bool m_terminalTooSmall;
    
    const char *bandFrequency(int band) const;
};
} // ncxmms2

#endif // EQUALIZERBANDSWINDOW_H

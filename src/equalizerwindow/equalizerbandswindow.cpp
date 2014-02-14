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

#include <map>

#include "equalizerbandswindow.h"
#include "../utils.h"

#include "../lib/application.h"
#include "../lib/palette.h"
#include "../lib/painter.h"
#include "../lib/keyevent.h"
#include "../lib/mouseevent.h"

using namespace ncxmms2;

EqualizerBandsWindow::EqualizerBandsWindow(const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    m_selectedBand(-1),
    m_legacyModeEnabled(false),
    m_terminalTooSmall(false)
{
    loadPalette("EqualizerBandsWindow");
}

int EqualizerBandsWindow::bandsNumber() const
{
    return m_bandsGain.size();
}

void EqualizerBandsWindow::setBandsNumber(int number)
{
    m_bandsGain.resize(number);
    m_selectedBand = 0;
    m_legacyModeEnabled = false;
}

void EqualizerBandsWindow::setBandGain(int band, int gain)
{
    assert((size_t)band < m_bandsGain.size());
    m_bandsGain[band] = gain;
    update();
}

bool EqualizerBandsWindow::legacyModeEnabled() const
{
    return m_legacyModeEnabled;
}

void EqualizerBandsWindow::setLegacyModeEnabled()
{
    setBandsNumber(LegacyBandsNumber);
    m_legacyModeEnabled = true;
}

int EqualizerBandsWindow::selectedBand() const
{
    return m_selectedBand;
}

void EqualizerBandsWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (m_terminalTooSmall) {
        m_selectedBand = 0;
        return;
    }
    
    switch (keyEvent.key()) {
        case KeyEvent::KeyLeft:
            if (m_selectedBand > 0) {
                --m_selectedBand;
                update();
            }
            break;
            
        case KeyEvent::KeyRight:
            if (m_selectedBand != -1 && (size_t)m_selectedBand < m_bandsGain.size() - 1) {
                ++m_selectedBand;
                update();
            }
            break;
            
        case KeyEvent::KeyUp:
            if (m_selectedBand != -1 && m_bandsGain[m_selectedBand] < 20)
                bandGainChangeRequested(m_selectedBand, m_bandsGain[m_selectedBand] + 1);
            break;
            
        case KeyEvent::KeyDown:
            if (m_selectedBand != -1 && m_bandsGain[m_selectedBand] > -20)
                bandGainChangeRequested(m_selectedBand, m_bandsGain[m_selectedBand] - 1);
            break;
        
        default:
            Window::keyPressedEvent(keyEvent);
    }
}

void EqualizerBandsWindow::mouseEvent(const MouseEvent& ev)
{
    if (m_terminalTooSmall)
        return;
    
    if (ev.type() != MouseEvent::Type::ButtonPress)
        return;
        
    const int maxBandWidth = 6;
    const int bandWidth = maxBandWidth * (m_bandsGain.size() + 1) > (size_t)cols()
                          ? cols() / (m_bandsGain.size() + 1)
                          : maxBandWidth;
    const int xShift = (cols() - (m_bandsGain.size() - 1) * bandWidth) / 2;
    const int band = (ev.position().x() - xShift + bandWidth / 2 - 1) / bandWidth;
    if (band < 0 || (size_t)band >= m_bandsGain.size())
        return;
    m_selectedBand = band;
    update();
    
    switch (ev.button()) {
        case MouseEvent::ButtonLeft:
        {
            const double dBRange = 40.0; // : -20dB .. +20dB
            const int gain = (dBRange / (lines() - 2.0))
                               * ((lines() - 2.0) / 2.0 - ev.position().y());
            if (gain >= -20 && gain <= 20)
                bandGainChangeRequested(m_selectedBand, gain);
            break;
        }
            
        case MouseEvent::WheelUp:
            if (m_bandsGain[m_selectedBand] < 20)
                bandGainChangeRequested(m_selectedBand, m_bandsGain[m_selectedBand] + 1);
            break;
            
        case MouseEvent::WheelDown:
            if (m_bandsGain[m_selectedBand] > -20)
                bandGainChangeRequested(m_selectedBand, m_bandsGain[m_selectedBand] - 1);
            break;
            
        default:
            break;
    }
}

void EqualizerBandsWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect) ;
    
    Painter painter(this);
    painter.clearWindow();

    const double dBRange = 40.0; // : -20dB .. +20dB

    const int maxBandWidth = 6;
    const int bandWidth = maxBandWidth * (m_bandsGain.size() + 1) > (size_t)cols()
                          ? cols() / (m_bandsGain.size() + 1)
                          : maxBandWidth;
    m_terminalTooSmall = false;
    if (bandWidth == 0) {
        const char *message = "Terminal too small!";
        painter.move((cols() - strlen(message)) / 2, lines() / 2);
        painter.printString(message);
        painter.flush();
        m_terminalTooSmall = true;
        painter.flush();
        return;
    }
    
    const int xShift = (cols() - (m_bandsGain.size() - 1) * bandWidth) / 2;
    
    for (int band = 0; band < (int)m_bandsGain.size(); ++band) {
        painter.setBold(band == m_selectedBand && hasFocus());
        const int y = (lines() - 2.0) / 2.0 - (lines() - 2.0) * (m_bandsGain[band] / dBRange);
        painter.drawVLine(xShift + bandWidth * band, y + 1, lines() - 2 - y);
    }
    painter.setBold(false);
    
    if (bandWidth >= maxBandWidth) {
        for (int band = 0; band < (int)m_bandsGain.size(); ++band) {
            painter.move(xShift + bandWidth * band - 2, 0);
            painter.printString(Utils::format(" %1%dB", m_bandsGain[band]));
            painter.move(xShift + bandWidth * band - 1, lines() - 1);
            painter.printString(bandFrequency(band));
        }
    }
    
    if (hasFocus() && m_selectedBand >= 0) {
        painter.move(xShift + bandWidth * m_selectedBand - 2, 0);
        painter.printString(Utils::format(" %1%dB", m_bandsGain[m_selectedBand]));
        if (Application::useColors()) {
            painter.setColorPair(palette().color(Palette::GroupActive, Palette::RoleHighlightedText),
                                 palette().color(Palette::GroupActive, Palette::RoleHighlight));
        } else {
            painter.setReverse(true);
        }
        painter.move(xShift + bandWidth * m_selectedBand - 1, lines() - 1);
        painter.printString(bandFrequency(m_selectedBand));
    }

    painter.flush();
}

const char *EqualizerBandsWindow::bandFrequency(int band) const
{
    static const char  *legacy[] = {"60", "170", "310", "600", "1K", "3K", "6K", "12K", "14K", "16K"};
    static const char *bands10[] = {"31", "62", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"};
    static const char *bands15[] = {"25", "40", "63", "100", "160", "250", "400", "630", "1K", "1.6K",
                                    "2.5K", "4K", "6.3K", "10K", "16K"};
    static const char *bands25[] = {"25", "31.5", "40", "50", "80", "100", "125", "160", "250", "315",
                                    "400", "500", "800", "1K", "1.25K", "1.6K",
                                    "2.5K", "3.15K", "4K", "5K", "8K", "10K", "12.5K", "16K", "20K"};
    static const char *bands31[] = {"20", "25", "31.5", "40", "50", "63", "80", "100", "125", "160",
                                    "200", "250", "315", "400", "500", "630", "800", "1K", "1.25K",
                                    "1.6K", "2K", "2.5K", "3.15K", "4K", "5K", "6.3K", "8K", "10K",
                                    "12.5K", "16K", "20K"};
    
    if (m_legacyModeEnabled)
        return legacy[band];
    
    switch (m_bandsGain.size()) {
        case 10: return bands10[band];
        case 15: return bands15[band];
        case 25: return bands25[band];
        case 31: return bands31[band];
        default:
            assert(false);
            return nullptr;
    }
}

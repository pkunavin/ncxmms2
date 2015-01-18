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

#ifndef EQUALIZERWINDOW_H
#define EQUALIZERWINDOW_H

#include "../lib/window.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class EqualizerBandsWindow;
class EqualizerPreampWindow;
class CheckBox;
class RadioButtonGroupBox;

class EqualizerWindow : public Window
{
public:
    EqualizerWindow(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);

protected:
    virtual void paint(const Rectangle& rect);
    virtual void resizeChildren(const Size& size);
    virtual void showEvent();

private:
    xmms2::Client *m_xmmsClient;
    EqualizerBandsWindow *m_bandsWindow;
    EqualizerPreampWindow *m_preampWindow;
    CheckBox *m_enabledCheckBox;
    CheckBox *m_extraFilteringCheckBox;
    RadioButtonGroupBox *m_bandsNumberGroupBox;
    bool m_equalizerPluginEnabled;

    enum {
        BandsNumberGroupBoxLegacy,
        BandsNumberGroupBox10,
        BandsNumberGroupBox15,
        BandsNumberGroupBox25,
        BandsNumberGroupBox31,
        
        BandsNumberGroupBoxItemsCount
    };
    
    void loadEqualizerConfig();
    void handleEqualizerConfigChanged(const std::string& key, const std::string& value);
    void setEqualizerEnabled(bool enable);
    void setEqualizerExtraFiltering(bool enable);
    void setEqualizerBandsNumber(int index);
    void setEqualizerPreamp(int preamp);
    void setEqualizerBandGain(int band, int gain);
};
} // ncxmms2

#endif // EQUALIZERWINDOW_H

/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef RADIOBUTTONGROUPBOX_H
#define RADIOBUTTONGROUPBOX_H

#include "Window.h"

namespace ncxmms2 {

class RadioButtonGroupBoxPrivate;

class RadioButtonGroupBox : public Window
{
public:
    RadioButtonGroupBox(const Rectangle& rect, Window *parent = nullptr);
    ~RadioButtonGroupBox();

    const std::string& groupName() const;
    void setGroupName(const std::string& groupName);

    void addRadioButton(const std::string& buttonText);

    int checkedRadioButton() const;
    void setCheckedRadioButton(int index);
    
    int activeRadioButton() const;
    void setActiveRadioButton(int index);
    
    const std::string& radioButtonText(int index) const;
    const std::string& checkedRadioButtonText() const;
    
    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);
    
    // Signals
    NCXMMS2_SIGNAL(activated, int) // // Emitted only by user input, not by setCheckedRadioButton
    NCXMMS2_SIGNAL(checkedRadioButtonChanged, int)

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::unique_ptr<RadioButtonGroupBoxPrivate> d;
};
} // ncxmms2

#endif // RADIOBUTTONGROUPBOX_H

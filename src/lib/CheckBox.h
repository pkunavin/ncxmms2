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

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "Window.h"

namespace ncxmms2 {

class CheckBoxPrivate;

class CheckBox : public Window
{
public:
    CheckBox(int x, int y, int cols, Window *parent = nullptr);
    ~CheckBox();

    const std::string& text() const;
    void setText(const std::string& text);

    bool isChecked() const;
    void setChecked(bool checked);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);

    // Signals
    NCXMMS2_SIGNAL(activated, bool) // Emitted only by user input, not by setChecked
    NCXMMS2_SIGNAL(checkStateChanged, bool)

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::unique_ptr<CheckBoxPrivate> d;
};
} // ncxmms2

#endif // CHECKBOX_H

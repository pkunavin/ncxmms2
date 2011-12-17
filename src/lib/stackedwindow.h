/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef STACKEDWINDOW_H
#define STACKEDWINDOW_H

#include "window.h"

namespace ncxmms2 {

class StackedWindowPrivate;

class StackedWindow : public Window
{
public:
    StackedWindow(int lines, int cols, int yPos, int xPos, Window *parent = 0);
    ~StackedWindow();

    void addWindow(Window *window);
    Window *window(int index) const;
    void setCurrentIndex(int index);

    int size() const;
    int currentIndex() const;

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resizeEvent(const Size& size);

private:
    std::unique_ptr<StackedWindowPrivate> d;
};
} // ncxmms2


#endif // STACKEDWINDOW_H

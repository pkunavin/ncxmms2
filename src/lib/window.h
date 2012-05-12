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

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include "object.h"

namespace ncxmms2 {

class KeyEvent;
class Point;
class Size;
class WindowPrivate;
class Rectangle;

class Window : public Object
{
public:
    Window(const Rectangle& rect, Window *parent = nullptr);
    virtual ~Window();

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

    int cols() const;
    int lines() const;
    Size size() const;

    int x() const;
    int y() const;
    Point position() const;

    void move(int x, int y);
    void move(const Point& position);

    virtual void resize(const Size& size);

    void hide();
    void show();
    bool isHidden() const;

    void setFocus();
    bool hasFocus() const;

    void setTitle(const std::string& title);
    const std::string& title() const;

    // Signals
    NCXMMS2_SIGNAL(titleChanged, const std::string&)

protected:
    virtual void paint(const Rectangle& rect);

    void update();
    void update(const Rectangle& rect);

private:
    Window(const Window& other);
    Window& operator=(const Window& other);
    std::unique_ptr<WindowPrivate> d;
    friend class Painter;
};
} // ncxmms2


#endif // WINDOW_H

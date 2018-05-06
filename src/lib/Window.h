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

#ifndef WINDOW_H
#define WINDOW_H

#include <map>
#include <memory>

#include "Object.h"

namespace ncxmms2 {

class KeyEvent;
class MouseEvent;
class Point;
class Size;
class WindowPrivate;
class Rectangle;
class Palette;

class Window : public Object
{
public:
    Window(const Rectangle& rect, Window *parent = nullptr);
    virtual ~Window();

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);

    int cols() const;
    int lines() const;
    Size size() const;

    void setMinumumCols(int cols);
    void setMinumumLines(int lines);
    void setMinimumSize(const Size& size);

    int minimumCols() const;
    int minimumLines() const;
    Size minimumSize() const;

    void setMaximumCols(int cols);
    void setMaximumLines(int lines);
    void setMaximumSize(const Size& size);

    int maximumCols() const;
    int maximumLines() const;
    Size maximumSize() const;

    int x() const;
    int y() const;
    Point position() const;

    Point globalPosition() const;

    void move(int x, int y);
    void move(const Point& position);

    virtual void resize(const Size& size);

    void hide();
    void show();
    bool isHidden() const;

    void setFocus();
    bool hasFocus() const;
    Window *focusedWindow() const;
    
    void setPalette(const std::shared_ptr<const Palette>& palette);
    const Palette& palette() const;

    void loadPalette(const std::string& className);
    void loadPalette(const std::string& className, const std::map<std::string, int>& userRolesMap);

    bool pointInWindow(const Point& point) const; // Accepts coordinates relative to parent window (NOT global!)
    Point toLocalCoordinates(const Point& point) const; // Accepts coordinates relative to parent window (NOT global!)

    // Signals
    NCXMMS2_SIGNAL(focusAcquired)
    NCXMMS2_SIGNAL(focusLost)

protected:
    virtual void resizeChildren(const Size& size);
    virtual void showEvent();
    virtual void paint(const Rectangle& rect);

    void update();
    void update(const Rectangle& rect);

private:
    Window(const Window& other);
    Window& operator=(const Window& other);
    std::unique_ptr<WindowPrivate> d;
    friend class Painter;
    friend class Application;
};
} // ncxmms2


#endif // WINDOW_H

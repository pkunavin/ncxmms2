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

#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "window.h"

namespace ncxmms2 {

class TextViewPrivate;

class TextView : public Window
{
public:
    TextView(const Rectangle& rect, Window *parent = nullptr);
    ~TextView();

    void setText(const std::string& text);
    void appendLine(const std::string& line = std::string());

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resize(const Size& size);

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::unique_ptr<TextViewPrivate> d;
    friend class TextViewPrivate;
};
} // ncxmms2

#endif // TEXTVIEW_H

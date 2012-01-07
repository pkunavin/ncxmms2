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

#ifndef KEYEVENT_H
#define KEYEVENT_H

#include <curses.h>

namespace ncxmms2 {

class KeyEvent
{
public:
    typedef wchar_t key_t;

    KeyEvent(key_t key, bool isFunctionKey) : m_key(key)
    {
        m_isFunctionKey = isFunctionKey
                          || key==KeyEnter
                          || key==KeyEscape;
    }

    key_t key() const          {return m_key;}
    bool isFunctionKey() const {return m_isFunctionKey;}

    enum
    {
        KeyDown   = KEY_DOWN,
        KeyUp     = KEY_UP,
        KeyLeft   = KEY_LEFT,
        KeyRight  = KEY_RIGHT,

        KeyHome = KEY_HOME,
        KeyEnd  = KEY_END,

        KeyEnter     = '\n',
        KeyEscape    = '\033',
        KeyDelete    = KEY_DC,
        KeyBackspace = KEY_BACKSPACE
    };

private:
    key_t m_key;
    bool m_isFunctionKey;
};
} // ncxmms2

#endif // KEYEVENT_H

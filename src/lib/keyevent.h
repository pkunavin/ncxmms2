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

#include <string>

typedef struct TermKeyKey_St TermKeyKey;

namespace ncxmms2 {

class KeyEvent
{
public:
    typedef char32_t key_t;

    KeyEvent(const TermKeyKey& termKey);
    KeyEvent(key_t key) : m_key(key) {}

    key_t key() const          {return m_key;}
    bool isFunctionKey() const {return (m_key & KeyCodeMask) > KeyLastUtf32Char;}
    std::string keyName() const;

    enum
    {
        KeyLastUtf32Char = 0x10FFFF,

        KeyEnter,
        KeyEscape,

        KeyBackspace,
        KeyDelete,
        KeyInsert,

        KeyUp,
        KeyDown,
        KeyLeft,
        KeyRight,
        KeyTab,

        KeyHome,
        KeyEnd,
        KeyPageUp,
        KeyPageDown,

        KeyF1,
        KeyF2,
        KeyF3,
        KeyF4,
        KeyF5,
        KeyF6,
        KeyF7,
        KeyF8,
        KeyF9,
        KeyF10,
        KeyF11,
        KeyF12
    };

    enum
    {
        ModifierAlt  = 0x40000000,
        ModifierCtrl = 0x80000000,

        KeyCodeMask = 0x00FFFFFF
    };

private:
    key_t m_key;
};
} // ncxmms2

#endif // KEYEVENT_H

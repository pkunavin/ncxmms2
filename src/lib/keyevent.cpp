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

#include <map>

#include "keyevent.h"
#include "../../3rdparty/libtermkey/termkey.h"

namespace ncxmms2 {

class KeyEventPrivate
{
public:
    KeyEventPrivate(){}

    KeyEvent::key_t key;
    static const std::map<char32_t, char32_t> keyMap;
};

const std::map<char32_t, char32_t> KeyEventPrivate::keyMap =
{
    {TERMKEY_SYM_ENTER,     KeyEvent::KeyEnter},
    {TERMKEY_SYM_ESCAPE,    KeyEvent::KeyEscape},

    {TERMKEY_SYM_BACKSPACE, KeyEvent::KeyBackspace},
    {TERMKEY_SYM_DEL,       KeyEvent::KeyBackspace},
    {TERMKEY_SYM_DELETE,    KeyEvent::KeyDelete},
    {TERMKEY_SYM_INSERT,    KeyEvent::KeyInsert},

    {TERMKEY_SYM_UP,        KeyEvent::KeyUp},
    {TERMKEY_SYM_DOWN,      KeyEvent::KeyDown},
    {TERMKEY_SYM_LEFT,      KeyEvent::KeyLeft},
    {TERMKEY_SYM_RIGHT,     KeyEvent::KeyRight},
    {TERMKEY_SYM_TAB,       KeyEvent::KeyTab},

    {TERMKEY_SYM_HOME,      KeyEvent::KeyHome},
    {TERMKEY_SYM_END,       KeyEvent::KeyEnd},
    {TERMKEY_SYM_PAGEUP,    KeyEvent::KeyPageUp},
    {TERMKEY_SYM_PAGEDOWN,  KeyEvent::KeyPageDown}
};

} // ncxmms2

using namespace ncxmms2;

KeyEvent::KeyEvent(const TermKeyKey& termKey) :
    d(new KeyEventPrivate())
{
    switch (termKey.type) {
        case TERMKEY_TYPE_UNICODE:
            d->key = termKey.code.codepoint;
            break;

        case TERMKEY_TYPE_KEYSYM:
        {
            auto it = KeyEventPrivate::keyMap.find(termKey.code.sym);
            if (it != KeyEventPrivate::keyMap.end())
                d->key = it->second;
            break;
        }

        case TERMKEY_TYPE_FUNCTION:
            if (termKey.code.number > 0 && termKey.code.number <= 12)
                d->key = KeyF1 + termKey.code.number - 1;
            break;

        default:
            break;
    }

    if (termKey.modifiers & TERMKEY_KEYMOD_ALT)
        d->key |= ModifierAlt;

    if (termKey.modifiers & TERMKEY_KEYMOD_CTRL)
        d->key |= ModifierCtrl;
}

KeyEvent::~KeyEvent()
{

}

KeyEvent::key_t KeyEvent::key() const
{
    return d->key;
}

bool KeyEvent::isFunctionKey() const
{
    return (d->key & 0x00FFFFFF) > KeyLastUtf32Char;
}

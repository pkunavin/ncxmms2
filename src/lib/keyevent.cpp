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

#include <map>
#include <glib.h>
#include <boost/lexical_cast.hpp>

#include "keyevent.h"
#include "../../3rdparty/libtermkey/termkey.h"

using namespace ncxmms2;

KeyEvent::KeyEvent(const TermKeyKey& termKey) : m_key(0)
{
    static const std::map<char32_t, char32_t> keyMap =
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

    switch (termKey.type) {
        case TERMKEY_TYPE_UNICODE:
            m_key = termKey.code.codepoint;
            break;

        case TERMKEY_TYPE_KEYSYM:
        {
            auto it = keyMap.find(termKey.code.sym);
            if (it != keyMap.end())
                m_key = it->second;
            break;
        }

        case TERMKEY_TYPE_FUNCTION:
            if (termKey.code.number > 0 && termKey.code.number <= 12)
                m_key = KeyF1 + termKey.code.number - 1;
            break;

        default:
            break;
    }

    if (termKey.modifiers & TERMKEY_KEYMOD_ALT)
        m_key |= ModifierAlt;

    if (termKey.modifiers & TERMKEY_KEYMOD_CTRL)
        m_key |= ModifierCtrl;
}

std::string KeyEvent::keyName() const
{
    static const std::map<key_t, std::string> keyNames =
    {
        {KeyEnter,     "Enter"},
        {KeyEscape,    "Escape"},
        {KeyBackspace, "Backspace"},
        {KeyDelete,    "Delete"},
        {KeyInsert,    "Insert"},
        {KeyUp,        "Up arrow"},
        {KeyDown,      "Down arrow"},
        {KeyLeft,      "Left arrow"},
        {KeyRight,     "Right arrow"},
        {KeyTab,       "Tab"},
        {KeyHome,      "Home"},
        {KeyEnd,       "End"},
        {KeyPageUp,    "PageUp"},
        {KeyPageDown,  "PageDown"}
    };

    std::string name;

    if (m_key & ModifierCtrl)
        name.append("Ctrl-");

    if (m_key & ModifierAlt)
        name.append("Alt-");

    if (isFunctionKey()) {
        key_t keyCode = m_key & KeyCodeMask;
        if (keyCode >= KeyF1 && keyCode <= KeyF12) {
            name.push_back('F');
            name.append(boost::lexical_cast<std::string>(keyCode - KeyF1 + 1));
        } else if (keyCode < KeyF1) {
            auto it = keyNames.find(keyCode);
            if (it != keyNames.end()) {
                name.append(it->second);
            } else {
                name = "Unknown";
            }
        } else {
            name = "Unknown";
        }
    } else {
        char buf[6];
        char32_t ch = m_key & KeyCodeMask;
        if (ch == ' ') {
            name.append("space");
        } else {
            if (m_key & (ModifierCtrl | ModifierAlt))
                ch = g_unichar_toupper(ch);
            const int n = g_unichar_to_utf8(ch, buf);
            name.append(buf, n);
        }
    }

    return std::move(name);
}

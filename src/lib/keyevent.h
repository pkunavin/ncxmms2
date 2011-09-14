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

#ifndef KEYEVENT_H
#define KEYEVENT_H

#include <curses.h>

namespace ncxmms2
{
	class KeyEvent
	{
	public:
		KeyEvent(int key) : m_key(key){}

		int key() const    {return m_key;}

		enum
		{
			KeyDown   = KEY_DOWN,
			KeyUp     = KEY_UP,
			KeyLeft   = KEY_LEFT,
			KeyRight  = KEY_RIGHT,

			KeyEnter  = '\n',
			KeyDelete = KEY_DC
		};

	private:
		const int m_key;
	};
}

#endif // KEYEVENT_H

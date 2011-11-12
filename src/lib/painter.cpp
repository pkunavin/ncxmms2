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

#include <curses.h>
#include <glib.h>
#include "painter.h"
#include "window_p.h"

namespace ncxmms2
{
	class PainterPrivate
	{
	public:
		PainterPrivate(Window *window_, WINDOW *cursesWin_) :
			window(window_), cursesWin(cursesWin_) {}

		Window *window;
		WINDOW *cursesWin;
	};
}

using namespace ncxmms2;

Painter::Painter(Window *window) : d(new PainterPrivate(window, window->d->cursesWin))
{
	wmove(d->cursesWin, 0, 0);
	wbkgdset(d->cursesWin, A_NORMAL);
	wattrset(d->cursesWin, A_NORMAL);
}

Painter::~Painter()
{

}

void Painter::move(int x, int y)
{
	wmove(d->cursesWin, y, x);
}

void Painter::clearLine()
{
	wclrtoeol(d->cursesWin);
}

void Painter::clearLine(int line)
{
	wmove(d->cursesWin, line, 0);
	wclrtoeol(d->cursesWin);
}

void Painter::fillLine(int line, ncxmms2::Color color)
{
	auto bg=getbkgd(d->cursesWin);
	wbkgdset(d->cursesWin, A_REVERSE | COLOR_PAIR(color));
	wmove(d->cursesWin, line, 0);
	wclrtoeol(d->cursesWin);
	wbkgdset(d->cursesWin, bg);
}

void Painter::setReverse(bool reverse)
{
	if (reverse) {
		wattron(d->cursesWin, A_REVERSE);
	} else {
		wattroff(d->cursesWin, A_REVERSE);
	}
}

void Painter::clearWindow()
{
	werase(d->cursesWin);
}

void Painter::setBold(bool bold)
{
	if (bold) {
		wattron(d->cursesWin, A_BOLD);
	} else {
		wattroff(d->cursesWin, A_BOLD);
	}
}

void Painter::setColor(ncxmms2::Color color)
{
	wattron(d->cursesWin, COLOR_PAIR(color));
}

void Painter::printString(const std::string& str)
{
	waddstr(d->cursesWin, str.c_str());
}

void Painter::printString(const std::string& str, std::string::size_type maxLength)
{
	if (str.size()<=maxLength) {
		waddstr(d->cursesWin, str.c_str());
	} else {
		const char *c_str=str.c_str();
		while (maxLength && *c_str) {
			c_str=g_utf8_next_char(c_str);
			--maxLength;
		}
		waddnstr(d->cursesWin, str.c_str(), c_str-str.c_str());
	}
}

void Painter::squeezedPrint(const std::string& str, std::string::size_type maxLength)
{
	if (str.size()<=maxLength) {
		waddstr(d->cursesWin, str.c_str());
	} else {
		const char *c_str=str.c_str();
		while (maxLength && *c_str) {
			c_str=g_utf8_next_char(c_str);
			--maxLength;
		}
		
		if (!maxLength) {
			for (int i=0; i<3; ++i) {
				c_str=g_utf8_prev_char(c_str);
			}
			waddnstr(d->cursesWin, str.c_str(), c_str-str.c_str());
			waddstr(d->cursesWin, "...");
		} else {
			waddstr(d->cursesWin, str.c_str());
		}
	}
}

void Painter::printString(const std::wstring& str)
{
	waddnwstr(d->cursesWin, str.c_str(), str.size());
}

void Painter::drawHLine(int startX, int startY, int length, int symbol)
{
	mvwhline(d->cursesWin, startY, startX, symbol, length);
}

void Painter::drawVLine(int startX, int startY, int length, int symbol)
{
	mvwvline(d->cursesWin, startY, startX, symbol, length);
}

void Painter::flush()
{
	wrefresh(d->cursesWin);
}

int Painter::xPosition() const
{
	return getcurx(d->cursesWin);
}

int Painter::yPosition() const
{
	return getcury(d->cursesWin);
}

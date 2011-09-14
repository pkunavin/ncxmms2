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

Painter::Painter(Window *window)
{
	d=new PainterPrivate(window, window->d->cursesWin);

	wmove(d->cursesWin, 0, 0);
	wbkgdset(d->cursesWin, A_NORMAL);
	wattrset(d->cursesWin, A_NORMAL);
}

Painter::~Painter()
{
	delete d;
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

void Painter::printString(const std::string& string)
{
	waddstr(d->cursesWin, string.c_str());
}

void Painter::printString(const std::string &string, std::string::size_type maxLength)
{
	if (string.size()<=maxLength) {
		waddstr(d->cursesWin, string.c_str());
	} else {
		char *buffer=new char[string.size()+1];
		buffer[string.size()]='\0';
		g_utf8_strncpy(buffer, string.c_str(), maxLength);
		waddstr(d->cursesWin, buffer);
		delete [] buffer;
	}
}

void Painter::squeezedPrint(const std::string &string, std::string::size_type maxLength)
{
	if (string.size()<=maxLength) {
		waddstr(d->cursesWin, string.c_str());
	} else {
		const std::string::size_type xBegin=getcurx(d->cursesWin);
		const std::string::size_type yBegin=getcury(d->cursesWin);

		printString(string, maxLength);

		const std::string::size_type xEnd=getcurx(d->cursesWin);
		const std::string::size_type yEnd=getcury(d->cursesWin);
		if ((yEnd-yBegin)*d->window->cols()+(xEnd-xBegin)==maxLength) {
			if (xEnd<3) {
				wmove(d->cursesWin, yEnd-1, d->window->cols()-(3-xEnd));
			} else {
				wmove(d->cursesWin, yEnd, xEnd-3);
			}
			waddstr(d->cursesWin, "...");
		}
	}
}

void Painter::drawHLine(int startX, int startY, int length, int symbol)
{
	mvwhline(d->cursesWin, startY, startX, symbol, length);
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

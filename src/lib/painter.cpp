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

#include <curses.h>
#include <glib.h>
#include <stdexcept>
#include <assert.h>

#include "painter.h"
#include "painter_p.h"
#include "window.h"
#include "window_p.h"

using namespace ncxmms2;

Painter::Painter(Window *window)
{
    assert(!window->isHidden());

    WindowPrivate *p = window->d.get();
    if (G_UNLIKELY(p->isPainterPrivateAlreadyInUse))
        throw std::logic_error("You are trying to create more than one copy of"
                               "Painter in a one scope, it is not possible!");

    p->isPainterPrivateAlreadyInUse = true;
    new (&p->painterPrivate) PainterPrivate(window, p->cursesWin);
    d = &p->painterPrivate;

    wmove(d->cursesWin, 0, 0);
    wbkgdset(d->cursesWin, A_NORMAL);
    wattrset(d->cursesWin, A_NORMAL);
}

Painter::~Painter()
{
    d->window->d->isPainterPrivateAlreadyInUse = false;
    d->~PainterPrivate();
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
    auto bg = getbkgd(d->cursesWin);
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

void Painter::printChar(char ch)
{
    waddnstr(d->cursesWin, &ch, 1);
}

void Painter::printChar(wchar_t ch)
{
    waddnwstr(d->cursesWin, &ch, 1);
}

void Painter::printChar(char32_t ch)
{
    // We assume here that char32_t and wchar_t have the smae size
    static_assert(sizeof(char32_t) == sizeof(wchar_t),
                  "sizeof(char32_t) != sizeof(wchar_t)");
    waddnwstr(d->cursesWin, reinterpret_cast<const wchar_t*>(&ch), 1);
}

void Painter::printString(const std::string& str)
{
    waddstr(d->cursesWin, str.c_str());
}

void Painter::printString(const std::string& str, std::string::size_type maxLength)
{
    if (str.size() <= maxLength) {
        waddstr(d->cursesWin, str.c_str());
    } else {
        const char *c_str = str.c_str();
        while (maxLength && *c_str) {
            c_str = g_utf8_next_char(c_str);
            --maxLength;
        }
        waddnstr(d->cursesWin, str.c_str(), c_str - str.c_str());
    }
}

void Painter::printString(const char *str)
{
    waddstr(d->cursesWin, str);
}

void Painter::squeezedPrint(const std::string& str, std::string::size_type maxLength)
{
    if (str.size() <= maxLength) {
        waddstr(d->cursesWin, str.c_str());
    } else {
        const char *c_str = str.c_str();
        while (maxLength && *c_str) {
            c_str = g_utf8_next_char(c_str);
            --maxLength;
        }

        if (!maxLength) {
            for (int i = 0; i < 3; ++i) {
                c_str = g_utf8_prev_char(c_str);
            }
            waddnstr(d->cursesWin, str.c_str(), c_str - str.c_str());
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

void Painter::printString(const wchar_t *str, size_t maxLength)
{
    waddnwstr(d->cursesWin, str, maxLength);
}

void Painter::printString(const std::u32string& str)
{
    // We assume here that char32_t and wchar_t have the smae size
    static_assert(sizeof(char32_t) == sizeof(wchar_t),
                  "sizeof(char32_t) != sizeof(wchar_t)");
    waddnwstr(d->cursesWin, reinterpret_cast<const wchar_t*>(str.c_str()), str.size());
}

void Painter::printString(const char32_t *str, size_t maxLength)
{
    // We assume here that char32_t and wchar_t have the smae size
    static_assert(sizeof(char32_t) == sizeof(wchar_t),
                  "sizeof(char32_t) != sizeof(wchar_t)");
    waddnwstr(d->cursesWin, reinterpret_cast<const wchar_t*>(str), maxLength);
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

int Painter::x() const
{
    return getcurx(d->cursesWin);
}

int Painter::y() const
{
    return getcury(d->cursesWin);
}

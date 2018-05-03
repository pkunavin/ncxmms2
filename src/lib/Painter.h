/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef PAINTER_H
#define PAINTER_H

#include <string>
#include "Colors.h"

namespace ncxmms2 {

class Window;
class PainterPrivate;

// Painter class acts like Qt's QPainter.
class Painter
{
public:
    explicit Painter(Window *window);
    ~Painter();

    void move(int x, int y);
    void clearLine();
    void clearLine(int line);
    void fillLine(int line, Color color);
    void setReverse(bool reverse);
    void clearWindow();
    
    void setNormal();
    void setBold(bool bold);
    void setUnderline(bool underline);
    void setBlink(bool blink);
    
    void setColor(Color color);
    void setBackgroundColor(Color color);
    void setColorPair(Color foreground, Color background);

    void printChar(char ch);
    void printChar(wchar_t ch);
    void printChar(char32_t ch);

    void printString(const std::string& str);
    void printString(const std::string& str, std::string::size_type maxLength);
    void printString(const char *str);
    void printString(const char *begin, const char *end);
    void printString(const std::wstring& str);
    void printString(const wchar_t *str, size_t maxLength);
    void printString(const std::u32string& str);
    void printString(const char32_t *str, size_t maxLength);
    void squeezedPrint(const std::string& str, std::string::size_type maxLength);

    void drawHLine(int startX, int startY, int length, int symbol = 0);
    void drawVLine(int startX, int startY, int length, int symbol = 0);
    void drawRect(int x, int y, int cols, int lines);
    void drawBox(int vSymbol = 0, int hSymbol = 0);

    void flush();

    int x() const;
    int y() const;

private:
    Painter(const Painter&);
    Painter& operator=(const Painter&);
    PainterPrivate *d;
};
} // ncxmms2

#endif // PAINTER_H

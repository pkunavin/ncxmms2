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

#ifndef PAINTER_H
#define PAINTER_H

#include <string>
#include <memory>
#include "colors.h"

namespace ncxmms2
{
	class Window;
	class PainterPrivate;

	// Painter class acts like Qt's QPainter.
	class Painter
	{
	public:
		Painter(Window *window);
		~Painter();

		void move(int x, int y);
		void clearLine();
		void clearLine(int line);
		void fillLine(int line, Color color);
		void setReverse(bool reverse);
		void clearWindow();
		void setBold(bool bold);
		void setColor(Color color);

		void printString(const std::string& string);
		void printString(const std::string& string, std::string::size_type maxLength);
		void squeezedPrint(const std::string& string, std::string::size_type maxLength);

		void drawHLine(int startX, int startY, int length, int symbol=0);
		void drawVLine(int startX, int startY, int length, int symbol=0);

		void flush();

		int xPosition() const;
		int yPosition() const;

	private:
		Painter(const Painter&);
		Painter& operator=(const Painter&);
		std::unique_ptr<PainterPrivate> d;
	};
}

#endif // PAINTER_H

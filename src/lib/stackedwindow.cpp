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

#include <vector>
#include "stackedwindow.h"

namespace ncxmms2 
{
	class StackedWindowPrivate
	{
	public:
		StackedWindowPrivate() : currentIndex(-1) {}
		
		std::vector<Window*> windows;
		int currentIndex;
	};
}

using namespace ncxmms2;

StackedWindow::StackedWindow(int lines, int cols, int yPos, int xPos, Window* parent) :
	Window(lines, cols, yPos, xPos, parent),
	d(new StackedWindowPrivate())
{

}

void StackedWindow::addWindow(Window *window)
{
	if (d->currentIndex!=-1)
		d->windows[d->currentIndex]->hide();
	
	d->currentIndex=d->windows.size();
	d->windows.push_back(window);
}

Window *StackedWindow::window(int index) const
{
	return d->windows[index];
}

void StackedWindow::setCurrentIndex(int index)
{
	if (d->currentIndex!=-1)
		d->windows[d->currentIndex]->hide();
	d->currentIndex=index;
	d->windows[index]->show();
}

int StackedWindow::size() const
{
	return d->windows.size();
}

int StackedWindow::currentIndex() const
{
	return d->currentIndex;
}

void StackedWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
	d->windows[d->currentIndex]->keyPressedEvent(keyEvent);
}

void StackedWindow::resizeEvent(const Size& size)
{
	Window::resizeEvent(size);
	for (Window *win : d->windows)
		win->resizeEvent(size);
	
	d->windows[d->currentIndex]->show();
}

StackedWindow::~StackedWindow()
{

}


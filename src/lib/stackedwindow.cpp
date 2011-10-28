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

#include "stackedwindow.h"

using namespace ncxmms2;

StackedWindow::StackedWindow(int lines, int cols, int yPos, int xPos, Window* parent) :
	Window(lines, cols, yPos, xPos, parent),
	m_currentIndex(-1)
{

}

void StackedWindow::addWindow(Window *window)
{
	if (m_currentIndex!=-1)
		m_windows[m_currentIndex]->hide();
	
	m_currentIndex=m_windows.size();
	m_windows.push_back(window);
}

Window *StackedWindow::window(int index) const
{
	return m_windows[index];
}

void StackedWindow::setCurrentIndex(int index)
{
	if (m_currentIndex!=-1)
		m_windows[m_currentIndex]->hide();
	m_currentIndex=index;
	m_windows[index]->show();
}

int StackedWindow::size() const
{
	return m_windows.size();
}

int StackedWindow::currentIndex() const
{
	return m_currentIndex;
}

void StackedWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
	m_windows[m_currentIndex]->keyPressedEvent(keyEvent);
}

void StackedWindow::resizeEvent(const Size& size)
{
	Window::resizeEvent(size);
	for (Window *win : m_windows)
		win->resizeEvent(size);
	
	m_windows[m_currentIndex]->show();
}

StackedWindow::~StackedWindow()
{

}


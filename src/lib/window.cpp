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

#include "curses.h"
#include "window_p.h"

using namespace ncxmms2;

Window::Window(int lines, int cols, int yPos, int xPos, Window* parent)
{
	d=new WindowPrivate(lines, cols, yPos, xPos, parent);

	if (parent)	{
		d->cursesWin=derwin(parent->d->cursesWin, lines, cols, yPos, xPos);
		parent->d->children.push_back(this);
	} else {
		d->cursesWin=newwin(lines, cols, yPos, xPos);
	}
}

int Window::lines() const
{
	return d->lines;
}

int Window::cols() const
{
	return d->cols;
}

void Window::move(int yPos, int xPos)
{
	d->yPos=yPos;
	d->xPos=xPos;
	delwin(d->cursesWin);
	d->cursesWin=d->parent
	             ? derwin(d->parent->d->cursesWin, d->lines, d->cols, yPos, xPos)
	             : newwin(d->lines, d->cols, yPos, xPos);
	
	update();
}

void Window::hide()
{
	d->isVisible=false;
	for (auto child : d->children)
		child->hide();
}

void Window::show()
{
	d->isVisible=true;
	showEvent();
	for (auto child : d->children)
		child->show();
}

bool Window::isHidden() const
{
	return !d->isVisible;
}

void Window::keyPressedEvent(const KeyEvent& keyEvent)
{

}

void Window::resizeEvent(const Size& size)
{
	d->lines=size.lines();
	d->cols=size.cols();

	delwin(d->cursesWin);
	d->cursesWin=d->parent
	             ? derwin(d->parent->d->cursesWin, d->lines, d->cols, d->yPos, d->xPos)
	             : newwin(d->lines, d->cols, d->yPos, d->xPos);
	
	update();
}

void Window::showEvent()
{

}

void Window::update()
{
	if (d->isVisible)
		showEvent();
}

const std::string& Window::title() const
{
	return d->title;
}

void Window::setTitleChangedCallback(const TitleChangedCallback& callback)
{
	d->titleChangedCallback=callback;
}

void Window::setTitle(const std::string& title)
{
	d->title=title;
	if (!d->titleChangedCallback.empty())
		d->titleChangedCallback(title);
}

Window::~Window()
{
	delwin(d->cursesWin);
	for (auto child : d->children)
		delete child;

	delete d;
}


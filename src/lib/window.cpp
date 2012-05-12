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

#include "window.h"
#include "window_p.h"
#include "size.h"
#include "rectangle.h"

using namespace ncxmms2;

Window::Window(const Rectangle& rect, Window *parent) :
    Object(parent),
    d(new WindowPrivate(rect.position(), rect.size(), parent))
{
    if (parent) {
        d->cursesWin = derwin(parent->d->cursesWin,
                              rect.lines(), rect.cols(), rect.y(), rect.x());
        parent->d->childrenWins.push_back(this);
    } else {
        d->cursesWin = newwin(rect.lines(), rect.cols(), rect.y(), rect.x());
    }
}

int Window::cols() const
{
    return d->size.cols();
}

int Window::lines() const
{
    return d->size.lines();
}

Size Window::size() const
{
    return d->size;
}

int Window::x() const
{
    return d->position.x();
}

int Window::y() const
{
    return d->position.y();
}

Point Window::position() const
{
    return d->position;
}

void Window::move(int x, int y)
{
    d->position.setX(x);
    d->position.setY(y);
    delwin(d->cursesWin);
    d->cursesWin = d->parent
                   ? derwin(d->parent->d->cursesWin, lines(), cols(), y, x)
                   : newwin(lines(), cols(), y, x);

    update();

    for (auto child : d->childrenWins)
        child->move(child->x(), child->y());
}

void Window::move(const Point& position)
{
    move(position.x(), position.y());
}

void Window::hide()
{
    d->isVisible = false;
    for (auto child : d->childrenWins)
        child->hide();
}

void Window::show()
{
    d->isVisible = true;
    paint(Rectangle(0, 0, cols(), lines()));
    for (auto child : d->childrenWins)
        child->show();
}

bool Window::isHidden() const
{
    return !d->isVisible;
}

void Window::setFocus()
{
    if (!d->parent)
        return;

    Window *old = d->parent->d->focusedWindow;
    d->parent->d->focusedWindow = this;
    if (old)
        old->update();
    update();
}

bool Window::hasFocus() const
{
    if (!d->parent)
        return true;

    return d->parent->d->focusedWindow == this;
}

void Window::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (d->focusedWindow)
        d->focusedWindow->keyPressedEvent(keyEvent);
}

void Window::resize(const Size& size)
{
    d->size = size;

    delwin(d->cursesWin);
    d->cursesWin = d->parent
                   ? derwin(d->parent->d->cursesWin, lines(), cols(), y(), x())
                   : newwin(lines(), cols(), y(), x());

    update();
}

void Window::paint(const Rectangle& rect)
{

}

void Window::update()
{
    update(Rectangle(0, 0, cols(), lines()));
}

void Window::update(const Rectangle& rect)
{
    if (d->isVisible)
        paint(rect);
}

const std::string& Window::title() const
{
    return d->title;
}

void Window::setTitle(const std::string& title)
{
    d->title = title;
    titleChanged(title);
}

Window::~Window()
{
    delwin(d->cursesWin);
}


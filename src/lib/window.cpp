/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <assert.h>
#include <algorithm>

#include "window.h"
#include "window_p.h"
#include "size.h"
#include "rectangle.h"
#include "exceptions.h"
#include "application.h"
#include "mouseevent.h"

using namespace ncxmms2;

bool WindowPrivate::doingResize = false;

Window::Window(const Rectangle& rect, Window *parent) :
    Object(parent),
    d(new WindowPrivate(rect.position(), rect.size(), parent))
{
    d->checkSize(rect.size());

    if (parent) {
        d->cursesWin = derwin(parent->d->cursesWin,
                              rect.lines(), rect.cols(), rect.y(), rect.x());
        parent->d->childrenWins.push_back(this);
    } else {
        d->cursesWin = newwin(rect.lines(), rect.cols(), rect.y(), rect.x());
    }

    loadPalette("Window");
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

void Window::setMinumumCols(int cols)
{
    setMinimumSize(Size(cols, minimumLines()));
}

void Window::setMinumumLines(int lines)
{
    setMinimumSize(Size(minimumCols(), lines));
}

void Window::setMinimumSize(const Size& size)
{
    d->checkSize(size);
    d->minimumSize = size;
}

int Window::minimumCols() const
{
    return d->minimumSize.cols();
}

int Window::minimumLines() const
{
    return d->minimumSize.lines();
}

Size Window::minimumSize() const
{
    return d->minimumSize;
}

void Window::setMaximumCols(int cols)
{
    setMaximumSize(Size(cols, maximumLines()));
}

void Window::setMaximumLines(int lines)
{
    setMaximumSize(Size(maximumCols(), lines));
}

void Window::setMaximumSize(const Size& size)
{
    d->checkSize(size);
    d->maximumSize = size;
}

int Window::maximumCols() const
{
    return d->maximumSize.cols();
}

int Window::maximumLines() const
{
    return d->maximumSize.lines();
}

Size Window::maximumSize() const
{
    return d->maximumSize;
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

Point Window::globalPosition() const
{
    Point result(position());
    Window *win = static_cast<Window *>(parent());
    for (; win; win = static_cast<Window *>(win->parent())) {
        result.setX(result.x() + win->x());
        result.setY(result.y() + win->y());
    }
    return result;
}

void Window::move(int x, int y)
{
    d->position.setX(x);
    d->position.setY(y);
    delwin(d->cursesWin);
    d->cursesWin = d->parent
                   ? derwin(d->parent->d->cursesWin, lines(), cols(), y, x)
                   : newwin(lines(), cols(), y, x);

    if (!WindowPrivate::doingResize)
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
    showEvent();
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
    if (old == this)
        return;

    d->parent->d->focusedWindow = this;
    if (old) {
        old->update();
        old->focusLost();
    }
    update();
    focusAcquired();
}

bool Window::hasFocus() const
{
    if (!d->parent)
        return true;

    return d->parent->d->focusedWindow == this;
}

void Window::setPalette(const std::shared_ptr<Palette>& palette)
{
    d->palette = palette;
}

const Palette& Window::palette() const
{
    return *(d->palette);
}

void Window::loadPalette(const std::string&                className,
                         const std::map<std::string, int>& userRolesMap)
{
    std::shared_ptr<Palette> newPalette = Application::getPalette(className,
                                                                  d->palette,
                                                                  userRolesMap);
    if (newPalette)
        d->palette = newPalette;

    assert(d->palette);
}

bool Window::pointInWindow(const Point& point) const
{
    return point.x() >= x() && point.x() < x() + cols()
        && point.y() >= y() && point.y() < y() + lines();
}

Point Window::toLocalCoordinates(const Point& point) const
{
    return Point(point.x() - x(), point.y() - y());
}

void Window::resizeChildren(const Size& size)
{
    NCXMMS2_UNUSED(size);
}

void Window::showEvent()
{
    for (auto child : d->childrenWins)
        child->show();
}

void Window::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (d->focusedWindow)
        d->focusedWindow->keyPressedEvent(keyEvent);
}

void Window::mouseEvent(const MouseEvent& ev)
{
    auto it = std::find_if(d->childrenWins.begin(), d->childrenWins.end(), [&ev](Window *win){
        return win->pointInWindow(ev.position());
    });
    if (it != d->childrenWins.end()) {
        if (!(*it)->hasFocus())
            (*it)->setFocus();
        (*it)->mouseEvent(MouseEvent(ev.type(),
                                     (*it)->toLocalCoordinates(ev.position()),
                                     ev.button()));
    }
}

void Window::resize(const Size& size)
{
    d->checkSize(size);
    d->size = size;

     bool needToShowResult = false;
     if (!WindowPrivate::doingResize && !isHidden()) {
         needToShowResult = true;
         WindowPrivate::doingResize = true;
     }

    delwin(d->cursesWin);
    d->cursesWin = d->parent
                   ? derwin(d->parent->d->cursesWin, size.lines(), size.cols(), y(), x())
                   : newwin(size.lines(), size.cols(), y(), x());

    resizeChildren(size);

    if (needToShowResult)
        show();

    WindowPrivate::doingResize = false;
}

void Window::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);
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

Window::~Window()
{
    if (d->parent) {
        auto& childrenOfParent = d->parent->d->childrenWins;
        auto it = std::find(childrenOfParent.begin(), childrenOfParent.end(), this);
        assert(it != childrenOfParent.end());
        childrenOfParent.erase(it);
    }

    for (Window *win : d->childrenWins) {
        win->d->parent = nullptr;
    }

    if (d->cursesWin)
        delwin(d->cursesWin);
}

void WindowPrivate::checkSize(const Size& size)
{
    /* We can't forbid resizing window, as we can't stop resizing terminal,
     * so throw an exception here
    */

    if (size.cols() < minimumSize.cols()
        || size.lines() < minimumSize.lines()) {
        throw DesiredWindowSizeTooSmall();
    }

    if (size.cols() > maximumSize.cols()
        || size.lines() > maximumSize.lines()) {
        throw DesiredWindowSizeTooBig();
    }
}

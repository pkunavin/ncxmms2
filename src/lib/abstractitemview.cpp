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

#include <boost/bind.hpp>
#include "abstractitemview.h"
#include "timer.h"
#include "painter.h"

namespace ncxmms2 {

class AbstractItemViewPrivate
{
public:
    AbstractItemViewPrivate(AbstractItemView *_q) :
        q(_q),
        currentItem(-1),
        viewportFirstItem(-1),
        viewportLastItem(-1),
        currentItemHidden(false),
        hideCurrentItemSelectionInterval(0) {}

    AbstractItemView *q;

    int currentItem;
    int viewportFirstItem;
    int viewportLastItem;


    Timer hideSelectionTimer;
    bool currentItemHidden;
    unsigned int hideCurrentItemSelectionInterval;

    void changeCurrentItem(int item);

    void scrollUp();
    void scrollDown();
};
} // ncxmms2

using namespace ncxmms2;

AbstractItemView::AbstractItemView(int lines, int cols, int yPos, int xPos, Window *parent) :
    Window(lines, cols, yPos, xPos, parent),
    d(new AbstractItemViewPrivate(this))
{
    d->hideSelectionTimer.timeout_Connect(boost::bind(&AbstractItemView::hideCurrentItem, this));
}

void AbstractItemView::reset()
{
    const int itemsNum = itemsCount();

    if (itemsNum > 0) {
        d->changeCurrentItem(0);
        d->viewportFirstItem = 0;
        d->viewportLastItem = itemsNum > lines() ? lines() : itemsNum;
        if (d->hideCurrentItemSelectionInterval)
            d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
    } else {
        d->changeCurrentItem(-1);
        d->viewportFirstItem = -1;
        d->viewportLastItem = -1;
    }

    update();
}

void AbstractItemView::redrawItem(int item)
{
    if (isHidden())
        return;

    if (item >= d->viewportFirstItem && item < d->viewportLastItem)
        drawItem(item);
    Painter(this).flush();
}

void AbstractItemView::redrawItems(int first, int last)
{
    if (isHidden())
        return;

    const int firstItem = std::max(first, d->viewportFirstItem);
    const int lastItem = std::min(last, d->viewportLastItem);

    for (int item = firstItem; item < lastItem; ++item) {
        drawItem(item);
    }
    Painter(this).flush();
}

void AbstractItemView::redrawAll()
{
    if (isHidden())
        return;

    for (int i = d->viewportFirstItem; i < d->viewportLastItem; ++i) {
        drawItem(i);
    }

    Painter painter(this);
    if (d->viewportFirstItem == -1) {
        painter.clearWindow();
    } else {
        for (int i = d->viewportLastItem; i < lines(); ++i) {
            painter.clearLine(i);
        }
    }

    painter.flush();
}

void AbstractItemView::itemAdded()
{
    if (d->currentItem == -1)
        reset();

    const int itemsNum = itemsCount();
    if (lines() >= itemsNum) {
        d->viewportLastItem = itemsNum;
        redrawItem(itemsNum - 1);
    }
}

void AbstractItemView::itemInserted(int item)
{
    if (d->currentItem == -1)
        reset();

    const int itemsNum = itemsCount();
    if (lines() >= itemsNum)
        d->viewportLastItem = itemsNum;

    redrawItems(item, itemsNum);
}

void AbstractItemView::itemRemoved(int item)
{
    const int itemsNum = itemsCount();

    if (d->currentItem > item || d->currentItem == itemsNum) {
        d->changeCurrentItem(d->currentItem - 1);
    } else if (d->currentItem == item) {
        /* Current item number is not changed, but items are moving,
    so information corresponding to the current item is changed */
        d->changeCurrentItem(d->currentItem);
    }

    if (lines() > itemsNum) {
        d->viewportLastItem = itemsNum;
        redrawAll();
    } else {
        if (item < d->viewportFirstItem) {
            --d->viewportFirstItem;
            --d->viewportLastItem;
        } else if (item < d->viewportLastItem) {
            if (itemsNum >= d->viewportLastItem) {
                redrawItems(item, d->viewportLastItem);
            } else {
                --d->viewportFirstItem;
                --d->viewportLastItem;
                redrawAll();
            }
        }
    }
}

void AbstractItemView::itemMoved(int from, int to)
{
    redrawItems(std::min(from, to), std::max(from, to) + 1);
}

void AbstractItemView::itemEntered(int item)
{

}

int AbstractItemView::currentItem() const
{
    return d->currentItem;
}

void AbstractItemView::setCurrentItem(int item)
{
    if (item < 0 || item >= itemsCount())
        return;

    if (!(item >= d->viewportFirstItem && item < d->viewportLastItem))
        setViewportFirstItem(item);

    d->changeCurrentItem(item);
    redrawAll();
    showCurrentItem();
}

bool AbstractItemView::isCurrentItemHidden() const
{
    return d->currentItemHidden;
}

void AbstractItemView::setHideCurrentItemInterval(unsigned int sec)
{
    d->hideCurrentItemSelectionInterval = sec;
    if (d->hideCurrentItemSelectionInterval) {
        d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
    } else {
        d->hideSelectionTimer.stop();
        d->currentItemHidden = false;
        redrawItem(d->currentItem);
    }
}

void AbstractItemView::showCurrentItem()
{
    d->currentItemHidden = false;
    redrawItem(d->currentItem);
    if (d->hideCurrentItemSelectionInterval)
        d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
}

void AbstractItemView::scrollToItem(int item)
{
    if (item < 0 || item >= itemsCount())
        return;

    if (item < d->viewportFirstItem) {
        d->viewportFirstItem = item;
        d->viewportLastItem = d->viewportFirstItem + lines();
        if (!(d->currentItem >= d->viewportFirstItem && d->currentItem < d->viewportLastItem))
            d->changeCurrentItem(d->viewportLastItem - 1);
    } else if (item >= d->viewportLastItem) {
        d->viewportLastItem = item + 1;
        d->viewportFirstItem = d->viewportLastItem - lines();
        if (!(d->currentItem >= d->viewportFirstItem && d->currentItem < d->viewportLastItem))
             d->changeCurrentItem(d->viewportFirstItem);
    }
    redrawAll();
}

int AbstractItemView::itemLine(int item) const
{
    return item - d->viewportFirstItem;
}

int AbstractItemView::viewportFirstItem() const
{
    return d->viewportFirstItem;
}

void AbstractItemView::setViewportFirstItem(int item)
{
    if (itemsCount() <= lines())
        return;

    if (item + lines() < itemsCount()) {
        d->viewportFirstItem = item;
        d->viewportLastItem = d->viewportFirstItem + lines();
    } else {
        d->viewportLastItem = itemsCount();
        d->viewportFirstItem = d->viewportLastItem - lines();
    }
    d->changeCurrentItem(d->viewportFirstItem);
    redrawAll();
}

void AbstractItemView::showEvent()
{
    redrawAll();
}

void AbstractItemView::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:
            if (d->currentItemHidden) {
                d->currentItemHidden = false;
                redrawItem(d->currentItem);
            } else {
                d->scrollUp();
            }
            if (d->hideCurrentItemSelectionInterval)
                d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
            break;

        case KeyEvent::KeyDown:
            if (d->currentItemHidden) {
                d->currentItemHidden = false;
                redrawItem(d->currentItem);
            } else {
                d->scrollDown();
            }
            if (d->hideCurrentItemSelectionInterval)
                d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
            break;

        case KeyEvent::KeyEnter:
            if (d->currentItem != -1 && !d->currentItemHidden)
                itemEntered(d->currentItem);
            break;

        default:
            break;
    }
}

void AbstractItemViewPrivate::scrollUp()
{
    if (currentItem == -1)
        return;

    if (currentItem > 0) {
        changeCurrentItem(currentItem - 1);

        if (currentItem < viewportFirstItem) {
            --viewportFirstItem;
            --viewportLastItem;
            q->redrawAll();
        } else {
            q->drawItem(currentItem + 1);
            q->redrawItem(currentItem);
        }
    }
}

void AbstractItemViewPrivate::scrollDown()
{
    if (currentItem == -1)
        return;

    if (currentItem < q->itemsCount() - 1) {
        changeCurrentItem(currentItem + 1);
        if (currentItem >= viewportLastItem) {
            ++viewportFirstItem;
            ++viewportLastItem;
            q->redrawAll();
        } else {
            q->drawItem(currentItem - 1);
            q->redrawItem(currentItem);
        }
    }
}

void AbstractItemViewPrivate::changeCurrentItem(int item)
{
    currentItem = item;
    q->currentItemChanged(item);
}

void AbstractItemView::resizeEvent(const Size& size)
{
    if (size.lines() > lines()) {
        int extraSize = size.lines() - lines();

        if (itemsCount() > d->viewportLastItem) {
            const int min = std::min(itemsCount() - d->viewportLastItem, extraSize);
            d->viewportLastItem += min;
            extraSize -= min;
        }

        if (extraSize > 0 && d->viewportFirstItem > 0) {
            const int min = std::min(d->viewportFirstItem, extraSize);
            d->viewportFirstItem -= min;
            extraSize -= min;
        }
    }

    if (size.lines() < lines()) {
        int sizeDiff = lines() - size.lines();

        if (lines() > itemsCount())
            sizeDiff -= std::min(lines() - itemsCount(), sizeDiff);

        if (sizeDiff > 0 && d->currentItem < d->viewportLastItem - 1) {
            const int min = std::min(d->viewportLastItem - 1 - d->currentItem, sizeDiff);
            d->viewportLastItem -= min;
            sizeDiff -= min;
        }

        if (sizeDiff > 0)
            d->viewportFirstItem += sizeDiff;
    }

    Window::resizeEvent(size);
}

void AbstractItemView::hideCurrentItem()
{
    d->hideSelectionTimer.stop();
    d->currentItemHidden = true;
    redrawItem(d->currentItem);
}

AbstractItemView::~AbstractItemView()
{

}


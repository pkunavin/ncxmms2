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

#ifndef ABSTRACTITEMVIEW_H
#define ABSTRACTITEMVIEW_H

#include "window.h"

namespace ncxmms2 {

class AbstractItemViewPrivate;

class AbstractItemView : public Window
{
public:
    AbstractItemView(int lines, int cols, int yPos, int xPos, Window *parent = 0);
    ~AbstractItemView();

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resizeEvent(const Size& size);

    int currentItem() const;
    void setCurrentItem(int item);
    bool isCurrentItemHidden() const;
    void setHideCurrentItemInterval(unsigned int sec);
    void showCurrentItem();
    void hideCurrentItem();

    typedef boost::function<void (int)> CurrentItemChangedCallback;
    void setCurrentItemChangedCallback(const CurrentItemChangedCallback& callback);

protected:
    virtual void drawItem(int item) = 0;
    virtual int itemsCount() const = 0;

    int itemLine(int item) const;

    int viewportFirstItem() const;
    void setViewportFirstItem(int item);

    void redrawItem(int item);
    void redrawItems(int first, int last);
    void redrawAll();
    void reset();

    void itemAdded();
    void itemInserted(int item);
    void itemRemoved(int item);
    void itemMoved(int from, int to);

    virtual void itemEntered(int item);

    virtual void showEvent();

private:
    friend class AbstractItemViewPrivate;
    std::unique_ptr<AbstractItemViewPrivate> d;
};
} // ncxmms2

#endif // ABSTRACTITEMVIEW_H

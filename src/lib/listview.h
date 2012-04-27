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

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "window.h"

namespace ncxmms2 {

class ListModel;
class ListModelItemDelegate;
class ListViewPrivate;

class ListView : public Window
{
public:
    ListView(int lines, int cols, int yPos, int xPos, Window *parent = nullptr);
    ~ListView();

    void setModel(ListModel *model);
    ListModel *model() const;

    void setItemDelegate(ListModelItemDelegate *delegate);
    ListModelItemDelegate *itemDelegate() const;

    int currentItem() const;
    void setCurrentItem(int item);
    bool isCurrentItemHidden() const;
    void setHideCurrentItemInterval(unsigned int sec);
    void showCurrentItem();
    void hideCurrentItem();

    void scrollToItem(int item);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resizeEvent(const Size& size);

    // Signals
    NCXMMS2_SIGNAL(currentItemChanged, int)
    NCXMMS2_SIGNAL(itemEntered, int)

protected:
     virtual void paint(const Rectangle &rect);

private:
    std::unique_ptr<ListViewPrivate> d;
    friend class ListViewPrivate;
};
} // ncxmms2

#endif // LISTVIEW_H

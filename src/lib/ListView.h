/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <vector>
#include "Window.h"

namespace ncxmms2 {

class ListModel;
class ListModelItemDelegate;
class ListViewPrivate;

class ListView : public Window
{
public:
    ListView(const Rectangle& rect, Window *parent = nullptr);
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

    int viewportFirstItem() const;
    int viewportLastItem() const;

    void setViewportFirstItem(int item);

    // Selection routines
    const std::vector<int>& selectedItems() const; // Returns sorted list of selected items
    void selectItem(int item);
    void unselectItem(int item);
    bool isItemSelected(int item) const;
    void clearSelection();
    void invertSelection();
    void selectItemsByRegExp(const std::string& pattern);
    void unselectItemsByRegExp(const std::string& pattern);
    void selectItems(const std::function<bool (int)>& predicate);
    void unselectItems(const std::function<bool (int)>& predicate);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void mouseEvent(const MouseEvent& ev);
    virtual void resize(const Size& size);

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

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

#include <vector>
#include <glib.h>

#include "size.h"
#include "keyevent.h"
#include "mouseevent.h"
#include "listview.h"
#include "listmodel.h"
#include "listmodelitemdata.h"
#include "listmodelitemdelegate.h"
#include "listitempaintoptions.h"
#include "timer.h"
#include "painter.h"

namespace ncxmms2 {

class ListViewPrivate
{
public:
    ListViewPrivate(ListView *_q) :
        q(_q),
        model(nullptr),
        delegate(nullptr),
        currentItem(-1),
        viewportBeginItem(-1),
        viewportEndItem(-1),
        currentItemHidden(false),
        hideCurrentItemSelectionInterval(0) {}

    ListView *q;
    ListModel *model;
    ListModelItemDelegate *delegate;

    int currentItem;
    int viewportBeginItem;
    int viewportEndItem;

    int itemLine(int item) const {return item - viewportBeginItem;}

    std::vector<int> selectedItems;

    Timer hideSelectionTimer;
    bool currentItemHidden;
    unsigned int hideCurrentItemSelectionInterval;

    std::vector<Signals::connection> modelConnections;
    void disconnectModel();

    void reset();
    void itemsChanged(int first, int last);
    void itemAdded();
    void itemInserted(int item);
    void itemRemoved(int item);
    void itemMoved(int from, int to);

    void changeCurrentItem(int item);
    void scrollUp();
    void scrollDown();

    void cursorUp();
    void cursorDown();

    void toggleSelection(int item);
    void jumpToNextSelectedItem();
    void jumpToPreviousSelectedItem();

};
} // ncxmms2

using namespace ncxmms2;

ListView::ListView(const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    d(new ListViewPrivate(this))
{
    loadPalette("ListView");
    d->hideSelectionTimer.setSingleShot(true);
    d->hideSelectionTimer.timeout_Connect(&ListView::hideCurrentItem, this);
}

ListView::~ListView()
{
    delete d->delegate;
}

void ListView::setModel(ListModel *model)
{
    d->disconnectModel();
    d->model = model;

    delete d->delegate;
    d->delegate = nullptr;

    if (model) {
        d->delegate = new ListModelItemDelegate(model);

        d->modelConnections.push_back(
            model->reset_Connect(
                boost::bind(&ListViewPrivate::reset, d.get())
        ));

        d->modelConnections.push_back(
            model->itemsChanged_Connect(
                boost::bind(&ListViewPrivate::itemsChanged, d.get(), _1, _2)
        ));

        d->modelConnections.push_back(
            model->itemAdded_Connect(
                boost::bind(&ListViewPrivate::itemAdded, d.get())
        ));

        d->modelConnections.push_back(
            model->itemInserted_Connect(
                boost::bind(&ListViewPrivate::itemInserted, d.get(), _1)
        ));

        d->modelConnections.push_back(
            model->itemRemoved_Connect(
                boost::bind(&ListViewPrivate::itemRemoved, d.get(), _1)
        ));

        d->modelConnections.push_back(
            model->itemMoved_Connect(
                boost::bind(&ListViewPrivate::itemMoved, d.get(), _1, _2)
        ));
    }
    d->reset();
}

ListModel *ListView::model() const
{
    return d->model;
}

void ListView::setItemDelegate(ListModelItemDelegate *delegate)
{
    if (d->model) {
        delete d->delegate;
        d->delegate = delegate
                      ? delegate
                      : new ListModelItemDelegate(d->model);
        update();
    }
}

ListModelItemDelegate *ListView::itemDelegate() const
{
    return d->delegate;
}

int ListView::currentItem() const
{
    return d->currentItem;
}

void ListView::setCurrentItem(int item)
{
    if (d->currentItem == -1)
        return;

    assert(d->model);
    if (item < 0 || item >= d->model->itemsCount())
        return;

    if (item < d->viewportBeginItem) {
        d->viewportBeginItem = item;
        d->viewportEndItem = d->viewportBeginItem + lines();
    } else if (item >= d->viewportEndItem) {
        d->viewportEndItem = item + 1;
        d->viewportBeginItem = d->viewportEndItem - lines();
    }
    d->changeCurrentItem(item);
    update();
    showCurrentItem();
}

bool ListView::isCurrentItemHidden() const
{
    return d->currentItemHidden;
}

void ListView::setHideCurrentItemInterval(unsigned int sec)
{
    if (!d->model)
        return;

    d->hideCurrentItemSelectionInterval = sec;
    if (d->hideCurrentItemSelectionInterval) {
        d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
    } else {
        d->hideSelectionTimer.stop();
        d->currentItemHidden = false;
        d->itemsChanged(d->currentItem, d->currentItem);
    }
}

void ListView::showCurrentItem()
{
    if (!d->model)
        return;

    d->currentItemHidden = false;
    d->itemsChanged(d->currentItem, d->currentItem);
    if (d->hideCurrentItemSelectionInterval)
        d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
}

void ListView::hideCurrentItem()
{
    if (!d->model)
        return;

    d->currentItemHidden = true;
    d->itemsChanged(d->currentItem, d->currentItem);
}

void ListView::scrollToItem(int item)
{
    if (!d->model)
        return;

    if (item < 0 || item >= d->model->itemsCount())
        return;

    if (item < d->viewportBeginItem) {
        d->viewportBeginItem = item;
        d->viewportEndItem = d->viewportBeginItem + lines();
        if (!(d->currentItem >= d->viewportBeginItem && d->currentItem < d->viewportEndItem))
            d->changeCurrentItem(d->viewportEndItem - 1);
    } else if (item >= d->viewportEndItem) {
        d->viewportEndItem = item + 1;
        d->viewportBeginItem = d->viewportEndItem - lines();
        if (!(d->currentItem >= d->viewportBeginItem && d->currentItem < d->viewportEndItem))
            d->changeCurrentItem(d->viewportBeginItem);
    }
    update();
}

int ListView::viewportFirstItem() const
{
    return d->viewportBeginItem;
}

int ListView::viewportLastItem() const
{
    return d->viewportEndItem != -1 ? d->viewportEndItem - 1 : -1;
}

void ListView::setViewportFirstItem(int item)
{
    if (!d->model)
        return;

    const int itemsCount = d->model->itemsCount();
    if (item < 0 || item >= itemsCount)
        return;

    if (itemsCount <= lines())
        return;

    if (item + lines() < itemsCount) {
        d->viewportBeginItem = item;
        d->viewportEndItem = d->viewportBeginItem + lines();
    } else {
        d->viewportEndItem = itemsCount;
        d->viewportBeginItem = d->viewportEndItem - lines();
    }
    d->changeCurrentItem(d->viewportBeginItem);
    update();
}

const std::vector<int>& ListView::selectedItems() const
{
    return d->selectedItems;
}

void ListView::selectItem(int item)
{
    if (!d->model)
        return;

    auto it = std::lower_bound(d->selectedItems.begin(), d->selectedItems.end(), item);
    if (it != d->selectedItems.end() && *it == item) {
        return;
    }
    d->selectedItems.insert(it, item);
    d->itemsChanged(item, item);
}

void ListView::unselectItem(int item)
{
    if (!d->model)
        return;

    auto it = std::lower_bound(d->selectedItems.begin(), d->selectedItems.end(), item);
    if (it != d->selectedItems.end() && *it == item) {
        d->selectedItems.erase(it);
        d->itemsChanged(item, item);
    }
}

bool ListView::isItemSelected(int item) const
{
    return std::binary_search(d->selectedItems.begin(), d->selectedItems.end(),
                              item);
}

void ListView::clearSelection()
{
    if (!d->model)
        return;

    if (!d->selectedItems.empty()) {
        const int first = *d->selectedItems.begin();
        const int last = *d->selectedItems.rbegin();
        d->selectedItems.clear();
        d->itemsChanged(first, last);
    }
}

void ListView::invertSelection()
{
    if (!d->model)
        return;

    const int itemsCount = d->model->itemsCount();
    std::vector<int> invertedSelection;
    invertedSelection.reserve(itemsCount - d->selectedItems.size());
    for (int item = 0; item < itemsCount; ++item) {
        if (!isItemSelected(item)) {
            invertedSelection.push_back(item);
        }
    }
    d->selectedItems.swap(invertedSelection);
    update();
}

void ListView::selectItemsByRegExp(const std::string& pattern)
{
    if (!d->model)
        return;

    ListModel *model = d->model;
    const int itemsCount = model->itemsCount();
    ListModelItemData itemData;

    GRegex *regex = g_regex_new(pattern.c_str(), G_REGEX_OPTIMIZE, (GRegexMatchFlags)0, nullptr);
    if (!regex)
        return;

    for (int item = 0; item < itemsCount; ++item) {
        model->data(item, &itemData);
        if (g_regex_match(regex, itemData.textPtr->c_str(), (GRegexMatchFlags)0, nullptr)) {
            auto it = std::lower_bound(d->selectedItems.begin(), d->selectedItems.end(), item);
            if (it != d->selectedItems.end() && *it == item) {
                continue;
            }
            d->selectedItems.insert(it, item);
        }
    }
    update();
    g_regex_unref(regex);
}

void ListView::unselectItemsByRegExp(const std::string &pattern)
{
    if (!d->model)
        return;

    ListModel *model = d->model;
    ListModelItemData itemData;

    GRegex *regex = g_regex_new(pattern.c_str(), G_REGEX_OPTIMIZE, (GRegexMatchFlags)0, nullptr);
    if (!regex)
        return;

    std::vector<int> leftSelectedItems;
    for (int item : d->selectedItems) {
        model->data(item, &itemData);
        if (!g_regex_match(regex, itemData.textPtr->c_str(), (GRegexMatchFlags)0, nullptr)) {
            leftSelectedItems.push_back(item);
        }
    }
    d->selectedItems.swap(leftSelectedItems);
    update();
    g_regex_unref(regex);
}

void ListView::selectItems(const boost::function<bool (int)>& predicate)
{
    if (!d->model || predicate.empty())
        return;

    const int itemsCount = d->model->itemsCount();
    for (int item = 0; item < itemsCount; ++item) {
        if (predicate(item)) {
            auto it = std::lower_bound(d->selectedItems.begin(), d->selectedItems.end(), item);
            if (it != d->selectedItems.end() && *it == item) {
                continue;
            }
            d->selectedItems.insert(it, item);
        }
    }
    update();
}

void ListView::unselectItems(const boost::function<bool (int)>& predicate)
{
    if (!d->model || predicate.empty())
        return;

    std::vector<int> leftSelectedItems;
    for (int item : d->selectedItems) {
        if (!predicate(item)) {
            leftSelectedItems.push_back(item);
        }
    }
    d->selectedItems.swap(leftSelectedItems);
    update();
}

void ListViewPrivate::disconnectModel()
{
    for (auto& connection : modelConnections) {
        connection.disconnect();
    }
}

void ListView::paint(const Rectangle& rect)
{
    if (G_LIKELY(d->model)) {
        int item = rect.y() + d->viewportBeginItem;
        const int lastItem = std::min(item + rect.lines(), d->viewportEndItem);
        const Palette *_palette = &palette();
        const bool focused = hasFocus();

        for (; item < lastItem; ++item) {
            int stateFlags = ListItemStateRegular;
            const bool selected = std::binary_search(d->selectedItems.begin(),
                                                     d->selectedItems.end(),
                                                     item);
            if (selected)
                stateFlags |= ListItemStateSelected;

            if (item == d->currentItem && !d->currentItemHidden)
                stateFlags |= ListItemStateCurrent;

            const ListItemPaintOptions options(_palette,
                                               Rectangle(0, d->itemLine(item), cols(), 1),
                                               stateFlags,
                                               focused);

            Painter painter(this);
            d->delegate->paint(&painter, options, item);
        }

        Painter painter(this);
        if (d->viewportBeginItem == -1) {
            painter.clearWindow();
        } else {
            for (int i = d->viewportEndItem; i < lines(); ++i) {
                painter.clearLine(i);
            }
        }
        painter.flush();
    } else {
        Painter painter(this);
        painter.clearWindow();
        painter.flush();
    }
}

void ListView::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (!d->model)
        return;

    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:
            d->cursorUp();
            break;

        case KeyEvent::KeyDown:
            d->cursorDown();
            break;

        case KeyEvent::KeyEnter:
            if (d->currentItem != -1 && !d->currentItemHidden)
                itemEntered(d->currentItem);
            break;

        case KeyEvent::KeyInsert:
            if (d->currentItem != -1 && !d->currentItemHidden) {
                d->toggleSelection(d->currentItem);
                d->scrollDown();
                if (d->hideCurrentItemSelectionInterval)
                    d->hideSelectionTimer.start(d->hideCurrentItemSelectionInterval);
            }
            break;

        case '*':
            invertSelection();
            break;

        case '.':
            d->jumpToNextSelectedItem();
            break;

        case ',':
            d->jumpToPreviousSelectedItem();
            break;

        case KeyEvent::KeyHome:
            setCurrentItem(0);
            break;

        case KeyEvent::KeyEnd:
            setCurrentItem(d->model->itemsCount() - 1);
            break;

        default:
            break;
    }
}

void ListView::mouseEvent(const MouseEvent& ev)
{
    if (!d->model)
        return;

    switch (ev.type()) {
        case MouseEvent::Type::ButtonPress:
            switch (ev.button()) {
                case MouseEvent::ButtonLeft:
                    setCurrentItem(d->viewportBeginItem + ev.position().y());
                    break;

                case MouseEvent::ButtonRight:
                    // FIXME: With this simple implementation selection may flicker.
                    setCurrentItem(d->viewportBeginItem + ev.position().y());
                    if (d->currentItem != -1) {
                        d->toggleSelection(d->currentItem);
                    }
                    break;

                case MouseEvent::WheelUp:
                    d->cursorUp();
                    break;

                case MouseEvent::WheelDown:
                    d->cursorDown();
                    break;
            }
            break;

        case MouseEvent::Type::ButtonDoubleClick:
            // Assume here that item under mouse is already current item,
            // that is true because ButtonDoubleClick always happens after ButtonPress
            // at the same point.
            if (ev.button() == MouseEvent::ButtonLeft) {
                if (d->currentItem != -1 && !d->currentItemHidden)
                    itemEntered(d->currentItem);
            }
            break;

        default:
            break;
    }
}

void ListView::resize(const Size& size)
{
    const int itemsCount = d->model ? d->model->itemsCount() : 0;

    if (itemsCount > 0) {

        if (size.lines() > lines()) {
            int extraSize = size.lines() - lines();

            if (itemsCount > d->viewportEndItem) {
                const int min = std::min(itemsCount - d->viewportEndItem, extraSize);
                d->viewportEndItem += min;
                extraSize -= min;
            }

            if (extraSize > 0 && d->viewportBeginItem > 0) {
                const int min = std::min(d->viewportBeginItem, extraSize);
                d->viewportBeginItem -= min;
                extraSize -= min;
            }
        }

        if (size.lines() < lines()) {
            int sizeDiff = lines() - size.lines();

            if (lines() > itemsCount)
                sizeDiff -= std::min(lines() - itemsCount, sizeDiff);

            if (sizeDiff > 0 && d->currentItem < d->viewportEndItem - 1) {
                const int min = std::min(d->viewportEndItem - 1 - d->currentItem, sizeDiff);
                d->viewportEndItem -= min;
                sizeDiff -= min;
            }

            if (sizeDiff > 0)
                d->viewportBeginItem += sizeDiff;
        }
    }
    Window::resize(size);
}

void ListViewPrivate::reset()
{
    const int itemsCount = model ? model->itemsCount() : 0;

    selectedItems.clear();

    if (itemsCount > 0) {
        changeCurrentItem(0);
        viewportBeginItem = 0;
        viewportEndItem = itemsCount > q->lines() ? q->lines() : itemsCount;
        if (hideCurrentItemSelectionInterval)
            hideSelectionTimer.start(hideCurrentItemSelectionInterval);
    } else {
        changeCurrentItem(-1);
        viewportBeginItem = -1;
        viewportEndItem = -1;
    }

    q->update();
}

void ListViewPrivate::itemsChanged(int first, int last)
{
    assert(last >= first);
    const int yPos = std::max(first, viewportBeginItem) - viewportBeginItem;
    const int lines = std::min(last + 1, viewportEndItem) - viewportBeginItem - yPos;
    if (lines > 0)
        q->update(Rectangle(0, yPos, q->cols(), lines));
}

void ListViewPrivate::itemAdded()
{
    if (currentItem == -1)
        reset();

    const int itemsCount = model->itemsCount();
    if (q->lines() >= itemsCount) {
        viewportEndItem = itemsCount;
        itemsChanged(itemsCount - 1, itemsCount - 1);
    }
}

void ListViewPrivate::itemInserted(int item)
{
    if (currentItem == -1)
        reset();

    const int itemsCount = model->itemsCount();
    if (q->lines() >= itemsCount)
        viewportEndItem = itemsCount;

    auto it = std::lower_bound(selectedItems.begin(), selectedItems.end(), item);
    if (it != selectedItems.end()) {
        std::for_each(it, selectedItems.end(), [](int& item){
             ++item;
        });
    }

    itemsChanged(item, itemsCount - 1);
}

void ListViewPrivate::itemRemoved(int item)
{
    const int itemsCount = model->itemsCount();

    if (currentItem > item || currentItem == itemsCount) {
        changeCurrentItem(currentItem - 1);
    } else if (currentItem == item) {
        /* Current item number is not changed, but items are moving,
    so information corresponding to the current item is changed */
        changeCurrentItem(currentItem);
    }

    auto it = std::lower_bound(selectedItems.begin(), selectedItems.end(), item);
    if (it != selectedItems.end()) {
        std::for_each(it, selectedItems.end(), [](int& item){
             --item;
        });
        if (*it + 1 == item)
            selectedItems.erase(it);
    }

    if (q->lines() > itemsCount) {
        viewportEndItem = itemsCount;
        q->update();
    } else {
        if (item < viewportBeginItem) {
            --viewportBeginItem;
            --viewportEndItem;
        } else if (item < viewportEndItem) {
            if (itemsCount >= viewportEndItem) {
                itemsChanged(item, viewportEndItem - 1);
        } else {
                --viewportBeginItem;
                --viewportEndItem;
                q->update();
            }
        }
    }
}

void ListViewPrivate::itemMoved(int from, int to)
{
    auto it = std::lower_bound(selectedItems.begin(), selectedItems.end(), from);
    bool itemSelected = false;
    if (it != selectedItems.end()) {
        std::for_each(it, selectedItems.end(), [](int& item){
             --item;
        });
        if (*it + 1 == from) {
            selectedItems.erase(it);
            itemSelected = true;
        }
    }

    it = std::lower_bound(selectedItems.begin(), selectedItems.end(), to);
    if (it != selectedItems.end()) {
        std::for_each(it, selectedItems.end(), [](int& item){
             ++item;
        });
    }
    if (itemSelected)
        selectedItems.insert(it, to);

    q->update();
}

void ListViewPrivate::changeCurrentItem(int item)
{
    currentItem = item;
    q->currentItemChanged(item);
}

void ListViewPrivate::scrollUp()
{
    if (currentItem == -1)
        return;

    if (currentItem > 0) {
        changeCurrentItem(currentItem - 1);

        if (currentItem < viewportBeginItem) {
            --viewportBeginItem;
            --viewportEndItem;
            q->update();
        } else {
            q->update(Rectangle(0, itemLine(currentItem), q->cols(), 2));
        }
    }
}

void ListViewPrivate::scrollDown()
{
    if (currentItem == -1)
        return;

    if (currentItem < model->itemsCount() - 1) {
        changeCurrentItem(currentItem + 1);
        if (currentItem >= viewportEndItem) {
            ++viewportBeginItem;
            ++viewportEndItem;
            q->update();
        } else {
            q->update(Rectangle(0, itemLine(currentItem - 1), q->cols(), 2));
        }
    }
}

void ListViewPrivate::cursorUp()
{
    if (currentItem == -1)
        return;

    if (currentItemHidden) {
        currentItemHidden = false;
        q->update(Rectangle(0, itemLine(currentItem), q->cols(), 1));
    } else {
        scrollUp();
    }
    if (hideCurrentItemSelectionInterval)
        hideSelectionTimer.start(hideCurrentItemSelectionInterval);
}

void ListViewPrivate::cursorDown()
{
    if (currentItem == -1)
        return;

    if (currentItemHidden) {
        currentItemHidden = false;
        q->update(Rectangle(0, itemLine(currentItem), q->cols(), 1));
    } else {
        scrollDown();
    }
    if (hideCurrentItemSelectionInterval)
        hideSelectionTimer.start(hideCurrentItemSelectionInterval);
}

void ListViewPrivate::toggleSelection(int item)
{
    assert(item >= 0 && item < q->model()->itemsCount());
    auto it = std::lower_bound(selectedItems.begin(), selectedItems.end(), item);
    if (it != selectedItems.end() && *it == item) {
        selectedItems.erase(it);
    } else {
        selectedItems.insert(it, item);
    }
    itemsChanged(item, item);
}

void ListViewPrivate::jumpToNextSelectedItem()
{
    if (selectedItems.empty())
        return;

    auto it = std::upper_bound(selectedItems.begin(), selectedItems.end(), currentItem);
    if (it != selectedItems.end()) {
        q->setCurrentItem(*it);
    } else {
        q->showCurrentItem();
    }
}

void ListViewPrivate::jumpToPreviousSelectedItem()
{
    if (selectedItems.empty())
        return;

    auto it = std::lower_bound(selectedItems.begin(), selectedItems.end(), currentItem);
    if (it != selectedItems.begin()) {
        q->setCurrentItem(*(--it));
    } else {
        q->showCurrentItem();
    }
}

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

#include "listviewappintegrated.h"
#include "../statusarea/statusarea.h"
#include "../lib/keyevent.h"

using namespace ncxmms2;

ListViewAppIntegrated::ListViewAppIntegrated(const Rectangle& rect, Window *parent) :
    ListView(rect, parent)
{
    loadPalette("ListViewAppIntegrated");
}

void ListViewAppIntegrated::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyInsert: // Toggle selection
            if (!isCurrentItemHidden()) {
                ListView::keyPressedEvent(keyEvent);
                StatusArea::showMessage("%1% items selected", selectedItems().size());
            }
            break;

        case '*': // Invert selection
            ListView::keyPressedEvent(keyEvent);
            StatusArea::showMessage("%1% items selected", selectedItems().size());
            break;

        case '+': // Select be regexp
        {
            auto resultCallback = [this](const std::string& pattern, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted) {
                    selectItemsByRegExp(pattern);
                    StatusArea::showMessage("%1% items selected", selectedItems().size());
                }
            };
            StatusArea::askQuestion("Select items: ", resultCallback, ".*");
            break;
        }

        case '\\': // Unselect be regexp
        {
            auto resultCallback = [this](const std::string& pattern, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted) {
                    unselectItemsByRegExp(pattern);
                    StatusArea::showMessage("%1% items selected", selectedItems().size());
                }
            };
            StatusArea::askQuestion("Unselect items: ", resultCallback, ".*");
            break;
        }

        case '.': // Jump to next selected item
        {
            if (selectedItems().empty()) {
                StatusArea::showMessage("No items selected!");
                return;
            }
            const int oldCurrentItem = currentItem();
            ListView::keyPressedEvent(keyEvent);
            if (isItemSelected(currentItem()) && currentItem() == oldCurrentItem) {
                StatusArea::showMessage("The last selected item reached!");
            }
            break;
        }

        case ',': // Jump to previous selected item
        {
            if (selectedItems().empty()) {
                StatusArea::showMessage("No items selected!");
                return;
            }
            const int oldCurrentItem = currentItem();
            ListView::keyPressedEvent(keyEvent);
            if (isItemSelected(currentItem()) && currentItem() == oldCurrentItem) {
                StatusArea::showMessage("The first selected item reached!");
            }
            break;
        }

        default: ListView::keyPressedEvent(keyEvent);
    }
}

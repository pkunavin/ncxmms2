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

#include "ListModel.h"
#include "ListModelItemDelegate.h"
#include "ListItemPaintOptions.h"
#include "ListModelItemData.h"
#include "Painter.h"
#include "Palette.h"
#include "Application.h"

using namespace ncxmms2;

ListModelItemDelegate::ListModelItemDelegate(const ListModel *model) :
    m_model(model)
{

}

ListModelItemDelegate::~ListModelItemDelegate()
{

}

void ListModelItemDelegate::paint(Painter *painter, const ListItemPaintOptions& options, int item)
{
    const Palette *palette = options.palette;
    const Palette::ColorGroup colorGroup = options.hasFocus
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;

    if (Application::useColors()) {
        if (options.state & ListItemStateCurrent) {
            painter->setColorPair(palette->color(colorGroup, Palette::RoleHighlightedText),
                                  palette->color(colorGroup, Palette::RoleHighlight));
        }
        if (options.state & ListItemStateSelected) {
            painter->setColor(palette->color(colorGroup, Palette::RoleSelection));
            painter->setBold(true);
        } else if (options.state == ListItemStateRegular) {
            painter->setColor(palette->color(colorGroup, Palette::RoleText));
        }
        painter->clearLine(options.rect.y());
    } else {
        if (options.state & ListItemStateCurrent) {
            painter->fillLine(options.rect.y(), ColorBlack);
            painter->setColor(ColorBlack);
            painter->setReverse(true);
        }
        if (options.state & ListItemStateSelected)
            painter->setBold(true);

        if (!(options.state & ListItemStateCurrent))
            painter->clearLine(options.rect.y());
    }

    ListModelItemData itemData;
    model()->data(item, &itemData);
    if (itemData.bold && !(options.state & ListItemStateCurrent)) {
        painter->setBold(true);
    }
    painter->squeezedPrint(*itemData.textPtr, options.rect.cols());
}

const ListModel *ListModelItemDelegate::model() const
{
    return m_model;
}


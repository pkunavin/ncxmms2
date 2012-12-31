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

#include "listmodel.h"
#include "listmodelitemdelegate.h"
#include "listitempaintoptions.h"
#include "listmodelitemdata.h"
#include "painter.h"
#include "palette.h"
#include "application.h"

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

    if (options.state & ListItemStateCurrent) {
        if (Application::useColors()) {
            painter->setColorPair(palette->color(colorGroup, Palette::RoleHighlightedText),
                                  palette->color(colorGroup, Palette::RoleHighlight));
            painter->clearLine(options.rect.y());
        } else {
            painter->fillLine(options.rect.y(), ColorBlack);
            painter->setColor(ColorBlack);
            painter->setReverse(true);
        }
    } else if (options.state & ListItemStateSelected) {
        painter->setColor(palette->color(colorGroup, Palette::RoleSelection));
        painter->clearLine(options.rect.y());
        painter->setBold(true);
    } else {
        painter->setColor(palette->color(colorGroup, Palette::RoleText));
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


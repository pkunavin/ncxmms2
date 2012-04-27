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
    // TODO: use colors from color scheme
    if (options.state == ListItemStateCurrent) {
        painter->fillLine(options.rect.yPos(), ColorYellow);
        painter->setColor(ColorYellow);
        painter->setReverse(true);
    } else {
        painter->setColor(ColorYellow );
        painter->clearLine(options.rect.yPos());
    }

    ListModelItemData itemData;
    model()->data(item, &itemData);
    painter->squeezedPrint(*itemData.textPtr, options.rect.cols());
}

const ListModel *ListModelItemDelegate::model() const
{
    return m_model;
}


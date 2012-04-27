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

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include "object.h"

namespace ncxmms2 {

struct ListModelItemData;

class ListModel : public Object
{
public:
    ListModel(Object *parent = nullptr);
    ~ListModel();

    virtual int itemsCount() const = 0;
    virtual void data(int item, ListModelItemData *itemData) const = 0;

    // Signals
    NCXMMS2_SIGNAL(reset)
    NCXMMS2_SIGNAL(itemsChanged, int, int)
    NCXMMS2_SIGNAL(itemAdded)
    NCXMMS2_SIGNAL(itemInserted, int)
    NCXMMS2_SIGNAL(itemRemoved, int)
    NCXMMS2_SIGNAL(itemMoved, int, int)
};
} // ncxmms2

#endif // LISTMODEL_H

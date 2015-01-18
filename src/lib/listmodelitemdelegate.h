/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef LISTMODELITEMDELEGATE_H
#define LISTMODELITEMDELEGATE_H

namespace ncxmms2 {

class ListModel;
class Painter;
class Rectangle;
class ListItemPaintOptions;

class ListModelItemDelegate
{
public:
    ListModelItemDelegate(const ListModel *model);
    virtual ~ListModelItemDelegate();

    virtual void paint(Painter *painter, const ListItemPaintOptions& options, int item);

    const ListModel *model() const;

private:
    const ListModel *m_model;

};
} // ncxmms2

#endif // LISTMODELITEMDELEGATE_H

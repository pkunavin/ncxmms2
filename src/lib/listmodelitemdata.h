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

#ifndef LISTMODELITEMDATA_H
#define LISTMODELITEMDATA_H

#include <string>

namespace ncxmms2 {

struct ListModelItemData
{
    std::string text;
    const std::string *textPtr;

    bool bold;
    // TODO: Add more data fields: font settings, etc...

    ListModelItemData() :
        textPtr(&text),
        bold(false) {}
};
} // ncxmms2

#endif // LISTMODELITEMDATA_H

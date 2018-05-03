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

#include "StringAlgo.h"

namespace ncxmms2 {

void toLowerAscii(char *str)
{
    while (*str) {
        *str = g_ascii_tolower(*str);
        ++str;
    }
}

void toLowerAscii(std::string *str)
{
    auto begin = str->begin();
    auto end = str->end();
    while (begin != end) {
        *begin = g_ascii_tolower(*begin);
        ++begin;
    }
}

} // ncxmms2

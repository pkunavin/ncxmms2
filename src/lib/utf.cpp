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

#include <glib.h>
#include <utility>
#include "utf.h"

using namespace ncxmms2;

std::u32string ncxmms2::utf8ToU32String(const std::string& str)
{
    std::u32string result;
    if (!g_utf8_validate(str.c_str(), str.size(), NULL))
        return result;

    result.reserve(str.size());

    const char *c_str = str.c_str();
    while (*c_str) {
        result.push_back(g_utf8_get_char(c_str));
        c_str = g_utf8_next_char(c_str);
    }

    return std::move(result);
}

std::string ncxmms2::u32stringToUtf8(const std::u32string& str)
{
    std::string result;
    result.reserve(str.size() * 3);

    char buf[6];
    int n;
    for (auto ch : str) {
        n = g_unichar_to_utf8(ch, buf);
        result.append(buf, n);
    }

    return std::move(result);
}

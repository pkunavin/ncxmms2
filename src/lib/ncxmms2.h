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

#ifndef NCXMMS2_H
#define NCXMMS2_H

#include <utility>
#include <memory>

// Put global stuff here

#define NCXMMS2_UNUSED(x) (void)x

#if defined(__GNUC__) && __GNUC__ >= 4
#define NCXMMS2_LIKELY(x)   (__builtin_expect((x), 1))
#define NCXMMS2_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define NCXMMS2_LIKELY(x)   (x)
#define NCXMMS2_UNLIKELY(x) (x)
#endif

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)... ));
}

#endif // NCXMMS2_H

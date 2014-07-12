/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef STRINGALGO_H
#define STRINGALGO_H

#include <glib.h>
#include <string>

#include "ncxmms2.h"

namespace ncxmms2 {

template <char... Args>
struct IsAnyOfImpl;

template <char Arg, char... Args>
struct IsAnyOfImpl<Arg, Args...>
{
    static bool isAnyOf(char ch)
    {
        return ch == Arg || IsAnyOfImpl<Args...>::isAnyOf(ch);
    }
};

template <>
struct IsAnyOfImpl<>
{
    static bool isAnyOf(char)
    {
        return false;
    }
};

template <char... Args>
bool isAnyOf(char ch)
{
    return IsAnyOfImpl<Args...>::isAnyOf(ch);
}

/*   readUntil returns a pointer to a first occurrence of the any of the characters
 * provided in Args or returns a pointer to the end of the string.
 * This function in essence is a find algorithm. The main differences from
 * std::find are:
 *  - readUntil accepts multiple characters to find
 *  - characters to find should be complile time constants
 *  - for null-terminated strings pointer to the end is not needed
 */
template <char... Args>
const char * readUntil(const char *str)
{
    while (*str && !isAnyOf<Args...>(*str))
        ++str;
    return str;
}

/*   readUntilIf is the same as readUntil but also stops at the charecters for
 * which predicate f is true.
 */
template <char... Args, typename F>
const char * readUntilIf(const char *str, F f)
{
    while (*str && !isAnyOf<Args...>(*str) && !f(*str))
        ++str;
    return str;
}

template <char... Args>
const char * readUntil(const char *begin, const char *end)
{
    while (begin != end && !isAnyOf<Args...>(*begin))
        ++begin;
    return begin;
}

template <char... Args, typename F>
const char * readUntilIf(const char *begin, const char *end, F f)
{
    while (begin != end && !isAnyOf<Args...>(*begin) && !f(*begin))
        ++begin;
    return begin;
}

/*   forEachLine calls a function object f for the each line in the text.
 * The following newline charecters are supported: \n, \r and \r\n.
 *  Function should be of the form:
 * void f(const char *lineBegin, const char *lineEnd)
 */
template <typename Function>
void forEachLine(const char *text, Function f)
{
    auto readLine = [](const char *p){return readUntil<'\n', '\r'>(p);};
    
    const char *p = text;
    while (*p) {
        const char *lineEnd = readLine(p);
        f(p, lineEnd);
        p = lineEnd;
        if (NCXMMS2_LIKELY(*p)) {
            if (NCXMMS2_UNLIKELY(*p == '\r' && *(p + 1) == '\n')) {
                p += 2;
            } else {
                ++p;
            }
        }
    }
}

// Checks whether the string str begins with prefix
inline bool startsWith(const char *str, const char *prefix)
{
    return g_str_has_prefix(str, prefix);
}

inline bool startsWith(const std::string& str, const char *prefix)
{
    return startsWith(str.c_str(), prefix);
}

// Checks whether the string str ends with suffix
inline bool endsWith(const char *str, const char *suffix)
{
    return g_str_has_suffix(str, suffix);
}

inline bool endsWith(const std::string& str, const char *suffix)
{
    return endsWith(str.c_str(), suffix);
}

// Converts a string to ASCII lower case
void toLowerAscii(char *str);
void toLowerAscii(std::string *str);

} // ncxmms2

#endif // STRINGALGO_H

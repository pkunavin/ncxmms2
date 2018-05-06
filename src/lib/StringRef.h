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

#ifndef STRINGREF_H
#define STRINGREF_H

#include <cstring>

namespace ncxmms2 {

/*   StringRef is a thin non-owning wrapper around const char* string (similiar to
 * QLatin1String in Qt, but even lighter because it doesn't store a size of a string).
 *   Motivation: To use a plain C string in a map-like containers as a key or to
 * do sorting a proper set of comparison operators should be provided. This class
 * solves exactly this problem.
 */
class StringRef
{
public:
    StringRef(const char *str = nullptr) : m_str(str) {}
    
    void assign(const char *str) {m_str = str;}
    
    const char *c_str() const {return m_str;}
    
    bool isNull() const {return !m_str;}
    
private:
    const char *m_str;
};

inline bool operator==(const StringRef& lhs, const StringRef& rhs)
{
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

inline bool operator<(const StringRef& lhs, const StringRef& rhs)
{
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

inline bool operator>(const StringRef& lhs, const StringRef& rhs)
{
    return std::strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

} // ncxmms2

#endif // STRINGREF_H

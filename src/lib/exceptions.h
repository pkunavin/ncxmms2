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

#ifndef NCXMMS2_EXCEPTIONS_H
#define NCXMMS2_EXCEPTIONS_H

#include <stdexcept>

namespace ncxmms2 {

class WindowSizeError : public std::runtime_error
{
public:
    explicit WindowSizeError(const std::string& errorString) :
        std::runtime_error(errorString) {}
};

class DesiredWindowSizeTooSmall : public WindowSizeError
{
public:
    DesiredWindowSizeTooSmall() :
        WindowSizeError("You are trying to set a size, which is smaller than a minimum size!"){}
};

class DesiredWindowSizeTooBig : public WindowSizeError
{
public:
    DesiredWindowSizeTooBig() :
        WindowSizeError("You are trying to set a size, which is bigger than a maximum size!"){}
};

} // ncxmms2


#endif // NCXMMS2_EXCEPTIONS_H

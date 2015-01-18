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

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <utility>

#include "../3rdparty/tinyformat/tinyformat.h"

namespace ncxmms2 {
namespace Utils {

std::string getTimeStringFromInt(int msec);

enum class FileType
{
    Media,
    Playlist,
    Unknown
};

FileType getFileType(const std::string& path);

template <typename... Args>
std::string format(const char *fmt, Args&&... args)
{
    return tfm::format(fmt, std::forward<Args>(args)...);
}

} // Utils
} // ncxmms2

#endif // UTILS_H

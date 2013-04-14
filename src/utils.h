/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <functional>
#include <boost/format.hpp>

namespace ncxmms2 {
namespace Utils {

std::string getTimeStringFromInt(int msec);

enum FileType
{
    MediaFile,
    PlaylistFile,
    UnknownFile
};

FileType getFileType(const std::string& path);

namespace FormatImpl {

class BoostFormatWrapper
{
    boost::format m_format;

    template <typename T, typename Arg>
    boost::format& applyArgs(T&& value, Arg&& arg)
    {
        return std::forward<T>(value) % std::forward<Arg>(arg);
    }

    template <typename T, typename Arg, typename... Args>
    boost::format& applyArgs(T&& value, Arg&& arg,  Args&&... args)
    {
        return  applyArgs(std::forward<T>(value) % std::forward<Arg>(arg),
                          std::forward<Args>(args)...);
    }

public:
    template <typename T>
    BoostFormatWrapper(T&& str) :
        m_format(std::forward<T>(str)) {}

    boost::format& compile()
    {
        return m_format;
    }

    template <typename... Args>
    boost::format& compile(Args&&... args)
    {
        return applyArgs(m_format, std::forward<Args>(args)...);
    }
};
} // FormatImpl

// Convenient wrapper around boost::format
// Replaces boost::format(string) % arg_1 % arg_2 ...  to format(string, arg_1, arg_2 ...)
template <typename T, typename... Args>
std::string format(T&& str, Args&&... args)
{
    return FormatImpl::BoostFormatWrapper(std::forward<T>(str)).compile(std::forward<Args>(args)...).str();
}

} // Utils
} // ncxmms2

#endif // UTILS_H

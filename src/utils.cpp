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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "utils.h"
#include "lib/stringref.h"

#include "../3rdparty/folly/sorted_vector_types.h"

using namespace ncxmms2;

std::string ncxmms2::Utils::getTimeStringFromInt(int msec)
{
    msec /= 1000;
    std::string timeString;

    if (msec < 3600) {
        timeString.reserve(5); // xx:xx

        const int min = msec/60;
        timeString.append(boost::lexical_cast<std::string>(min));

        timeString.push_back(':');

        const int sec = msec % 60;
        if (sec < 10)
            timeString.push_back('0');
        timeString.append(boost::lexical_cast<std::string>(sec));
    } else {
        timeString.reserve(8); // xx:xx:xx

        const int hours = msec / 3600;
        timeString.append(boost::lexical_cast<std::string>(hours));

        timeString.push_back(':');

        const int min = msec / 60 - 60 * hours;
        if (min < 10)
            timeString.push_back('0');
        timeString.append(boost::lexical_cast<std::string>(min));

        timeString.push_back(':');

        const int sec = msec % 60;
        if (sec < 10)
            timeString.push_back('0');
        timeString.append(boost::lexical_cast<std::string>(sec));
    }

    return timeString;
}

Utils::FileType Utils::getFileType(const std::string& path)
{
    const std::string::size_type dotPos = path.rfind('.');
    if (dotPos == std::string::npos || dotPos + 1 >= path.size() || dotPos == 0)
        return Utils::FileType::Unknown;

    std::string suffix = path.substr(dotPos + 1);
    boost::to_lower(suffix);

    static const folly::sorted_vector_set<StringRef> playlistFileSuffixes
    {"cue", "m3u", "pls", "asx"};

    static const folly::sorted_vector_set<StringRef> mediaFileSuffixes
    {"mp3", "flac", "ape", "ogg", "wma", "wav", "mp4", "aac", "alac"};

    if (mediaFileSuffixes.find(suffix.c_str()) != mediaFileSuffixes.end())
        return Utils::FileType::Media;

    if (playlistFileSuffixes.find(suffix.c_str()) != playlistFileSuffixes.end())
        return Utils::FileType::Playlist;

     return Utils::FileType::Unknown;
}

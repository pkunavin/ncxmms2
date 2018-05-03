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

#include "Utils.h"
#include "lib/StringRef.h"
#include "lib/StringAlgo.h"

#include "../3rdparty/folly/sorted_vector_types.h"

using namespace ncxmms2;

std::string ncxmms2::Utils::getTimeStringFromInt(int msec)
{
    const int sec = msec / 1000;
    
    if (sec < 3600)
        return format("%d:%02d", sec / 60, sec % 60);
    
    const int hours = sec / 3600;
    return format("%d:%02d:%02d", hours, sec / 60 - 60 * hours, sec % 60);
}

Utils::FileType Utils::getFileType(const std::string& path)
{
    const std::string::size_type dotPos = path.rfind('.');
    if (dotPos == std::string::npos || dotPos + 1 >= path.size() || dotPos == 0)
        return Utils::FileType::Unknown;

    std::string suffix = path.substr(dotPos + 1);
    toLowerAscii(&suffix);

    static const folly::sorted_vector_set<StringRef> playlistFileSuffixes
    {"cue", "m3u", "pls", "asx"};

    static const folly::sorted_vector_set<StringRef> mediaFileSuffixes
    {"mp3", "flac", "ape", "ogg", "wma", "wav", "mp4", "aac", "alac", "mod"};

    if (mediaFileSuffixes.find(suffix.c_str()) != mediaFileSuffixes.end())
        return Utils::FileType::Media;

    if (playlistFileSuffixes.find(suffix.c_str()) != playlistFileSuffixes.end())
        return Utils::FileType::Playlist;

     return Utils::FileType::Unknown;
}

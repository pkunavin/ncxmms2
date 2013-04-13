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

#include <xmmsclient/xmmsclient++.h>
#include "song.h"

using namespace ncxmms2;

void Song::loadInfo(const Xmms::PropDict& info)
{
    m_id = info.get<int>("id");

#define INFO_GET_STRING(var, key)             \
    do {                                      \
        var.clear();                          \
        if (info.contains(key))               \
            var = info.get<std::string>(key); \
    } while (0)

    INFO_GET_STRING(m_title, "title");
    INFO_GET_STRING(m_artist, "artist");
    INFO_GET_STRING(m_album, "album");
    INFO_GET_STRING(m_performer, "performer");
    INFO_GET_STRING(m_date, "date");
    INFO_GET_STRING(m_genre, "genre");

#undef INFO_GET_STRING

    m_url.clear();
    m_fileName.clear();
    if (info.contains("url")) {
        m_url = Xmms::decodeUrl(info.get<std::string>("url"));
        const std::string::size_type slashPos = m_url.rfind('/');
        if (!(slashPos == std::string::npos || slashPos + 1 >= m_url.size()))
            m_fileName = m_url.substr(slashPos + 1);
    }

#define INFO_GET_INT(var, key)        \
    do {                              \
        var = -1;                     \
        if (info.contains(key))       \
            var = info.get<int>(key); \
    } while (0)

    INFO_GET_INT(m_durartion, "duration");
    INFO_GET_INT(m_trackNumber, "tracknr");
    INFO_GET_INT(m_timesPlayed, "timesplayed");
    INFO_GET_INT(m_bitrate, "bitrate");
    INFO_GET_INT(m_samplerate, "samplerate");

#undef INFO_GET_INT
}


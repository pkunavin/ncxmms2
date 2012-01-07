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

#include <xmmsclient/xmmsclient++.h>
#include "song.h"
#include "utils.h"

using namespace ncxmms2;

void Song::loadInfo(const Xmms::PropDict& info)
{
    m_id = info.get<int>("id");

    m_title.clear();
    if (!info.contains("title")) {
        std::string url = Xmms::decodeUrl(info.get<std::string>("url"));
        const std::string::size_type slashPos = url.rfind('/');
        if (!(slashPos == std::string::npos || slashPos + 1 >= url.size()))
            m_title = url.substr(slashPos+1);
    } else {
        m_title = info.get<std::string>("title");
    }

    if (info.contains("artist")) {
        m_artist = info.get<std::string>("artist");
    } else {
        m_artist.clear();
    }

    if (info.contains("duration")) {
        m_durartion = info.get<int>("duration");
        m_durationString = Utils::getTimeStringFromInt(m_durartion);
    } else {
        m_durartion = 0;
        m_durationString.clear();
    }
}


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

#include "Song.h"
#include "XmmsUtils/Types.h"

using namespace ncxmms2;

void Song::loadInfo(const xmms2::PropDict& info)
{
    m_id          = info.value<int>("id");
    m_durartion   = info.value<int>("duration"   , -1);
    m_trackNumber = info.value<int>("tracknr"    , -1);
    m_timesPlayed = info.value<int>("timesplayed", -1);
    m_bitrate     = info.value<int>("bitrate"    , -1);
    m_samplerate  = info.value<int>("samplerate" , -1);
    
    m_title     = info.value<StringRef>("title"    , "").c_str();
    m_artist    = info.value<StringRef>("artist"   , "").c_str();
    m_album     = info.value<StringRef>("album"    , "").c_str();
    m_performer = info.value<StringRef>("performer", "").c_str();
    m_date      = info.value<StringRef>("date"     , "").c_str();
    m_genre     = info.value<StringRef>("genre"    , "").c_str();
    
    m_url.clear();
    m_fileName.clear();
    StringRef urlRef = info.value<StringRef>("url");
    if (!urlRef.isNull()) {
        m_url = xmms2::decodeUrl(urlRef.c_str());
        m_fileName = xmms2::getFileNameFromUrl(m_url);
    }
}


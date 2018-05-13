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
    
#define GET_STRING_TAG_VALUE(var, tag) var = info.value<StringRef>(getTagKey(Tag::tag).c_str(), "").c_str()
    GET_STRING_TAG_VALUE(m_title,       Title);
    GET_STRING_TAG_VALUE(m_artist,      Artist);
    GET_STRING_TAG_VALUE(m_album,       Album);
    GET_STRING_TAG_VALUE(m_albumArtist, AlbumArtist);
    GET_STRING_TAG_VALUE(m_performer,   Performer);
    GET_STRING_TAG_VALUE(m_composer,    Composer);
    GET_STRING_TAG_VALUE(m_date,        Year);
    GET_STRING_TAG_VALUE(m_genre,       Genre);
#undef GET_STRING_TAG_VALUE
    
    m_url.clear();
    m_fileName.clear();
    StringRef urlRef = info.value<StringRef>("url");
    if (!urlRef.isNull()) {
        m_url = xmms2::decodeUrl(urlRef.c_str());
        m_fileName = xmms2::getFileNameFromUrl(m_url);
    }
}

StringRef Song::getTagKey(Song::Tag tag)
{
    switch (tag) {
        case Tag::Title:       return "title";
        case Tag::Artist:      return "artist";
        case Tag::Album:       return "album";
        case Tag::AlbumArtist: return "album_artist";
        case Tag::Year:        return "date";
        case Tag::Genre:       return "genre";
        case Tag::Composer:    return "composer";
        case Tag::Performer:   return "performer";
        default:
            assert(false);
            return "";
    }
}


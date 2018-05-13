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

#ifndef SONG_H
#define SONG_H

#include <string>
#include "lib/StringRef.h"

namespace ncxmms2 {

namespace xmms2 {
class PropDict;
}

class Song
{
public:
    Song() :
        m_id(0),
        m_durartion(-1),
        m_trackNumber(-1),
        m_timesPlayed(-1),
        m_bitrate(-1),
        m_samplerate(-1) {}

    void loadInfo(const xmms2::PropDict& info);

    int id() const                            {return m_id;}
    int duration() const                      {return m_durartion;}
    int trackNumber() const                   {return m_trackNumber;}
    int timesPlayed() const                   {return m_timesPlayed;}
    int bitrate() const                       {return m_bitrate;}
    int samplerate() const                    {return m_samplerate;}
    const std::string& title() const          {return m_title;}
    const std::string& artist() const         {return m_artist;}
    const std::string& album() const          {return m_album;}
    const std::string& albumArtist() const    {return m_albumArtist;}
    const std::string& performer() const      {return m_performer;}
    const std::string& composer() const       {return m_composer;}
    const std::string& date() const           {return m_date;}
    const std::string& genre() const          {return m_genre;}
    const std::string& url() const            {return m_url;}
    const std::string& fileName() const       {return m_fileName;}

    enum class Tag
    {
        Title,
        Artist,
        Album,
        AlbumArtist,
        Year,
        Genre,
        Composer,
        Performer
    };

    static StringRef getTagKey(Tag tag);

private:
    int m_id;
    int m_durartion;
    int m_trackNumber;
    int m_timesPlayed;
    int m_bitrate;
    int m_samplerate;
    std::string m_title;
    std::string m_artist;
    std::string m_album;
    std::string m_albumArtist;
    std::string m_performer;
    std::string m_composer;
    std::string m_date;
    std::string m_genre;
    std::string m_url;
    std::string m_fileName;
};
} // ncxmms2

#endif // SONG_H

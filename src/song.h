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

#ifndef SONG_H
#define SONG_H

#include <string>

namespace Xmms {
class PropDict;
}

namespace ncxmms2 {

class Song
{
public:
    Song() : m_id(0), m_durartion(0) {};
    void loadInfo(const Xmms::PropDict& info);

    int id() const                            {return m_id;}
    const std::string& title() const          {return m_title;}
    const std::string& artist() const         {return m_artist;}
    const std::string& durationString() const {return m_durationString;}
    int duration() const                      {return m_durartion;}

private:
    int m_id;
    std::string m_title;
    std::string m_artist;
    std::string m_durationString;
    int m_durartion;

};
} // ncxmms2

#endif // SONG_H

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

#ifndef DIR_H
#define DIR_H

#include <string>

namespace ncxmms2 {

class Dir
{
public:
    explicit Dir(const std::string& url);

    const std::string& path() const {return m_path;}
    std::string url() const;
    std::string name() const;
    std::string protocol() const;
    Dir& cd(const std::string& dir);
    Dir& cdUp();
    
    bool isRootPath() const;

    bool operator==(const Dir& other)
    {
        return m_path == other.m_path && m_protocol == other.m_protocol;
    }
    
private:
    std::string m_path;
    std::string m_protocol;
};
} // ncxmms2

#endif // DIR_H

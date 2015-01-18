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

#include "dir.h"
#include "../lib/stringalgo.h"

using namespace ncxmms2;

namespace {
const char *dirProtocolSeparator = "://";
}

Dir::Dir(const std::string& url) :
    m_path("/"),
    m_protocol("file")
{
    cd(url);
}

std::string Dir::url() const
{
    return std::string(m_protocol).append(dirProtocolSeparator).append(m_path);
}

std::string Dir::name() const
{
    if (isRootPath())
        return m_path;
    
    const auto slashPos = m_path.rfind('/');
    if (slashPos == std::string::npos || slashPos + 1 >= m_path.size())
        return std::string();

    return m_path.substr(slashPos + 1);
}

std::string Dir::protocol() const
{
    return m_protocol;
}

Dir& Dir::cd(const std::string& dir)
{
    const size_t protocolSeparatorPos = dir.find(dirProtocolSeparator);
    const char *path;
    if (protocolSeparatorPos != std::string::npos) {
        m_protocol = dir.substr(0, protocolSeparatorPos);
        path = dir.c_str() + protocolSeparatorPos + std::strlen(dirProtocolSeparator);
        m_path.clear();
    } else {
        path = dir.c_str();
        if (path[0] == '/')
            m_protocol = "file";
    }

    if (path[0] == '/' && m_protocol == "file") {
        m_path = '/';
    }
    
    forEachToken<'/'>(path, [this](const char *begin, const char *end) {
        const size_t tokenLen = end - begin;
        if (stringsEqual(".", begin, tokenLen))
            return;
        if (stringsEqual("..", begin, tokenLen)) {
            cdUp();
        } else {
            if (!endsWith(m_path, '/') && !m_path.empty())
                m_path.push_back('/');
            m_path.append(begin, tokenLen);
        }
    });
    return *this;
}

Dir& Dir::cdUp()
{
    if (isRootPath())
        return *this;
    
    auto slashPos = m_path.rfind('/');
    if (slashPos == std::string::npos)
        return *this;
    
    m_path.resize(slashPos != 0 ? slashPos : 1);
    return *this;
}

bool Dir::isRootPath() const
{
    if (m_protocol == "file")
        return m_path == "/";
    
    return m_path.find('/') == std::string::npos;
}

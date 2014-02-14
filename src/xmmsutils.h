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

#ifndef XMMSUTILS_H
#define XMMSUTILS_H

#include <string>
#include <memory>
#include <unordered_map>

#include "lib/object.h"

namespace Xmms {
class Client;
class Dict;
}

namespace ncxmms2 {
namespace XmmsUtils {

std::unique_ptr<Xmms::Client> clientCreateAndConnect(const std::string& ipcPath);

void playlistAddPlaylistFile(Xmms::Client *xmmsClient,
                             const std::string& playlist, const std::string& file);


//FIXME: Current implementation of XmmsConfig doesn't have any sharing mechanism
// which means that every instance will have a copy of xmms2 config.
class XmmsConfig : public Object
{
public:
    XmmsConfig(Xmms::Client *xmmsClient);

    std::string getValue(const std::string& key, const std::string& defaultValue = std::string()) const;
    void setValue(const std::string& key, const std::string& value);

    // Signals
    NCXMMS2_SIGNAL(configLoaded)
    NCXMMS2_SIGNAL(valueChanged, const std::string&, const std::string&)

private:
    bool getConfig(const Xmms::Dict& dict);
    bool handleConfigChange(const Xmms::Dict& dict);

    Xmms::Client *m_xmmsClient;
    std::unordered_map<std::string, std::string> m_config;
};

} // XmmsUtils
} // ncxmms2

#endif // XMMSUTILS_H

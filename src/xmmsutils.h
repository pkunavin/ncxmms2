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

#ifndef XMMSUTILS_H
#define XMMSUTILS_H

#include <string>
#include <memory>

namespace Xmms {
class Client;
}

namespace ncxmms2 {
namespace XmmsUtils {

std::unique_ptr<Xmms::Client> clientCreateAndConnect(const std::string& ipcPath);

void playlistAddPlaylistFile(Xmms::Client *xmmsClient,
                             const std::string& playlist, const std::string& file);

} // XmmsUtils
} // ncxmms2

#endif // XMMSUTILS_H

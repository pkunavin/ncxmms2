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

#ifndef XMMS2CLIENT_H
#define XMMS2CLIENT_H

#include <xmmsclient/xmmsclient++.h>

namespace ncxmms2 {

class Xmms2Client
{
public:
    Xmms2Client();
    bool connect(const std::string& ipcPath);
    Xmms::Client *client();

private:
    void disconnectCallback();
    Xmms::Client m_xmmsClient;

    Xmms2Client(const Xmms2Client& other);
    Xmms2Client& operator=(const Xmms2Client& other);
};
} // ncxmms2

#endif // XMMS2CLIENT_H

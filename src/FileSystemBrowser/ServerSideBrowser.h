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

#ifndef SERVERSIDEBROWSER_H
#define SERVERSIDEBROWSER_H

#include "FileSystemBrowser.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class ServerSideBrowser : public FileSystemBrowser
{
public:
    ServerSideBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);
    ~ServerSideBrowser();
    
private:
    void onDirectoryLoaded(const Dir& dir);
};
} // ncxmms2

#endif // SERVERSIDEBROWSER_H

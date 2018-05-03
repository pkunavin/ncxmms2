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

#ifndef LOCALFILESYSTEMBROWSER_H
#define LOCALFILESYSTEMBROWSER_H

#include "FileSystemBrowser.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class LocalFileSystemBrowser : public FileSystemBrowser
{
public:
    LocalFileSystemBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);
    ~LocalFileSystemBrowser();
    
private:
    void onDirectoryLoaded(const Dir& dir);
};
} // ncxmms2

#endif // LOCALFILESYSTEMBROWSER_H

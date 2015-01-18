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

#include "serversidebrowser.h"
#include "serversidebrowsermodel.h"
#include "filesystemitemdelegate.h"
#include "../settings.h"

using namespace ncxmms2;

ServerSideBrowser::ServerSideBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    FileSystemBrowser(xmmsClient, rect, parent)
{
    setName("Server side browser");
    loadPalette("ServerSideBrowser");
    
    ServerSideBrowserModel *fsModel = new ServerSideBrowserModel(xmmsClient, this);
    fsModel->directoryLoaded_Connect(&ServerSideBrowser::onDirectoryLoaded, this);
    setFsModel(fsModel);
    setItemDelegate(new FileSystemItemDelegate(fsModel));
    
    std::string lastPath = Settings::value<std::string>("ServerSideBrowser", "lastPath", "/");
    setDirectory(Dir(lastPath));
}

ServerSideBrowser::~ServerSideBrowser()
{
    Settings::setValue("ServerSideBrowser", "lastPath", directory().url());
}

void ServerSideBrowser::onDirectoryLoaded(const Dir& dir)
{
    // FIXME: Path may be too long to display
    std::string path = dir.protocol() == "file" ? dir.path() : dir.url();
    setName(std::string("Server side browser: ").append(path));
}

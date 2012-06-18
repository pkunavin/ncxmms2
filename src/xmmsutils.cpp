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

#include <xmmsclient/xmmsclient++.h>
#include "xmmsutils.h"

using namespace ncxmms2;

void XmmsUtils::playlistAddPlaylistFile(Xmms::Client *xmmsClient,
                                        const std::string& playlist, const std::string& file)
{
    auto getIdList = [xmmsClient, playlist](const Xmms::Coll::Coll& idlist)
    {
        xmmsClient->playlist.addIdlist(idlist, playlist);
        return true;
    };

    xmmsClient->collection.idlistFromPlaylistFile(file)(getIdList);
}

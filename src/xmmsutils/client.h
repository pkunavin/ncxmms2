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

#ifndef CLIENT_H
#define CLIENT_H

#include <stdexcept>

#include "result.h"
#include "../utils.h"
#include "../lib/object.h"

namespace ncxmms2 {
namespace xmms2 {

class ClientPrivate;

class Client : public Object
{
public:
    Client(Object *parent = nullptr);
    ~Client();
    
    bool connect(const std::string& patch);
    void disconnect();
    
    NCXMMS2_SIGNAL(disconnected)
    
    /* **************************************
       ********* Playback subsystem *********
       ************************************** */
public:
    PlaybackStatusResult playbackStatus();
    IntResult playbackPlaytime();
    IntResult playbackCurrentId();
    
    VoidResult playbackStart();
    VoidResult playbackStop();
    VoidResult playbackPause();
    VoidResult playbackTickle();
    VoidResult playbackSeekMs(int ms);
    VoidResult playbackSeekMsRel(int ms);
    
    NCXMMS2_SIGNAL(playbackPlaytimeChanged, int)
    NCXMMS2_SIGNAL(playbackStatusChanged, PlaybackStatus)
    NCXMMS2_SIGNAL(playbackCurrentIdChanged, int)
    
    /* **************************************
       ********* Playlist subsystem *********
       ************************************** */
public:
    StringResult playlistCurrentActive();
    VoidResult playlistLoad(const std::string& playlist);
    VoidResult playlistCreat(const std::string& playlist);
    VoidResult playlistRemove(const std::string& playlist);
    VoidResult playlistClear(const std::string& playlist);
    VoidResult playlistShuffle(const std::string& playlist);
    
    VoidResult playlistAddId(const std::string& playlist, int id);
    VoidResult playlistAddUrl(const std::string& playlist, const std::string& url);
    VoidResult playlistAddRecursive(const std::string& playlist, const std::string& path);
    VoidResult playlistAddCollection(const std::string& playlist, const Collection& coll,
                                     const std::vector<std::string>& order);
    VoidResult playlistAddIdList(const std::string& playlist, const Collection& idList);
    void playlistAddPlaylistFile(const std::string& playlist, const std::string& file);
    
    VoidResult playlistRemoveEntry(const std::string& playlist, int entry);
    VoidResult playlistMoveEntry(const std::string& playlist, int from, int to);
    
    VoidResult playlistSetNext(int item);
    VoidResult playlistSetNextRel(int next);
    DictResult playlistCurrentPosition(const std::string& playlist);
    IntListResult playlistListEntries(const std::string& playlist);
    StringListResult playlistList();
    
    NCXMMS2_SIGNAL(playlistLoaded, StringRef)
    NCXMMS2_SIGNAL(playlistCurrentPositionChanged, const Dict&)
    NCXMMS2_SIGNAL(playlistChanged, const PlaylistChangeEvent&)
    
    /* **************************************
       ********* Medialib subsystem *********
       ************************************** */
public:
    PropDictResult medialibGetInfo(int id);
    
    NCXMMS2_SIGNAL(medialibEntryChanged, int)
    NCXMMS2_SIGNAL(medialibEntryAdded, int)
        
    /* **************************************
       ********* Collection subsystem *******
       ************************************** */
public:
    VoidResult collectionRename(const std::string& oldName, const std::string& newName, const std::string& kind);
    CollectionResult collectionIdListFromPlaylistFile(const std::string& file);
    DictListResult collectionQueryInfos(const Collection& coll,
                                        const std::vector<std::string>& fetch,
                                        const std::vector<std::string>& order,
                                        const std::vector<std::string>& groupBy = std::vector<std::string>(),
                                        int start = 0, int limit = 0);
    
    NCXMMS2_SIGNAL(collectionChanged, const CollectionChangeEvent&)
    
    /* **************************************
       ********* Config subsystem ***********
       ************************************** */
public:
    DictResult configValueList();
    VoidResult configSetValue(const std::string& key, const std::string& value);
    
    bool configHasValue(const std::string& key) const;
    const std::string& configValue(const std::string& key) const;
    
    NCXMMS2_SIGNAL(configLoaded)
    NCXMMS2_SIGNAL(configValuesChanged, const Dict&)
    NCXMMS2_SIGNAL(configValueChanged, const std::string&, const std::string&)
    
    /* **************************************
       ************ Exceptions **************
       ************************************** */
public:
    class ConnectionError : public std::runtime_error
    {
    public:
         template <typename... Args>
         ConnectionError(Args&&... args) :
             runtime_error(Utils::format(std::forward<Args>(args)...)) {}
    };
    
private:
    std::unique_ptr<ClientPrivate> d;
    friend class ClientPrivate;
};

} // xmms2
} // ncxmms2

#endif // CLIENT_H

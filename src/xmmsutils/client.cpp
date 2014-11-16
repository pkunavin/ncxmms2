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

#include <vector>
#include <unordered_map>
#include <xmmsclient/xmmsclient.h>
#include <xmmsclient/xmmsclient-glib.h>

#include "client.h"
#include "../log.h"

namespace ncxmms2 {
namespace xmms2 {

class ClientPrivate
{
public:
    ClientPrivate(Client *_q) :
        q(_q),
        connection(nullptr),
        connected(false),
        ml(nullptr),
        configLoadRequested(false)
         {}
     
    Client *q;
    xmmsc_connection_t *connection;
    bool connected;
    void *ml;

    std::vector<xmmsc_result_t*> broadcastsAndSignals;

    template <typename T>
    void connectBroadcastOrSignal(xmmsc_result_t *result, Signals::Signal<T>& signal)
    {
        auto *callback = new XmmsValueFunctionWrapper<T>(std::ref(signal));
        xmmsc_result_notifier_set_full(result, callback->get(), callback,
                                       XmmsValueFunctionWrapper<T>::free);
        xmmsc_result_unref(result);
        broadcastsAndSignals.push_back(result);
    }
    
    bool configLoadRequested;
    std::unordered_map<std::string, std::string> config;
    
    void getConfig(const Expected<Dict>& dict);
    void handleConfigChange(const Expected<Dict>& dict);
    
    static void disconnectCallback(void *data);
};

void ClientPrivate::getConfig(const Expected<Dict>& dict)
{
    if (dict.isError()) {
        NCXMMS2_LOG_ERROR("%s", dict.error().c_str());
        // TODO: handle error properly
        return;
    }
    
    dict->forEach([this](StringRef key, Variant value){
        std::string valueStr;
        switch (value.type()) {
            case Variant::Type::Int:
                valueStr = std::to_string(value.value<int>());
                break;
                
            case Variant::Type::String:
                valueStr = value.value<StringRef>().c_str();
                break;
            
            default:
                return;
        }
        config[key.c_str()] = std::move(valueStr);
    });
    q->configLoaded();
}

void ClientPrivate::handleConfigChange(const Expected<Dict>& dict)
{
    if (dict.isError()) {
        NCXMMS2_LOG_ERROR("%s", dict.error().c_str());
        return;
    }
    
    dict->forEach([this](StringRef key, Variant value){
        std::string valueStr;
        switch (value.type()) {
            case Variant::Type::Int:
                valueStr = std::to_string(value.value<int>());
                break;
                
            case Variant::Type::String:
                valueStr = value.value<StringRef>().c_str();
                break;
            
            default:
                return;
        }
        q->configValueChanged(key.c_str(), config[key.c_str()] = std::move(valueStr));
    });
}

void ClientPrivate::disconnectCallback(void *data)
{
    // NOTE: Potentially unsafe, as threre is no guarantee that Client is stil alive
    Client *q = static_cast<Client*>(data);
    q->disconnect();
    q->disconnected();
}

class StringListHelper
{
public:
    StringListHelper(const std::vector<std::string>& list)
    {
        m_list = xmmsv_new_list();
        for (const auto& str : list) {
            xmmsv_t *xmmsStr = xmmsv_new_string(str.c_str());
            xmmsv_list_append(m_list, xmmsStr);
            xmmsv_unref(xmmsStr);
        }
    }
    
    ~StringListHelper()
    {
        xmmsv_unref(m_list);
    }
    
    xmmsv_t *get() const {return m_list;}
    
    StringListHelper(const StringListHelper&) = delete;
    StringListHelper& operator=(const StringListHelper&) = delete;
    
private:
    xmmsv_t *m_list;
};

} // xmms2
} // ncxmms2

using namespace ncxmms2;

#define CLIENT_CHECK_CONNECTION                                                             \
    do {                                                                                    \
        if (!d->connected)                                                                  \
            throw ConnectionError("%s is called while Client is not connected!", __func__); \
    } while (0)

xmms2::Client::Client(Object *parent) :
    Object(parent),
    d(new ClientPrivate(this))
{
    
}

xmms2::Client::~Client()
{
    disconnect();
}

bool xmms2::Client::connect(const std::string& patch)
{
    disconnect();
    d->connection = xmmsc_init("ncxmms2");
    if (!d->connection) {
        NCXMMS2_LOG_ERROR("xmmsc_init failed");
        return false;
    }
    
    if (!xmmsc_connect(d->connection, !patch.empty() ? patch.c_str() : nullptr)) {
        NCXMMS2_LOG_ERROR("xmmsc_connect failed");
        xmmsc_unref(d->connection);
        return false;
    }
    d->connected = true;
    
    // Mainloop integration
    d->ml = xmmsc_mainloop_gmain_init(d->connection);
    
    xmmsc_disconnect_callback_set(d->connection, &ClientPrivate::disconnectCallback, this);
    
    //  Broadcasts and signals
    d->connectBroadcastOrSignal(xmmsc_signal_playback_playtime(d->connection), playbackPlaytimeChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playback_status(d->connection), playbackStatusChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playback_current_id(d->connection), playbackCurrentIdChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_medialib_entry_changed(d->connection), medialibEntryChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_medialib_entry_added(d->connection), medialibEntryAdded);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_loaded(d->connection), playlistLoaded);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_current_pos(d->connection), playlistCurrentPositionChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_changed(d->connection), playlistChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_collection_changed(d->connection), collectionChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_config_value_changed(d->connection), configValuesChanged);
    
    return true;
}

void xmms2::Client::disconnect()
{
    if (d->connected) {
        for (xmmsc_result_t *res : d->broadcastsAndSignals)
            xmmsc_result_disconnect(res);
        d->broadcastsAndSignals.clear();
       
        xmmsc_mainloop_gmain_shutdown(d->connection, d->ml);
        d->ml = nullptr;
        
        d->connected = false;
        xmmsc_unref(d->connection);
        d->connection = nullptr;
    }
}

xmms2::PlaybackStatusResult xmms2::Client::playbackStatus()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_status(d->connection)};
}

xmms2::IntResult xmms2::Client::playbackPlaytime()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_playtime(d->connection)};
}

xmms2::IntResult xmms2::Client::playbackCurrentId()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_current_id(d->connection)};
}

xmms2::VoidResult xmms2::Client::playbackStart()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_start(d->connection)};
}

xmms2::VoidResult xmms2::Client::playbackStop()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_stop(d->connection)};
}

xmms2::VoidResult xmms2::Client::playbackPause()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_pause(d->connection)};
}

xmms2::VoidResult xmms2::Client::playbackTickle()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playback_tickle(d->connection)};
}

xmms2::VoidResult xmms2::Client::playbackSeekMs(int ms)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection,
            xmmsc_playback_seek_ms(d->connection, ms, XMMS_PLAYBACK_SEEK_SET)};
}

xmms2::VoidResult xmms2::Client::playbackSeekMsRel(int ms)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection,
            xmmsc_playback_seek_ms(d->connection, ms, XMMS_PLAYBACK_SEEK_CUR)};
}

xmms2::StringResult xmms2::Client::playlistCurrentActive()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_current_active(d->connection)};
}

xmms2::VoidResult xmms2::Client::playlistLoad(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_load(d->connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistCreat(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_create(d->connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistRemove(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_remove(d->connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistClear(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_clear(d->connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistShuffle(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_shuffle(d->connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddId(const std::string& playlist, int id)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_add_id(d->connection, playlist.c_str(), id)};
}

xmms2::VoidResult xmms2::Client::playlistAddUrl(const std::string& playlist, const std::string& url)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_add_url(d->connection, playlist.c_str(), url.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddRecursive(const std::string& playlist, const std::string& path)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_radd(d->connection, playlist.c_str(), path.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddCollection(const std::string& playlist,
                                                       const xmms2::Collection& coll,
                                                       const std::vector<std::string>& order)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_add_collection(d->connection,
                                                         playlist.c_str(),
                                                         coll.m_coll,
                                                         !order.empty() ? StringListHelper(order).get() : nullptr)};
}

xmms2::VoidResult xmms2::Client::playlistAddIdList(const std::string& playlist, const xmms2::Collection& idList)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_add_idlist(d->connection, playlist.c_str(), idList.m_coll)};
}

void xmms2::Client::playlistAddPlaylistFile(const std::string& playlist, const std::string& file)
{
    CLIENT_CHECK_CONNECTION;
    collectionIdListFromPlaylistFile(file)([playlist, this](const Expected<Collection>& idlist){
        if (idlist.isValid())
            playlistAddIdList(playlist, idlist.value());
    });
}

xmms2::VoidResult xmms2::Client::playlistRemoveEntry(const std::string& playlist, int entry)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_remove_entry(d->connection, playlist.c_str(), entry)};
}

xmms2::VoidResult xmms2::Client::playlistMoveEntry(const std::string& playlist, int from, int to)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_move_entry(d->connection, playlist.c_str(), from, to)};
}

xmms2::VoidResult xmms2::Client::playlistSetNext(int item)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_set_next(d->connection, item)};
}

xmms2::VoidResult xmms2::Client::playlistSetNextRel(int next)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_set_next_rel(d->connection, next)};
}

xmms2::DictResult xmms2::Client::playlistCurrentPosition(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_current_pos(d->connection, playlist.c_str())};
}

xmms2::IntListResult xmms2::Client::playlistListEntries(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_list_entries(d->connection, playlist.c_str())};
}

xmms2::StringListResult xmms2::Client::playlistList()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_playlist_list(d->connection)};
}

xmms2::PropDictResult xmms2::Client::medialibGetInfo(int id)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_medialib_get_info(d->connection, id)};
}

xmms2::VoidResult xmms2::Client::collectionRename(const std::string& oldName, const std::string& newName,
                                                  const std::string& kind)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_coll_rename(d->connection, oldName.c_str(), newName.c_str(), kind.c_str())};
}

xmms2::CollectionResult xmms2::Client::collectionIdListFromPlaylistFile(const std::string& file)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_coll_idlist_from_playlist_file(d->connection, file.c_str())};
}

xmms2::DictListResult xmms2::Client::collectionQueryInfos(const xmms2::Collection& coll,
                                                          const std::vector<std::string>& fetch,
                                                          const std::vector<std::string>& order,
                                                          const std::vector<std::string>& groupBy,
                                                          int start, int limit)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_coll_query_infos(d->connection,
                                                  coll.m_coll,
                                                  !order.empty() ? StringListHelper(order).get() : nullptr,
                                                  start, limit,
                                                  !fetch.empty() ? StringListHelper(fetch).get() : nullptr,
                                                  !groupBy.empty() ? StringListHelper(groupBy).get() : nullptr)};
}

xmms2::DictResult xmms2::Client::configValueList()
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_config_list_values(d->connection)};
}

xmms2::VoidResult xmms2::Client::configSetValue(const std::string& key, const std::string& value)
{
    CLIENT_CHECK_CONNECTION;
    return {d->connection, xmmsc_config_set_value(d->connection, key.c_str(), value.c_str())};
}

void xmms2::Client::configLoad()
{
    if (!d->configLoadRequested) {
        configValueList()(&ClientPrivate::getConfig, d.get());
        configValuesChanged_Connect(&ClientPrivate::handleConfigChange, d.get());
        d->configLoadRequested = true;
    }
}

bool xmms2::Client::isConfigLoaded() const
{
    return !d->config.empty();
}

bool xmms2::Client::configHasValue(const std::string& key) const
{
    return d->config.find(key) != d->config.end();
}

const std::string& xmms2::Client::configValue(const std::string& key) const
{
    static const std::string empty;
    auto it = d->config.find(key);
    return it != d->config.end() ? it->second : empty;
}


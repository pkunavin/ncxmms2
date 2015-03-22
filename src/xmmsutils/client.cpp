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

#include <vector>
#include <unordered_map>
#include <xmmsclient/xmmsclient.h>
#include <xmmsclient/xmmsclient-glib.h>

#include "../lib/timer.h"

#include "client.h"
#include "../log.h"

namespace ncxmms2 {
namespace xmms2 {

class ClientPrivate
{
public:
    ClientPrivate(Client *_q) :
        q(_q),
        m_connection(nullptr),
        m_connected(false),
        m_ml(nullptr),
        m_configLoadRequested(false),
        m_playbackStatus(PlaybackStatus::Stopped),
        m_requestedToPlayId(-1)
         {}
     
    Client *q;
    xmmsc_connection_t *m_connection;
    bool m_connected;
    void *m_ml;

    std::vector<xmmsc_result_t*> m_broadcastsAndSignals;

    template <typename T>
    void connectBroadcastOrSignal(xmmsc_result_t *result, Signals::Signal<T>& signal)
    {
        auto *callback = new XmmsValueFunctionWrapper<T>(std::ref(signal));
        xmmsc_result_notifier_set_full(result, callback->get(), callback,
                                       XmmsValueFunctionWrapper<T>::free);
        xmmsc_result_unref(result);
        m_broadcastsAndSignals.push_back(result);
    }
    
    bool m_configLoadRequested;
    std::unordered_map<std::string, std::string> m_config;
    
    void getConfig(const Expected<Dict>& dict);
    void handleConfigChange(const Expected<Dict>& dict);
    
    static void disconnectCallback(void *data);
    
    PlaybackStatus m_playbackStatus;
    void getPlaybackStatus(const xmms2::Expected<xmms2::PlaybackStatus>& status);
    
    std::string m_activePlaylist;
    void getActivePlaylist(const xmms2::Expected<StringRef>& playlist);
    
    int m_requestedToPlayId;
    std::string m_requestedToPlayIdPlaylist;
    Timer m_resetRequestedToPlayIdTimer;
    enum {RequestedToPlayIdTimeout = 5};
    
    void resetRequestedToPlayId();
    void onPlaylistChange(const xmms2::PlaylistChangeEvent& change);
};

void ClientPrivate::getConfig(const Expected<Dict>& dict)
{
    if (dict.isError()) {
        NCXMMS2_LOG_ERROR("%s", dict.error());
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
        m_config[key.c_str()] = std::move(valueStr);
    });
    q->configLoaded();
}

void ClientPrivate::handleConfigChange(const Expected<Dict>& dict)
{
    if (dict.isError()) {
        NCXMMS2_LOG_ERROR("%s", dict.error());
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
        q->configValueChanged(key.c_str(), m_config[key.c_str()] = std::move(valueStr));
    });
}

void ClientPrivate::disconnectCallback(void *data)
{
    // NOTE: Potentially unsafe, as threre is no guarantee that Client is stil alive
    Client *q = static_cast<Client*>(data);
    q->disconnect();
    q->disconnected();
}

void ClientPrivate::getPlaybackStatus(const xmms2::Expected<PlaybackStatus>& status)
{
    if (status.isError()) {
        NCXMMS2_LOG_ERROR("%s", status.error());
        return;
    }
    
    m_playbackStatus = status.value();
}

void ClientPrivate::getActivePlaylist(const xmms2::Expected<StringRef>& playlist)
{
    if (playlist.isError()) {
        NCXMMS2_LOG_ERROR("%s", playlist.error());
        return;
    }
    
    m_activePlaylist = playlist->c_str();
}

void ClientPrivate::resetRequestedToPlayId()
{
    m_resetRequestedToPlayIdTimer.stop();
    m_requestedToPlayId = -1;
    m_requestedToPlayIdPlaylist.clear();
}

void ClientPrivate::onPlaylistChange(const PlaylistChangeEvent& change)
{
    if (   change.type() == PlaylistChangeEvent::Type::Add 
        && change.id() == m_requestedToPlayId && change.playlist() == m_requestedToPlayIdPlaylist) {
        q->playlistPlayItem(change.playlist(), change.position());
        resetRequestedToPlayId();
    }
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
        if (!d->m_connected)                                                                \
            throw ConnectionError("%s is called while Client is not connected!", __func__); \
    } while (0)

xmms2::Client::Client(Object *parent) :
    Object(parent),
    d(new ClientPrivate(this))
{
    playbackStatusChanged_Connect(&ClientPrivate::getPlaybackStatus, d.get());
    playlistLoaded_Connect(&ClientPrivate::getActivePlaylist, d.get());
    playlistChanged_Connect(&ClientPrivate::onPlaylistChange, d.get());
    d->m_resetRequestedToPlayIdTimer.timeout_Connect(&ClientPrivate::resetRequestedToPlayId, d.get());
}

xmms2::Client::~Client()
{
    disconnect();
}

bool xmms2::Client::connect(const std::string& patch)
{
    disconnect();
    d->m_connection = xmmsc_init("ncxmms2");
    if (!d->m_connection) {
        NCXMMS2_LOG_ERROR("xmmsc_init failed");
        return false;
    }
    
    if (!xmmsc_connect(d->m_connection, !patch.empty() ? patch.c_str() : nullptr)) {
        NCXMMS2_LOG_ERROR("xmmsc_connect failed");
        xmmsc_unref(d->m_connection);
        return false;
    }
    d->m_connected = true;
    
    // Mainloop integration
    d->m_ml = xmmsc_mainloop_gmain_init(d->m_connection);
    
    xmmsc_disconnect_callback_set(d->m_connection, &ClientPrivate::disconnectCallback, this);
    
    //  Broadcasts and signals
    d->connectBroadcastOrSignal(xmmsc_signal_playback_playtime(d->m_connection), playbackPlaytimeChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playback_status(d->m_connection), playbackStatusChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playback_current_id(d->m_connection), playbackCurrentIdChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_medialib_entry_changed(d->m_connection), medialibEntryChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_medialib_entry_added(d->m_connection), medialibEntryAdded);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_loaded(d->m_connection), playlistLoaded);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_current_pos(d->m_connection), playlistCurrentPositionChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_playlist_changed(d->m_connection), playlistChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_collection_changed(d->m_connection), collectionChanged);
    d->connectBroadcastOrSignal(xmmsc_broadcast_config_value_changed(d->m_connection), configValuesChanged);
    
    playbackGetStatus()(&ClientPrivate::getPlaybackStatus, d.get());
    playlistGetCurrentActive()(&ClientPrivate::getActivePlaylist, d.get());
    
    return true;
}

void xmms2::Client::disconnect()
{
    d->resetRequestedToPlayId();
    if (d->m_connected) {
        for (xmmsc_result_t *res : d->m_broadcastsAndSignals) {
            xmmsc_result_disconnect(res);
        }
        d->m_broadcastsAndSignals.clear();
       
        xmmsc_mainloop_gmain_shutdown(d->m_connection, d->m_ml);
        d->m_ml = nullptr;
        
        d->m_connected = false;
        xmmsc_unref(d->m_connection);
        d->m_connection = nullptr;
    }
}

xmms2::PlaybackStatusResult xmms2::Client::playbackGetStatus()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_status(d->m_connection)};
}

xmms2::IntResult xmms2::Client::playbackGetPlaytime()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_playtime(d->m_connection)};
}

xmms2::IntResult xmms2::Client::playbackGetCurrentId()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_current_id(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playbackStart()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_start(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playbackStop()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_stop(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playbackPause()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_pause(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playbackTickle()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playback_tickle(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playbackSeekMs(int ms)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection,
            xmmsc_playback_seek_ms(d->m_connection, ms, XMMS_PLAYBACK_SEEK_SET)};
}

xmms2::VoidResult xmms2::Client::playbackSeekMsRel(int ms)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection,
            xmmsc_playback_seek_ms(d->m_connection, ms, XMMS_PLAYBACK_SEEK_CUR)};
}

xmms2::StringResult xmms2::Client::playlistGetCurrentActive()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_current_active(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::playlistLoad(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_load(d->m_connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistCreat(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_create(d->m_connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistRemove(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_remove(d->m_connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistClear(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_clear(d->m_connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistShuffle(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_shuffle(d->m_connection, playlist.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddId(const std::string& playlist, int id)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_add_id(d->m_connection, playlist.c_str(), id)};
}

xmms2::VoidResult xmms2::Client::playlistAddUrl(const std::string& playlist, const std::string& url)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_add_url(d->m_connection, playlist.c_str(), url.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddRecursive(const std::string& playlist, const std::string& path)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_radd(d->m_connection, playlist.c_str(), path.c_str())};
}

xmms2::VoidResult xmms2::Client::playlistAddCollection(const std::string& playlist,
                                                       const xmms2::Collection& coll,
                                                       const std::vector<std::string>& order)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_add_collection(d->m_connection,
                                                         playlist.c_str(),
                                                         coll.m_coll,
                                                         !order.empty() ? StringListHelper(order).get() : nullptr)};
}

xmms2::VoidResult xmms2::Client::playlistAddIdList(const std::string& playlist, const xmms2::Collection& idList)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_add_idlist(d->m_connection, playlist.c_str(), idList.m_coll)};
}

void xmms2::Client::playlistAddPlaylistFile(const std::string& playlist, const std::string& file)
{
    CLIENT_CHECK_CONNECTION;
    collectionGetIdListFromPlaylistFile(file)([playlist, this](const Expected<Collection>& idlist){
        if (idlist.isValid())
            playlistAddIdList(playlist, idlist.value());
    });
}

xmms2::VoidResult xmms2::Client::playlistRemoveEntry(const std::string& playlist, int entry)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_remove_entry(d->m_connection, playlist.c_str(), entry)};
}

xmms2::VoidResult xmms2::Client::playlistMoveEntry(const std::string& playlist, int from, int to)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_move_entry(d->m_connection, playlist.c_str(), from, to)};
}

xmms2::VoidResult xmms2::Client::playlistSetNext(int item)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_set_next(d->m_connection, item)};
}

xmms2::VoidResult xmms2::Client::playlistSetNextRel(int next)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_set_next_rel(d->m_connection, next)};
}

xmms2::DictResult xmms2::Client::playlistGetCurrentPosition(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_current_pos(d->m_connection, playlist.c_str())};
}

xmms2::IntListResult xmms2::Client::playlistGetEntries(const std::string& playlist)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_list_entries(d->m_connection, playlist.c_str())};
}

xmms2::StringListResult xmms2::Client::playlistGetList()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_playlist_list(d->m_connection)};
}

void xmms2::Client::playlistPlayItem(const std::string& playlist, int item)
{
    CLIENT_CHECK_CONNECTION;
    
    if (playlist != d->m_activePlaylist)
        playlistLoad(playlist);    
    
    playlistSetNext(item);
    playbackTickle();
    switch (d->m_playbackStatus) {
        case PlaybackStatus::Stopped:
            playbackStart();
            break;
            
        case PlaybackStatus::Playing:
            playbackTickle();
            break;
            
        case PlaybackStatus::Paused:
            playbackStart();
            playbackTickle();
            break;
    }
}

void xmms2::Client::playlistPlayId(const std::string& playlist, int id)
{
    CLIENT_CHECK_CONNECTION;
    d->m_requestedToPlayId = id;
    d->m_requestedToPlayIdPlaylist = playlist;
    d->m_resetRequestedToPlayIdTimer.start(ClientPrivate::RequestedToPlayIdTimeout);
    playlistAddId(playlist, id);
}

const std::string& xmms2::Client::playlistCurrentActive() const
{
    return d->m_activePlaylist;
}

xmms2::PropDictResult xmms2::Client::medialibGetInfo(int id)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_medialib_get_info(d->m_connection, id)};
}

xmms2::IntResult xmms2::Client::medialibGetId(const std::string& url)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_medialib_get_id(d->m_connection, url.c_str())};
}

xmms2::VoidResult xmms2::Client::medialibAddEntry(const std::string& url)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_medialib_add_entry(d->m_connection, url.c_str())};
}

xmms2::VoidResult xmms2::Client::collectionRename(const std::string& oldName, const std::string& newName,
                                                  const std::string& kind)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_coll_rename(d->m_connection, oldName.c_str(), newName.c_str(), kind.c_str())};
}

xmms2::CollectionResult xmms2::Client::collectionGetIdListFromPlaylistFile(const std::string& file)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_coll_idlist_from_playlist_file(d->m_connection, file.c_str())};
}

xmms2::DictListResult xmms2::Client::collectionQueryInfos(const xmms2::Collection& coll,
                                                          const std::vector<std::string>& fetch,
                                                          const std::vector<std::string>& order,
                                                          const std::vector<std::string>& groupBy,
                                                          int start, int limit)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_coll_query_infos(d->m_connection,
                                                  coll.m_coll,
                                                  !order.empty() ? StringListHelper(order).get() : nullptr,
                                                  start, limit,
                                                  !fetch.empty() ? StringListHelper(fetch).get() : nullptr,
                                                  !groupBy.empty() ? StringListHelper(groupBy).get() : nullptr)};
}

xmms2::DictListResult xmms2::Client::xformMediaBrowse(const std::string& url)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_xform_media_browse(d->m_connection, url.c_str())};
}

xmms2::DictResult xmms2::Client::configGetValueList()
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_config_list_values(d->m_connection)};
}

xmms2::VoidResult xmms2::Client::configSetValue(const std::string& key, const std::string& value)
{
    CLIENT_CHECK_CONNECTION;
    return {d->m_connection, xmmsc_config_set_value(d->m_connection, key.c_str(), value.c_str())};
}

void xmms2::Client::configLoad()
{
    if (!d->m_configLoadRequested) {
        configGetValueList()(&ClientPrivate::getConfig, d.get());
        configValuesChanged_Connect(&ClientPrivate::handleConfigChange, d.get());
        d->m_configLoadRequested = true;
    }
}

bool xmms2::Client::isConfigLoaded() const
{
    return !d->m_config.empty();
}

bool xmms2::Client::configHasValue(const std::string& key) const
{
    return d->m_config.find(key) != d->m_config.end();
}

const std::string& xmms2::Client::configValue(const std::string& key) const
{
    static const std::string empty;
    auto it = d->m_config.find(key);
    return it != d->m_config.end() ? it->second : empty;
}


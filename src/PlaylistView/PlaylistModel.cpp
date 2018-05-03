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

#include "PlaylistModel.h"
#include "../XmmsUtils/Client.h"
#include "../Log.h"

#include "../lib/ListModelItemData.h"

using namespace ncxmms2;

PlaylistModel::PlaylistModel(xmms2::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient),
    m_lazyLoadPlaylist(false),
    m_currentPosition(-1),
    m_totalDuration(0)
{
    m_xmmsClient->playlistChanged_Connect(&PlaylistModel::processPlaylistChange, this);
    m_xmmsClient->playlistCurrentPositionChanged_Connect(&PlaylistModel::getCurrentPosition, this);
    m_xmmsClient->collectionChanged_Connect(&PlaylistModel::handlePlaylistRename, this);
    m_xmmsClient->medialibEntryChanged_Connect(&PlaylistModel::handleSongInfoUpdate, this);
}

void PlaylistModel::setPlaylist(const std::string& playlist)
{
    m_playlist = playlist;
    m_currentPosition = -1;
    m_xmmsClient->playlistGetEntries(m_playlist)(&PlaylistModel::getEntries, this);
}

const std::string& PlaylistModel::playlist() const
{
    return m_playlist;
}

void PlaylistModel::getEntries(const xmms2::Expected<xmms2::List<int>>& entries)
{
    m_totalDuration = 0;
    m_idList.clear();
    m_songInfos.clear();
    
    if (entries.isError()) {
        NCXMMS2_LOG_ERROR("%s", entries.error());
        // Empty playlist name indicates that model data is not valid,
        // we can't recover from this error
        m_playlist.clear();
        reset();
        totalDurationChanged();
        return;
    }
    
    m_idList.reserve(entries->size());
    m_songInfos.rehash(m_idList.size() / m_songInfos.max_load_factor() + 1);

    for (auto it = entries->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        int id = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok)) {
            m_idList.clear();
            m_songInfos.clear();
            break;
        }
        m_idList.push_back(id);
    }
    
    if (!m_idList.empty())
        m_xmmsClient->playlistGetCurrentPosition(m_playlist)(&PlaylistModel::getCurrentPosition, this);
    
    if (!m_lazyLoadPlaylist) {
        for (int i = 0; i < (int)m_idList.size(); ++i) {
            int id = m_idList[i];
            m_songInfos[id];
            m_xmmsClient->medialibGetInfo(id)(&PlaylistModel::getSongInfo, this,
                                              i, std::placeholders::_1);
        }
    }
    
    reset();
    totalDurationChanged();
}

void PlaylistModel::getSongInfo(int position, const xmms2::Expected<xmms2::PropDict>& info)
{
    if (info.isError()) {
        NCXMMS2_LOG_ERROR("%s", info.error());
        return;
    }
    
    const int id = info->value<int>("id");
    auto it = m_songInfos.find(id);
    if (it == m_songInfos.end())
        return;

    Song *song = &(*it).second;
    int durationDiff = song->duration() > 0 ? -song->duration() : 0;
    song->loadInfo(*info);

    if (position == -1
        || (std::vector<int>::size_type)position >= m_idList.size()
        || m_idList[position] != id) {
        itemsChanged(0, m_idList.size() - 1);
    } else {
        itemsChanged(position, position);
    }

    durationDiff += song->duration() > 0 ? song->duration() : 0;
    if (durationDiff) {
        m_totalDuration += durationDiff;
        totalDurationChanged();
    }
}

void PlaylistModel::processPlaylistChange(const xmms2::PlaylistChangeEvent& change)
{
    if (m_playlist.empty() || change.playlist() != m_playlist)
        return;

    typedef xmms2::PlaylistChangeEvent::Type ChangeType;
    switch (change.type()) {
        case ChangeType::Add:
        {
            const int id = change.id();
            m_idList.push_back(id);
            m_songInfos[id];
            m_xmmsClient->medialibGetInfo(id)(&PlaylistModel::getSongInfo, this,
                                              m_idList.size() - 1, std::placeholders::_1);
            itemAdded();
            totalDurationChanged();
            break;
        }

        case ChangeType::Insert:
        {
            const int id = change.id();
            const int position = change.position();
            if (position < 0 || (size_t)position > m_idList.size()) {
                NCXMMS2_LOG_ERROR("Wrong insert position: %d, playlist size: %zu", position, m_idList.size());
                return;
            }
            m_idList.insert(m_idList.begin() + position, id);
            m_songInfos[id];
            m_xmmsClient->medialibGetInfo(id)(&PlaylistModel::getSongInfo, this,
                                              position, std::placeholders::_1);
            itemInserted(position);
            totalDurationChanged();
            break;
        }

        case ChangeType::Remove:
        {
            const int position = change.position();
            if (position < 0 || (size_t)position >= m_idList.size()) {
                NCXMMS2_LOG_ERROR("Wrong insert position: %d, playlist size: %zu", position, m_idList.size());
                return;
            }
            const int id = m_idList[position];
            m_idList.erase(m_idList.begin() + position);
            m_totalDuration -= m_songInfos[id].duration();
            m_songInfos.erase(id);

            itemRemoved(position);
            totalDurationChanged();
            break;
        }

        case ChangeType::Move:
        {
            const int position = change.position();
            const int newPosition = change.newPosition();
            if (position < 0 || (size_t)position >= m_idList.size()) {
                NCXMMS2_LOG_ERROR("Wrong insert position: %d, playlist size: %zu", position, m_idList.size());
                return;
            }
            if (newPosition < 0 || (size_t)newPosition >= m_idList.size()) {
                NCXMMS2_LOG_ERROR("Wrong insert position: %d, playlist size: %zu", newPosition, m_idList.size());
                return;
            }
            const int id = m_idList[position];
            m_idList.erase(m_idList.begin() + position);
            m_idList.insert(m_idList.begin() + newPosition, id);
            itemMoved(position, newPosition);
            break;
        }

        case ChangeType::Replace:
            m_xmmsClient->playlistGetEntries(m_playlist)(&PlaylistModel::getEntries, this);
            break;

        default:
            break;
    }
}

void PlaylistModel::getCurrentPosition(const xmms2::Expected<xmms2::Dict>& position)
{
    if (position.isError()) {
        NCXMMS2_LOG_ERROR("%s", position.error());
        return;
    }
    
    if (m_playlist.empty())
        return;
    
    if (m_playlist != position->value<StringRef>("name", "").c_str())
        return;

    const int newPosition = position->value<int>("position", -1);
    if (newPosition != -1) {
        const int oldPosition = m_currentPosition;
        m_currentPosition = newPosition;
        itemsChanged(oldPosition, oldPosition);
        itemsChanged(m_currentPosition, m_currentPosition);
        if (oldPosition != -1)
            activeSongPositionChanged(m_currentPosition);
    }
}

void PlaylistModel::handlePlaylistRename(const xmms2::CollectionChangeEvent& change)
{
    if (m_playlist.empty())
        return;
    
    typedef xmms2::CollectionChangeEvent::Type ChangeType;
    if (change.type() == ChangeType::Rename && change.kind() == "Playlists") {
        if (change.name() == m_playlist) {
            m_playlist = change.newName();
            playlistRenamed();
        }
    }
}

void PlaylistModel::handleSongInfoUpdate(const xmms2::Expected<int>& id)
{
    if (id.isError()) {
        NCXMMS2_LOG_ERROR("%s", id.error());
        return;
    }
    
    if (m_songInfos.find(*id) != m_songInfos.end()) {
        m_xmmsClient->medialibGetInfo(*id)(&PlaylistModel::getSongInfo, this, -1, std::placeholders::_1);
    }
}

int PlaylistModel::itemsCount() const
{
    return m_idList.size();
}

const Song &PlaylistModel::song(int item) const
{
    assert(item >= 0 && (size_t)item < m_idList.size());

    const Song *song = nullptr;
    const int id = m_idList[item];
    auto it = m_songInfos.find(id);
    if (it == m_songInfos.end()) {
        PlaylistModel *nonConstThis = const_cast<PlaylistModel*>(this);
        song = &nonConstThis->m_songInfos[id];
        m_xmmsClient->medialibGetInfo(id)(&PlaylistModel::getSongInfo, nonConstThis,
                                          -1, std::placeholders::_1);
    } else {
        song = &(*it).second;
    }

    return *song;
}

int PlaylistModel::currentSongItem() const
{
    return m_currentPosition;
}

int PlaylistModel::totalDuration() const
{
    return m_totalDuration;
}

void PlaylistModel::setLazyLoadPlaylist(bool enable)
{
    m_lazyLoadPlaylist = enable;
}

void PlaylistModel::data(int item, ListModelItemData *itemData) const
{
    // Actually, this is never used, PlaylistItemDelegate uses song method instead.
    const Song& s = song(item);

    if (s.id() > 0) {
        itemData->text.clear();
        itemData->text.append(s.artist()).append(" - ").append(s.title());
    } else {
        itemData->text = "Loading...";
    }
}

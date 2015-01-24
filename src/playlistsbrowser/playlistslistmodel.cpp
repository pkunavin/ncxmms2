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

#include <algorithm>
#include <assert.h>

#include "playlistslistmodel.h"
#include "../xmmsutils/client.h"
#include "../log.h"

#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

PlaylistsListModel::PlaylistsListModel(xmms2::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    
}

void PlaylistsListModel::data(int item, ListModelItemData *itemData) const
{
    assert(item >= 0 && (size_t)item < m_playlists.size());
    itemData->textPtr = &m_playlists[item];
    itemData->bold = m_playlists[item] == m_currentPlaylist;
}

int PlaylistsListModel::itemsCount() const
{
    return m_playlists.size();
}

void PlaylistsListModel::init()
{
    m_xmmsClient->playlistList()(&PlaylistsListModel::getPlaylists, this);
    m_xmmsClient->playlistCurrentActive()(&PlaylistsListModel::getCurrentPlaylist, this);
    m_xmmsClient->playlistLoaded_Connect(&PlaylistsListModel::getCurrentPlaylist, this);
    m_xmmsClient->collectionChanged_Connect(&PlaylistsListModel::handlePlaylistsChange, this);
}

const std::string& PlaylistsListModel::playlist(int item) const
{
    assert(item >= 0 && (size_t)item < m_playlists.size());
    return m_playlists[item];
}

bool PlaylistsListModel::playlistExists(const std::string& playlist) const
{
    return std::find(m_playlists.begin(), m_playlists.end(), playlist)
            != m_playlists.end();
}

int PlaylistsListModel::indexOf(const std::string& playlist) const
{
    auto it = std::find(m_playlists.begin(), m_playlists.end(), playlist);
    return it != m_playlists.end() ? it - m_playlists.begin() : -1;
}

const std::string& PlaylistsListModel::currentPlaylist() const
{
    return m_currentPlaylist;
}

void PlaylistsListModel::getPlaylists(const xmms2::Expected<xmms2::List<StringRef>>& playlists)
{
    if (playlists.isError()) {
        NCXMMS2_LOG_ERROR("%s", playlists.error().toString().c_str());
        return;
    }
    
    m_playlists.clear();
    m_playlists.reserve(playlists->size());
    
    for (auto it = playlists->getIterator(); it.isValid(); it.next()) {
        bool ok;
        StringRef strRef = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok || strRef.isNull()))
            continue;
        if (NCXMMS2_UNLIKELY(*strRef.c_str() == '\0' || *strRef.c_str() == '_'))
            continue;
        m_playlists.emplace_back(strRef.c_str());
    }
    reset();
}

void PlaylistsListModel::getCurrentPlaylist(const xmms2::Expected<StringRef>& playlist)
{
    if (playlist.isError()) {
        NCXMMS2_LOG_ERROR("%s", playlist.error().toString().c_str());
        return;
    }
    
    auto it = std::find(m_playlists.begin(), m_playlists.end(), m_currentPlaylist);
    m_currentPlaylist = playlist->c_str();
    if (it != m_playlists.end())
        itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());
    
    it = std::find(m_playlists.begin(), m_playlists.end(), m_currentPlaylist);
    if (it != m_playlists.end())
        itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());
}

void PlaylistsListModel::handlePlaylistsChange(const xmms2::CollectionChangeEvent& change)
{
    if (change.kind() != "Playlists")
        return;

    typedef xmms2::CollectionChangeEvent::Type ChangeType;
    switch (change.type()) {
        case ChangeType::Add:
            m_playlists.push_back(change.name());
            itemAdded();
            playlistAdded(m_playlists.back(), m_playlists.size() - 1);
            break;

        case ChangeType::Rename:
        {
            if (m_currentPlaylist == change.name())
                m_currentPlaylist = change.newName();

            auto it = std::find(m_playlists.begin(), m_playlists.end(), change.name());
            if (it != m_playlists.end()) {
                (*it) = change.newName();
                itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());
            }
            break;
        }

        case ChangeType::Remove:
        {
            auto it = std::find(m_playlists.begin(), m_playlists.end(), change.name());
            if (it != m_playlists.end()) {
                m_playlists.erase(it);
                itemRemoved(it - m_playlists.begin());
            }
            break;
        }

        case ChangeType::Update:
            break;
    }
}

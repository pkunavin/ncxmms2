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

#include <xmmsclient/xmmsclient++.h>
#include <algorithm>
#include <assert.h>

#include "playlistslistmodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

PlaylistsListModel::PlaylistsListModel(Xmms::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient)
{
    m_xmmsClient->playlist.list()(
        Xmms::bind(&PlaylistsListModel::getPlaylists, this)
    );
    m_xmmsClient->playlist.currentActive()(
        Xmms::bind(&PlaylistsListModel::getCurrentPlaylist, this)
    );
    m_xmmsClient->playlist.broadcastLoaded()(
        Xmms::bind(&PlaylistsListModel::getCurrentPlaylist, this)
    );
    m_xmmsClient->collection.broadcastCollectionChanged()(
        Xmms::bind(&PlaylistsListModel::handlePlaylistsChange, this)
    );
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

const std::string &PlaylistsListModel::currentPlaylist() const
{
    return m_currentPlaylist;
}

bool PlaylistsListModel::getPlaylists(const Xmms::List<std::string>& playlists)
{
    m_playlists.clear();
    for (auto& playlist : playlists)
    {
        if (playlist.empty() || playlist[0] == '_')
            continue;

        m_playlists.push_back(playlist);
    }

    reset();
    return true;
}

bool PlaylistsListModel::getCurrentPlaylist(const std::string& playlist)
{
    auto it = std::find(m_playlists.begin(), m_playlists.end(), m_currentPlaylist);
    m_currentPlaylist = playlist;
    if (it != m_playlists.end())
        itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());

    it = std::find(m_playlists.begin(), m_playlists.end(), m_currentPlaylist);
    if (it != m_playlists.end())
        itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());

    return true;
}

bool PlaylistsListModel::handlePlaylistsChange(const Xmms::Dict& change)
{
    if (change.get<std::string>("namespace") != XMMS_COLLECTION_NS_PLAYLISTS)
        return true;

    switch (change.get<int>("type")) {
        case XMMS_COLLECTION_CHANGED_ADD:
            m_playlists.push_back(change.get<std::string>("name"));
            itemAdded();
            playlistAdded(m_playlists.back(), m_playlists.size() - 1);
            break;

        case XMMS_COLLECTION_CHANGED_RENAME:
        {
            const std::string name = change.get<std::string>("name");
            const std::string newName = change.get<std::string>("newname");

            if (m_currentPlaylist == name)
                m_currentPlaylist = newName;

            auto it = std::find(m_playlists.begin(), m_playlists.end(), name);
            if (it != m_playlists.end()) {
                (*it) = newName;
                itemsChanged(it - m_playlists.begin(), it - m_playlists.begin());
            }

            break;
        }

        case XMMS_COLLECTION_CHANGED_REMOVE:
        {
            auto it = std::find(m_playlists.begin(), m_playlists.end(),
                                change.get<std::string>("name"));
            if (it != m_playlists.end()) {
                m_playlists.erase(it);
                itemRemoved(it - m_playlists.begin());
            }

            break;
        }

        case XMMS_COLLECTION_CHANGED_UPDATE:
            break;
    }

    return true;
}

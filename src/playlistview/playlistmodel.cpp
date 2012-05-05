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
#include "playlistmodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

PlaylistModel::PlaylistModel(Xmms::Client *xmmsClient, Object *parent) :
    ListModel(parent),
    m_xmmsClient(xmmsClient),
    m_currentPosition(-1),
    m_totalDuration(0)
{
    m_xmmsClient->playlist.broadcastChanged()(
        Xmms::bind(&PlaylistModel::processPlaylistChange, this)
    );
    m_xmmsClient->playlist.broadcastCurrentPos()(
        Xmms::bind(&PlaylistModel::getCurrentPosition, this)
    );
    m_xmmsClient->collection.broadcastCollectionChanged()(
        Xmms::bind(&PlaylistModel::handlePlaylistRename, this)
    );
    m_xmmsClient->medialib.broadcastEntryChanged()(
        Xmms::bind(&PlaylistModel::handleSongInfoUpdate, this)
    );
}

void PlaylistModel::setPlaylist(const std::string& playlist)
{
    m_playlist = playlist;
    m_currentPosition = -1;
    m_xmmsClient->playlist.listEntries(m_playlist)(
        Xmms::bind(&PlaylistModel::getEntries, this)
    );
    m_xmmsClient->playlist.currentPos(m_playlist)(
        Xmms::bind(&PlaylistModel::getCurrentPosition, this)
    );
}

const std::string& PlaylistModel::playlist() const
{
    return m_playlist;
}

bool PlaylistModel::getEntries(const Xmms::List<int>& list)
{
    m_totalDuration = 0;
    m_idList.clear();
    m_songInfos.clear();
    m_idList.reserve(200); // TODO: use Xmms::List::size
    m_songInfos.rehash(200 / m_songInfos.max_load_factor() + 1);

    int pos = 0;
    for(auto i(list.begin()), i_end(list.end()); i != i_end; ++i, ++pos) {
        const int id = *i;
        m_idList.push_back(id);
        m_songInfos[id];
        m_xmmsClient->medialib.getInfo(id)(
            boost::bind(&PlaylistModel::getSongInfo, this, pos, _1)
        );
    }
    reset();
    totalDurationChanged();
    return true;
}

bool PlaylistModel::getSongInfo(int position, const Xmms::PropDict& info)
{
    const int id = info.get<int>("id");
    auto it = m_songInfos.find(id);
    if (it == m_songInfos.end())
        return true;

    Song *song = &(*it).second;
    int durationDiff = -song->duration();
    song->loadInfo(info);

    if (position == -1
        || (std::vector<int>::size_type)position >= m_idList.size()
        || m_idList[position] != id) {
        itemsChanged(0, m_idList.size() - 1);//redrawAll();
    } else {
        itemsChanged(position, position);//redrawItem(position);
    }

    durationDiff += song->duration();
    if (durationDiff) {
        m_totalDuration += durationDiff;
        totalDurationChanged();
    }

    return true;
}

bool PlaylistModel::processPlaylistChange(const Xmms::Dict& change)
{
    if (change.get<std::string>("name") != m_playlist)
        return true;

    switch (change.get<int>("type")) {
        case XMMS_PLAYLIST_CHANGED_ADD:
        {
            const int id = change.get<int>("id");

            m_idList.push_back(id);
            m_songInfos[id];
            m_xmmsClient->medialib.getInfo(id)(
                boost::bind(&PlaylistModel::getSongInfo, this, m_idList.size() - 1, _1)
            );
            itemAdded();
            totalDurationChanged();
            break;
        }

        case XMMS_PLAYLIST_CHANGED_INSERT:
        {
            const int id = change.get<int>("id");
            const int position = change.get<int>("position");

            m_idList.insert(m_idList.begin() + position, id);
            m_songInfos[id];
            m_xmmsClient->medialib.getInfo(id)(
                boost::bind(&PlaylistModel::getSongInfo, this, position, _1)
            );
            itemInserted(position);
            totalDurationChanged();
            break;
        }

        case XMMS_PLAYLIST_CHANGED_REMOVE:
        {
            const int position = change.get<int>("position");
            const int id = m_idList[position];

            m_idList.erase(m_idList.begin() + position);
            m_totalDuration -= m_songInfos[id].duration();
            m_songInfos.erase(id);

            itemRemoved(position);
            totalDurationChanged();
            break;
        }

        case XMMS_PLAYLIST_CHANGED_MOVE:
        {
            const int position = change.get<int>("position");
            const int newPosition = change.get<int>("newposition");

            const int id = m_idList[position];
            m_idList.erase(m_idList.begin() + position);
            m_idList.insert(m_idList.begin() + newPosition, id);
            itemMoved(position, newPosition);
            break;
        }

        case XMMS_PLAYLIST_CHANGED_CLEAR:
            m_currentPosition = -1;
            m_totalDuration = 0;
            m_idList.clear();
            m_songInfos.clear();
            totalDurationChanged();
            reset();
            break;

        case XMMS_PLAYLIST_CHANGED_SHUFFLE:
        case XMMS_PLAYLIST_CHANGED_SORT:
            m_xmmsClient->playlist.listEntries(m_playlist)(
                Xmms::bind(&PlaylistModel::getEntries, this)
            );
            break;

        case XMMS_PLAYLIST_CHANGED_UPDATE: // Don't know how I should handle it, just ignore
            break;
    }

    return true;
}

bool PlaylistModel::getCurrentPosition(const Xmms::Dict &position)
{
    if (!position.contains("name") || position.get<std::string>("name") != m_playlist)
        return true;

    if (position.contains("position")) {
        const int oldPosition = m_currentPosition;
        m_currentPosition = position.get<int>("position");
        itemsChanged(oldPosition, oldPosition);
        itemsChanged(m_currentPosition, m_currentPosition);
        if (oldPosition != -1)
            activeSongPositionChanged(m_currentPosition);
    }
    return true;
}

bool PlaylistModel::handlePlaylistRename(const Xmms::Dict& change)
{
    if (change.get<int>("type") == XMMS_COLLECTION_CHANGED_RENAME
        && change.get<std::string>("namespace") == XMMS_COLLECTION_NS_PLAYLISTS) {
        const std::string name = change.get<std::string>("name");
        if (name == m_playlist) {
            m_playlist = change.get<std::string>("newname");
            playlistRenamed();
        }
    }
    return true;
}

bool PlaylistModel::handleSongInfoUpdate(const int& id)
{
    if (m_songInfos.find(id) != m_songInfos.end()) {
        m_xmmsClient->medialib.getInfo(id)(
            boost::bind(&PlaylistModel::getSongInfo, this, -1, _1)
        );
    }

    return true;
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
        m_xmmsClient->medialib.getInfo(id)(
            boost::bind(&PlaylistModel::getSongInfo, nonConstThis, -1, _1)
        );
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

void PlaylistModel::data(int item, ListModelItemData *itemData) const
{
    // Actually, this is never used, PlaylistItemDelegate uses song method instead.
    const Song& s = song(item);

    if (s.id()) {
        itemData->text.append(s.artist()).append(" - ").append(s.title());
    } else {
        itemData->text = "Loading...";
    }
}

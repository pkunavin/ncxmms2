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

#include "PlaylistsListView.h"
#include "PlaylistsListModel.h"
#include "../XmmsUtils/Client.h"
#include "../StatusArea/StatusArea.h"
#include "../Hotkeys.h"

#include "../lib/KeyEvent.h"

using namespace ncxmms2;

PlaylistsListView::PlaylistsListView(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListViewAppIntegrated(rect, parent),
    m_xmmsClient(xmmsClient),
    m_initialized(false)
{
    loadPalette("PlaylistsListView");

    PlaylistsListModel *model = new PlaylistsListModel(m_xmmsClient, this);
    setModel(model);
    model->playlistAdded_Connect(&PlaylistsListView::checkNewPlaylist, this);

    itemEntered_Connect(&PlaylistsListView::loadPlaylist, this);
}

const std::string& PlaylistsListView::playlist(int item) const
{
    PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());
    return plsModel->playlist(item);
}

void PlaylistsListView::showEvent()
{
    if (!m_initialized) {
        PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());
        plsModel->init();
        m_initialized = true;
    }
    ListViewAppIntegrated::showEvent();
}

void PlaylistsListView::loadPlaylist(int item)
{
    PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());
    m_xmmsClient->playlistLoad(plsModel->playlist(item));
}

void PlaylistsListView::keyPressedEvent(const KeyEvent& keyEvent)
{
    PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());

    switch (keyEvent.key()) {
        case Hotkeys::Screens::PlaylistsBrowser::RemovePlaylist:
        {
            if (plsModel->itemsCount() > 1) {
                const std::vector<int>& _selectedItems = selectedItems();
                if (!_selectedItems.empty()) {
                    for (int item : _selectedItems) {
                        m_xmmsClient->playlistRemove(plsModel->playlist(item));
                    }
                } else {
                    m_xmmsClient->playlistRemove(plsModel->playlist(currentItem()));
                }
            }
            break;
        }

        case Hotkeys::Screens::PlaylistsBrowser::CreateNewPlaylist:
        {
            auto resultCallback = [this](const std::string& playlist, LineEdit::Result result)
            {
                if (result == LineEdit::Result::Accepted)
                    createPlaylist(playlist);
            };

            StatusArea::askQuestion("Create new playlist: ", resultCallback);
            break;
        }

        case Hotkeys::Screens::PlaylistsBrowser::RenamePlaylist:
        {
            const std::string playlist = plsModel->playlist(currentItem());
            auto resultCallback = [this, playlist](const std::string& newName, LineEdit::Result result)
            {
                if (result == LineEdit::Result::Accepted)
                    renamePlaylist(playlist, newName);
            };

            StatusArea::askQuestion("Rename playlist: ", resultCallback, playlist);
            break;
        }

        case Hotkeys::Screens::PlaylistsBrowser::GoToCurrentlyActivePlaylist:
            setCurrentItem(plsModel->indexOf(plsModel->currentPlaylist()));
            break;

        default: ListViewAppIntegrated::keyPressedEvent(keyEvent);
    }
}

void PlaylistsListView::createPlaylist(const std::string& playlist)
{
    PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());

    if (playlist.empty()) {
        StatusArea::showMessage("Can't create playlist with empty name!");
        return;
    }

    if (playlist[0] == '_') {
        StatusArea::showMessage("Playlist's name shouldn't start with _ !");
        return;
    }

    if (plsModel->playlistExists(playlist)) {
        StatusArea::showMessage("\"%s\" playlist already exists!", playlist);
        return;
    }

    m_newPlaylist = playlist;
    m_xmmsClient->playlistCreat(playlist);
}

void PlaylistsListView::renamePlaylist(const std::string& oldName, const std::string& newName)
{
    PlaylistsListModel *plsModel = static_cast<PlaylistsListModel*>(model());

    if (!plsModel->playlistExists(oldName))
        return;

    if (newName == oldName)
        return;

    if (newName.empty()) {
        StatusArea::showMessage("Playlist's name shouldn't be empty!");
        return;
    }

    if (newName[0] == '_') {
        StatusArea::showMessage("Playlist's name shouldn't start with _ !");
        return;
    }

    if (plsModel->playlistExists(newName)) {
        StatusArea::showMessage("\"%s\" playlist already exists!", newName);
        return;
    }

    m_xmmsClient->collectionRename(oldName, newName, "Playlists");
}

void PlaylistsListView::checkNewPlaylist(const std::string& playlist, int item)
{
    if (m_newPlaylist.empty())
        return;

    if (playlist == m_newPlaylist) {
        setCurrentItem(item);
        m_newPlaylist.clear();
    }
}

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
#include <boost/format.hpp>
#include <boost/cast.hpp>

#include "playlistslistview.h"
#include "playlistslistmodel.h"
#include "../statusarea/statusarea.h"
#include "../hotkeys.h"

#include "../lib/keyevent.h"

using namespace ncxmms2;

PlaylistsListView::PlaylistsListView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListView(rect, parent),
    m_xmmsClient(xmmsClient)
{
    loadPalette("PlaylistsListView");
    setModel(new PlaylistsListModel(xmmsClient, this));
    itemEntered_Connect(&PlaylistsListView::loadPlaylist, this);
}

const std::string& PlaylistsListView::playlist(int item) const
{
    PlaylistsListModel *plsModel =
            boost::polymorphic_downcast<PlaylistsListModel*>(model());

    return plsModel->playlist(item);
}

void PlaylistsListView::loadPlaylist(int item)
{
    PlaylistsListModel *plsModel =
            boost::polymorphic_downcast<PlaylistsListModel*>(model());

    m_xmmsClient->playlist.load(plsModel->playlist(item));
}

void PlaylistsListView::keyPressedEvent(const KeyEvent& keyEvent)
{
    PlaylistsListModel *plsModel =
            boost::polymorphic_downcast<PlaylistsListModel*>(model());

    switch (keyEvent.key()) {
        case Hotkeys::Screens::PlaylistsBrowser::RemovePlaylist:
            if (plsModel->itemsCount() > 1)
                m_xmmsClient->playlist.remove(plsModel->playlist(currentItem()));
            break;

        case Hotkeys::Screens::PlaylistsBrowser::CreateNewPlaylist:
        {
            auto resultCallback = [this](const std::string& playlist, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted)
                    createPlaylist(playlist);
            };

            StatusArea::askQuestion("Create new playlist: ", resultCallback);
            break;
        }

        case Hotkeys::Screens::PlaylistsBrowser::RenamePlaylist:
        {
            const std::string playlist = plsModel->playlist(currentItem());
            auto resultCallback = [this, playlist](const std::string& newName, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted)
                    renamePlaylist(playlist, newName);
            };

            StatusArea::askQuestion("Rename playlist: ", resultCallback, playlist);
            break;
        }

        default: ListView::keyPressedEvent(keyEvent);
    }
}

void PlaylistsListView::createPlaylist(const std::string& playlist)
{
    PlaylistsListModel *plsModel =
            boost::polymorphic_downcast<PlaylistsListModel*>(model());

    if (playlist.empty()) {
        StatusArea::showMessage("Can't create playlist with empty name!");
        return;
    }

    if (playlist[0] == '_') {
        StatusArea::showMessage("Playlist's name shouldn't start with _ !");
        return;
    }

    if (plsModel->playlistExists(playlist)) {
        StatusArea::showMessage(
            (boost::format("\"%1%\" playlist already exists!") % playlist).str()
        );
        return;
    }

    m_xmmsClient->playlist.create(playlist);
}

void PlaylistsListView::renamePlaylist(const std::string& oldName, const std::string& newName)
{
    PlaylistsListModel *plsModel =
            boost::polymorphic_downcast<PlaylistsListModel*>(model());

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
        StatusArea::showMessage(
            (boost::format("\"%1%\" playlist already exists!") % newName).str()
        );
        return;
    }

    m_xmmsClient->collection.rename(oldName, newName, Xmms::Collection::PLAYLISTS);
}

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

#include <glib.h>
#include <boost/format.hpp>
#include <boost/cast.hpp>

#include "playlistview.h"
#include "playlistmodel.h"
#include "playlistitemdelegate.h"

#include "../statusarea/statusarea.h"
#include "../utils.h"
#include "../xmmsutils.h"

#include "../lib/keyevent.h"

using namespace ncxmms2;

PlaylistView::PlaylistView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListView(rect, parent),
    m_xmmsClient(xmmsClient),
    m_playbackStatus(Xmms::Playback::STOPPED)
{
    PlaylistModel *plsModel = new PlaylistModel(m_xmmsClient, this);
    setModel(plsModel);
    setItemDelegate(new PlaylistItemDelegate(plsModel));
    setHideCurrentItemInterval(10);

    itemEntered_Connect(&PlaylistView::onItemEntered, this);

    m_xmmsClient->playlist.currentActive()(Xmms::bind(&PlaylistView::getActivePlaylist, this));
    m_xmmsClient->playlist.broadcastLoaded()(Xmms::bind(&PlaylistView::getActivePlaylist, this));

    m_xmmsClient->playback.getStatus()(Xmms::bind(&PlaylistView::getPlaybackStatus, this));
    m_xmmsClient->playback.broadcastStatus()(Xmms::bind(&PlaylistView::getPlaybackStatus, this));
}

void PlaylistView::setPlaylist(const std::string& playlist)
{
    PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());
    plsModel->setPlaylist(playlist);
}

void PlaylistView::keyPressedEvent(const KeyEvent& keyEvent)
{
    PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());

    switch (keyEvent.key()) {
        case KeyEvent::KeyDelete:
            if (plsModel->itemsCount() && !isCurrentItemHidden()) {
                m_xmmsClient->playlist.removeEntry(currentItem(), plsModel->playlist());
                showCurrentItem();
            }
            break;

        case 'c':
            m_xmmsClient->playlist.clear(plsModel->playlist());
            break;

        case 'S':
            m_xmmsClient->playlist.shuffle(plsModel->playlist());
            break;

        case 'o':
            setCurrentItem(plsModel->currentSongItem());
            break;

        // CTRL + O
        // FIXME: Remove this magic number
        // I didn't find an easy way to recognise modifiers keys,
        // other projects like ncmpc or ncmpcpp also use such hardcoded numbers
        case 15:
        {
            auto resultCallback = [this](const std::string& path, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted)
                    addPath(path);
            };
            StatusArea::askQuestion("Add path: ", resultCallback);
            break;
        }

        // CTRL + U
        // FIXME: See above comment
        case 21:
        {
            auto resultCallback = [this](const std::string& url, LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted)
                    addUrl(url);
            };
            StatusArea::askQuestion("Add url: ", resultCallback);
            break;
        }

        default: ListView::keyPressedEvent(keyEvent);
    }
}

bool PlaylistView::getActivePlaylist(const std::string& playlist)
{
    m_activePlaylist = playlist;
    return true;
}

bool PlaylistView::getPlaybackStatus(const Xmms::Playback::Status& status)
{
    m_playbackStatus = status;
    return true;
}

void PlaylistView::onItemEntered(int item)
{
    PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());

    if (plsModel->playlist() != m_activePlaylist)
        m_xmmsClient->playlist.load(plsModel->playlist());

    m_xmmsClient->playlist.setNext(item);
    m_xmmsClient->playback.tickle();
    switch (m_playbackStatus) {
        case Xmms::Playback::STOPPED:
            m_xmmsClient->playback.start();
            break;
        case Xmms::Playback::PLAYING:
            m_xmmsClient->playback.tickle();
            break;
        case Xmms::Playback::PAUSED:
            m_xmmsClient->playback.start();
            m_xmmsClient->playback.tickle();
    }
}

void PlaylistView::addPath(const std::string& path)
{
    // TODO: Introduce FileInfo class instead of using glib functions,
    // maybe simple stat call wrapper
    if (!g_file_test(path.c_str(), G_FILE_TEST_EXISTS)) {
        StatusArea::showMessage("File doesn't exist!");
        return;
    }

    if (g_file_test(path.c_str(), G_FILE_TEST_IS_REGULAR)) {
        addFile(path);
    } else if (g_file_test(path.c_str(), G_FILE_TEST_IS_DIR)) {
        PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());
        m_xmmsClient->playlist.addRecursive(std::string("file://").append(path),
                                            plsModel->playlist());
        // FIXME: Path may be too long to display
        StatusArea::showMessage(
            (boost::format("Adding \"%1%\" directory to \"%2%\" playlist") % path
                                                                           % plsModel->playlist()).str()
        );
    } else {
        StatusArea::showMessage("File is neither a directory nor regular file!");
    }
}

void PlaylistView::addFile(const std::string& path)
{
    PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());

    const std::string::size_type slashPos = path.rfind('/');
    const std::string fileName = slashPos != std::string::npos
                                 ? path.substr(slashPos + 1)
                                 : path;

    const Utils::FileType fileType = Utils::getFileType(path);
    switch (fileType) {
        case Utils::MediaFile:
            m_xmmsClient->playlist.addUrl(std::string("file://").append(path),
                                          plsModel->playlist());
            StatusArea::showMessage(
                (boost::format("Adding \"%1%\" file to \"%2%\" playlist") % fileName
                                                                          % plsModel->playlist()).str()
            );
            break;

        case Utils::PlaylistFile:
            XmmsUtils::playlistAddPlaylistFile(m_xmmsClient,
                                               plsModel->playlist(),
                                               std::string("file://").append(path));
            StatusArea::showMessage(
                (boost::format("Adding \"%1%\" playlist to \"%2%\" playlist") % fileName
                                                                              % plsModel->playlist()).str()
            );
            break;

        case Utils::UnknownFile:
            StatusArea::showMessage("Unknown file type!");
            break;
    }
}

void PlaylistView::addUrl(const std::string& url)
{
    PlaylistModel *plsModel = boost::polymorphic_downcast<PlaylistModel*>(model());
    m_xmmsClient->playlist.addUrl(url, plsModel->playlist());

    // FIXME: Url may be too long to display
    StatusArea::showMessage(
        (boost::format("Adding \"%1%\" to \"%2%\" playlist") % url
                                                             % plsModel->playlist()).str()
    );
}

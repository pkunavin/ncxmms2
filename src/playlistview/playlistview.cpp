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
#include <algorithm>
#include <glib.h>
#include <assert.h>

#include "playlistview.h"
#include "playlistmodel.h"
#include "playlistitemdelegate.h"

#include "../statusarea/statusarea.h"
#include "../hotkeys.h"
#include "../log.h"

#include "../lib/keyevent.h"

using namespace ncxmms2;

PlaylistView::PlaylistView(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListViewAppIntegrated(rect, parent),
    m_xmmsClient(xmmsClient),
    m_playbackStatus(xmms2::PlaybackStatus::Stopped)
{
    loadPalette("PlaylistView");

    PlaylistModel *plsModel = new PlaylistModel(m_xmmsClient, this);
    setModel(plsModel);
    setItemDelegate(new PlaylistItemDelegate(plsModel));
    setHideCurrentItemInterval(10);

    itemEntered_Connect(&PlaylistView::onItemEntered, this);

    m_xmmsClient->playlistCurrentActive()(&PlaylistView::getActivePlaylist, this);
    m_xmmsClient->playlistLoaded_Connect(&PlaylistView::getActivePlaylist, this);

    m_xmmsClient->playbackStatus()(&PlaylistView::getPlaybackStatus, this);
    m_xmmsClient->playbackStatusChanged_Connect(&PlaylistView::getPlaybackStatus, this);
}

void PlaylistView::setPlaylist(const std::string& playlist)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    plsModel->setPlaylist(playlist);
}

void PlaylistView::setDisplayFormat(const std::string& format)
{
    PlaylistItemDelegate *plsDelegate = static_cast<PlaylistItemDelegate*>(itemDelegate());
    plsDelegate->setDisplayFormat(format);
}

void PlaylistView::setLazyLoadPlaylist(bool enable)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    plsModel->setLazyLoadPlaylist(enable);
}

void PlaylistView::keyPressedEvent(const KeyEvent& keyEvent)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());

    switch (keyEvent.key()) {
        case Hotkeys::PlaylistView::RemoveEntry:
            removeSelectedSongs();
            break;

        case Hotkeys::PlaylistView::ClearPlaylist:
            m_xmmsClient->playlistClear(plsModel->playlist());
            break;

        case Hotkeys::PlaylistView::ShufflePlaylist:
            m_xmmsClient->playlistShuffle(plsModel->playlist());
            break;

        case Hotkeys::PlaylistView::GoToCurrentlyPlayingSong:
            setCurrentItem(plsModel->currentSongItem());
            break;

        case Hotkeys::PlaylistView::MoveSelectedSongs:
            moveSelectedSongs();
            break;
            
        case Hotkeys::PlaylistView::AddFileOrDirectory:
        {
            auto resultCallback = [this](const std::string& path, LineEdit::Result result)
            {
                if (result == LineEdit::Result::Accepted)
                    addPath(path);
            };
            StatusArea::askQuestion("Add path: ", resultCallback);
            break;
        }

        case Hotkeys::PlaylistView::AddUrl:
        {
            auto resultCallback = [this](const std::string& url, LineEdit::Result result)
            {
                if (result == LineEdit::Result::Accepted)
                    addUrl(url);
            };
            StatusArea::askQuestion("Add url: ", resultCallback);
            break;
        }
        
        case Hotkeys::PlaylistView::ShowSongInfo:
            if (plsModel->itemsCount() && !isCurrentItemHidden()) {
                int id = plsModel->song(currentItem()).id();
                if (id > 0)
                    showSongInfo(id);
            }
            break;
            
        case '+': // Select be regexp
            selectSongsByRegExp();
            break;

        case '\\': // Unselect be regexp
            unselectSongsByRegExp();
            break;

        default: ListViewAppIntegrated::keyPressedEvent(keyEvent);
    }
}

void PlaylistView::getActivePlaylist(const xmms2::Expected<StringRef>& playlist)
{
    if (playlist.isError()) {
        NCXMMS2_LOG_ERROR("%s", playlist.error().c_str());
        return;
    }
    
    m_activePlaylist = playlist->c_str();
}

void PlaylistView::getPlaybackStatus(const xmms2::Expected<xmms2::PlaybackStatus>& status)
{
    if (status.isError()) {
        NCXMMS2_LOG_ERROR("%s", status.error().c_str());
        return;
    }
    
    m_playbackStatus = status.value();
}

void PlaylistView::onItemEntered(int item)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());

    if (plsModel->playlist() != m_activePlaylist)
        m_xmmsClient->playlistLoad(plsModel->playlist());

    m_xmmsClient->playlistSetNext(item);
    m_xmmsClient->playbackTickle();
    switch (m_playbackStatus) {
        case xmms2::PlaybackStatus::Stopped:
            m_xmmsClient->playbackStart();
            break;
            
        case xmms2::PlaybackStatus::Playing:
            m_xmmsClient->playbackTickle();
            break;
            
        case xmms2::PlaybackStatus::Paused:
            m_xmmsClient->playbackStart();
            m_xmmsClient->playbackTickle();
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
        PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
        m_xmmsClient->playlistAddRecursive(plsModel->playlist(),
                                           std::string("file://").append(path));
        // FIXME: Path may be too long to display
        StatusArea::showMessage("Adding \"%s\" directory to \"%s\" playlist",
                                path, plsModel->playlist());
    } else {
        StatusArea::showMessage("File is neither a directory nor regular file!");
    }
}

void PlaylistView::addFile(const std::string& path)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());

    const std::string::size_type slashPos = path.rfind('/');
    const std::string fileName = slashPos != std::string::npos
                                 ? path.substr(slashPos + 1)
                                 : path;

    const Utils::FileType fileType = Utils::getFileType(path);
    switch (fileType) {
        case Utils::FileType::Media:
            m_xmmsClient->playlistAddUrl(plsModel->playlist(),
                                         std::string("file://").append(path));
            StatusArea::showMessage("Adding \"%s\" file to \"%s\" playlist",
                                    fileName, plsModel->playlist());
            break;

        case Utils::FileType::Playlist:
            m_xmmsClient->playlistAddPlaylistFile(plsModel->playlist(),
                                                  std::string("file://").append(path));
            StatusArea::showMessage("Adding \"%s\" playlist to \"%s\" playlist",
                                    fileName, plsModel->playlist());
            break;

        case Utils::FileType::Unknown:
            StatusArea::showMessage("Unknown file type!");
            break;
    }
}

void PlaylistView::addUrl(const std::string& url)
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    m_xmmsClient->playlistAddUrl(plsModel->playlist(), url);

    // FIXME: Url may be too long to display
    StatusArea::showMessage("Adding \"%s\" to \"%s\" playlist", url, plsModel->playlist());
}

void PlaylistView::selectSongsByRegExp()
{
    // We can't use selectItemsByRegExp here, because it matches ListModelItemData::text,
    // but for PlaylistModel its value doesn't correspond to what actually displyed, as
    // it uses SongDisplayFormatParser for this job.
    auto resultCallback = [this](const std::string& pattern, LineEdit::Result result)
    {
        if (result == LineEdit::Result::Accepted) {
            GRegex *regex = g_regex_new(pattern.c_str(), G_REGEX_OPTIMIZE,
                                        (GRegexMatchFlags)0, nullptr);
            if (!regex)
                return;

            PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
            PlaylistItemDelegate *delegate = static_cast<PlaylistItemDelegate*>(itemDelegate());

            selectItems([plsModel, delegate, regex](int item){
                return delegate->matchFormattedString(plsModel->song(item), regex);
            });
            g_regex_unref(regex);
            StatusArea::showMessage("%d items selected", selectedItems().size());
        }
    };
    StatusArea::askQuestion("Select items: ", resultCallback, ".*");
}

void PlaylistView::unselectSongsByRegExp()
{
    // The same story here...
    auto resultCallback = [this](const std::string& pattern, LineEdit::Result result)
    {
        if (result == LineEdit::Result::Accepted) {
            GRegex *regex = g_regex_new(pattern.c_str(), G_REGEX_OPTIMIZE,
                                        (GRegexMatchFlags)0, nullptr);
            if (!regex)
                return;

            PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
            PlaylistItemDelegate *delegate = static_cast<PlaylistItemDelegate*>(itemDelegate());

            unselectItems([plsModel, delegate, regex](int item){
                return delegate->matchFormattedString(plsModel->song(item), regex);
            });
            g_regex_unref(regex);
            StatusArea::showMessage("%d items selected", selectedItems().size());
        }
    };
    StatusArea::askQuestion("Unselect items: ", resultCallback, ".*");
}

void PlaylistView::removeSelectedSongs()
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    if (plsModel->itemsCount() && !isCurrentItemHidden()) {
        //   Actually we don't have to make copy of selectedItems, since removeEntry
        // doesn't modify it immediately, but this is not obvious and may lead to
        // problems in the future.
        const std::vector<int> selectedSongs = selectedItems();
        if (!selectedSongs.empty()) {
            assert(std::is_sorted(selectedSongs.begin(), selectedSongs.end()));
            std::for_each(selectedSongs.rbegin(), selectedSongs.rend(), [&](int item){
                m_xmmsClient->playlistRemoveEntry(plsModel->playlist(), item);
            });
        } else {
            m_xmmsClient->playlistRemoveEntry(plsModel->playlist(), currentItem());
        }
        showCurrentItem();
    }
}

void PlaylistView::moveSelectedSongs()
{
    PlaylistModel *plsModel = static_cast<PlaylistModel*>(model());
    const std::vector<int> selectedSongs  = selectedItems();
    assert(std::is_sorted(selectedSongs.begin(), selectedSongs.end()));
    
    if (isCurrentItemHidden() || selectedSongs.empty())
        return;
    
    const int moveTo = currentItem();
    auto it = std::lower_bound(selectedSongs.begin(), selectedSongs.end(), moveTo);
    
    int to = moveTo;
    for (size_t i = it - selectedSongs.begin(); i < selectedSongs.size(); ++i, ++to) {
        m_xmmsClient->playlistMoveEntry(plsModel->playlist(), selectedSongs[i], to);
    }
    
    to = it == selectedSongs.end() ? moveTo : moveTo - 1;
    for (ptrdiff_t i = it - selectedSongs.begin() - 1; i >= 0; --i, --to) {
        m_xmmsClient->playlistMoveEntry(plsModel->playlist(), selectedSongs[i], to);
    }
}

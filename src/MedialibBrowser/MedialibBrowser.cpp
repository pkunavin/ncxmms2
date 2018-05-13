/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <iterator>
#include <assert.h>

#include "MedialibBrowser.h"
#include "TagValueListModel.h"
#include "AlbumsListModel.h"
#include "SongsListModel.h"

#include "../XmmsUtils/Client.h"
#include "../ListViewAppIntegrated/ListViewAppIntegrated.h"
#include "../StatusArea/StatusArea.h"
#include "../Hotkeys.h"
#include "../Log.h"

#include "../lib/Painter.h"
#include "../lib/Rectangle.h"
#include "../lib/KeyEvent.h"

using namespace ncxmms2;

MedialibBrowser::MedialibBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    m_xmmsClient(xmmsClient)
{
    setName("Medialib browser");
    loadPalette("MedialibBrowser");

    const int artistsListViewCols = (cols() - 2) / 3;
    const Rectangle artistsListViewRect(0, HeaderLines,
                                        artistsListViewCols, lines() - HeaderLines);
    m_primaryTagListView = new ListViewAppIntegrated(artistsListViewRect, this);
    m_primaryTagListView->setModel(new TagValueListModel(m_xmmsClient, this));

    m_primaryTagListView->currentItemChanged_Connect(&MedialibBrowser::setAlbumsListViewFilterTag, this);
    m_primaryTagListView->itemEntered_Connect(&MedialibBrowser::activePlaylistPlayByPrimaryTag, this);
    m_primaryTagListView->setFocus();

    const int albumsListViewCols = artistsListViewCols;
    const Rectangle albumsListViewRect(artistsListViewCols + 1, HeaderLines,
                                       albumsListViewCols, lines() - HeaderLines);
    m_albumsListView = new ListViewAppIntegrated(albumsListViewRect, this);
    m_albumsListView->setModel(new AlbumsListModel(m_xmmsClient, this));
    m_albumsListView->currentItemChanged_Connect(&MedialibBrowser::setSongsListViewAlbum, this);
    m_albumsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistPlayAlbum, this);

    const int songsListViewCols = cols() - artistsListViewCols - albumsListViewCols - 2;
    const Rectangle songsListViewRect(cols() - songsListViewCols, HeaderLines,
                                      songsListViewCols, lines() - HeaderLines);
    m_songsListView = new ListViewAppIntegrated(songsListViewRect, this);
    m_songsListView->setModel(new SongsListModel(m_xmmsClient, this));
    m_songsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistPlaySong, this);

    // This will reload the whole medialib on adding new medialib entry.
    // TODO: Can it be done in a more clever way?
    m_xmmsClient->medialibEntryAdded_Connect([this](const xmms2::Expected<int>& id)
    {
        NCXMMS2_UNUSED(id);
        m_primaryTagListView->model()->refresh();
    });
}

void MedialibBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    ListView *listViewSwitchOrder[] =
    {
        nullptr,
        m_primaryTagListView,
        m_albumsListView,
        m_songsListView,
        nullptr
    };

    auto activeListViewIt = std::find_if(std::begin(listViewSwitchOrder) + 1,
                                         std::end(listViewSwitchOrder) - 1,
                                         [](ListView *view){return view->hasFocus();});
    assert(activeListViewIt != std::end(listViewSwitchOrder) - 1);
    ListView *activeListView = *activeListViewIt;

    switch (keyEvent.key()) {
        case KeyEvent::KeyRight:
            if (*(++activeListViewIt))
                (*activeListViewIt)->setFocus();
            break;

        case KeyEvent::KeyLeft:
            if (*(--activeListViewIt))
                (*activeListViewIt)->setFocus();
            break;

        case Hotkeys::Screens::MedialibBrowser::AddItemToActivePlaylist:
        {
            const int currentItem = activeListView->currentItem();
            const std::vector<int>& selectedItems = activeListView->selectedItems();

#define ADD_ITEMS_TO_ACTIVE_PLAYLIST(addFunction, itemDescription)                      \
    do {                                                                                \
        if (!selectedItems.empty()) {                                                   \
            for (int item : selectedItems) {                                            \
                addFunction(item, true);                                                \
            }                                                                           \
            StatusArea::showMessage("Adding %s " itemDescription " to active playlist", \
                                    selectedItems.size());                              \
            activeListView->clearSelection();                                           \
        } else {                                                                        \
            if (currentItem != -1)                                                      \
                addFunction(currentItem);                                               \
        }                                                                               \
    } while (0)

            if (activeListView == m_songsListView) {
                ADD_ITEMS_TO_ACTIVE_PLAYLIST(activePlaylistAddSong, "songs");
            } else if (activeListView == m_albumsListView) {
                ADD_ITEMS_TO_ACTIVE_PLAYLIST(activePlaylistAddAlbum, "albums");
            } else if (activeListView == m_primaryTagListView) {
                ADD_ITEMS_TO_ACTIVE_PLAYLIST(activePlaylistAddByPrimaryTag, "items");
            }

#undef ADD_ITEMS_TO_ACTIVE_PLAYLIST
            break;
        }

        case Hotkeys::Screens::MedialibBrowser::ShowSongInfo:
            if (activeListView == m_songsListView) {
                SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
                const int currentItem = m_songsListView->currentItem();
                if (currentItem != -1)
                    showSongInfo(songsModel->songId(currentItem));
            }
            break;
            
        case Hotkeys::Screens::MedialibBrowser::Refresh:
            activeListView->model()->refresh();
            break;

        case Hotkeys::Screens::MedialibBrowser::SetPrimaryTag:
            askPrimaryListViewTag();
            break;

        default: Window::keyPressedEvent(keyEvent);
    }
}

void MedialibBrowser::resizeChildren(const Size &size)
{
    const int artistsListViewCols = (cols() - 2) / 3;
    m_primaryTagListView->resize(Size(artistsListViewCols, size.lines() - HeaderLines));

    const int albumsListViewCols = artistsListViewCols;
    m_albumsListView->resize(Size(albumsListViewCols, size.lines() - HeaderLines));
    m_albumsListView->move(artistsListViewCols + 1, HeaderLines);

    const int songsListViewCols = size.cols() - artistsListViewCols - albumsListViewCols - 2;
    m_songsListView->resize(Size(songsListViewCols, size.lines() - HeaderLines));
    m_songsListView->move(size.cols() - songsListViewCols, HeaderLines);
}

void MedialibBrowser::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    const int artistsListViewCols = (cols() - 2) / 3;
    const int albumsListViewCols = artistsListViewCols;
    const int songsListViewCols = cols() - artistsListViewCols - albumsListViewCols - 2;

    Painter painter(this);
    painter.clearLine();
    painter.printString(getPrimaryListViewName());
    painter.move(artistsListViewCols + 1, 0);
    painter.printString(getAlbumsListViewName());
    painter.move(cols() - songsListViewCols, 0);
    painter.printString("Song");

    painter.drawHLine(0, 1, cols());

    painter.drawVLine(artistsListViewCols, 0, lines());
    painter.drawVLine(artistsListViewCols + albumsListViewCols + 1, 0, lines());
    painter.flush();
}

void MedialibBrowser::showEvent()
{
    TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    if (primaryListModel->itemsCount() == 0)
        primaryListModel->refresh();
    Window::showEvent();
}

void MedialibBrowser::askPrimaryListViewTag()
{
    auto resultCallback = [this](const std::string& ans, LineEdit::Result result)
    {
        if (result != LineEdit::Result::Accepted || ans.empty())
            return;

        switch (ans[0]) {
            case 'a': setPrimaryListViewTag(Song::Tag::Artist);      break;
            case 'A': setPrimaryListViewTag(Song::Tag::AlbumArtist); break;
            case 'y': setPrimaryListViewTag(Song::Tag::Year);        break;
            case 'g': setPrimaryListViewTag(Song::Tag::Genre);       break;
            case 'c': setPrimaryListViewTag(Song::Tag::Composer);    break;
            case 'p': setPrimaryListViewTag(Song::Tag::Performer);   break;
            default:
                StatusArea::showMessage("Wrong tag type!");
                break;
        }
    };
    const std::string msg
            = "Tag type? [<b>a</b>rtist/album<b>A</b>rtist/<b>y</b>ear/<b>g</b>enre/<b>c</b>omposer/<b>p</b>erformer]: ";
    StatusArea::askQuestion(msg, resultCallback);
}

void MedialibBrowser::setPrimaryListViewTag(Song::Tag tag)
{
    TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    primaryListModel->setTag(tag);
    primaryListModel->refresh();
    update();
}

std::string MedialibBrowser::getPrimaryListViewName() const
{
    const TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    switch (primaryListModel->tag()) {
        case Song::Tag::Artist:      return "Artist";
        case Song::Tag::AlbumArtist: return "Album Artist";
        case Song::Tag::Year:        return "Year";
        case Song::Tag::Genre:       return "Genre";
        case Song::Tag::Composer:    return "Composer";
        case Song::Tag::Performer:   return "Performer";
        default:
            assert(false);
            return "";
    }
}

std::string MedialibBrowser::getAlbumsListViewName() const
{
    const TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    switch (primaryListModel->tag()) {
        case Song::Tag::Year:
        case Song::Tag::Genre:
            return "[Artist] Album";
        default:
            return "Album";
    }
}

void MedialibBrowser::setAlbumsListViewFilterTag(int item)
{
    TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    if (item != -1)
        albumsModel->setFilterByTag(primaryListModel->tag(), primaryListModel->tagValue(item));
}

void MedialibBrowser::setSongsListViewAlbum(int item)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());

    songsModel->setAlbum(albumsModel->getAlbumsCollection(),
                         item != -1 ? albumsModel->album(item) : std::string());
}

void MedialibBrowser::activePlaylistAddSong(int item, bool beQuiet)
{
    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    m_xmmsClient->playlistAddId(m_xmmsClient->playlistCurrentActive(), songsModel->songId(item));
    if (!beQuiet)
        StatusArea::showMessage("Adding \"%s\" song to active playlist", songsModel->title(item));
}

void MedialibBrowser::activePlaylistAddAlbum(int item, bool beQuiet)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    const auto album  = albumsModel->album(item);
    auto albumColl = SongsListModel::getSongsCollection(albumsModel->getAlbumsCollection(), album);

    m_xmmsClient->playlistAddCollection(m_xmmsClient->playlistCurrentActive(), albumColl, songsModel->sortingOrder());
    if (!beQuiet)
        StatusArea::showMessage("Adding \"%s\" album to active playlist", album);
}

void MedialibBrowser::activePlaylistAddByPrimaryTag(int item, bool beQuiet)
{
    TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    const auto primaryTag      = primaryListModel->tag();
    const auto primaryTagValue = primaryListModel->tagValue(item);

    xmms2::Collection            albumsColl = albumsModel->getAlbumsCollection();
    const std::vector<std::string>    fetch = {"album"};
    const std::vector<std::string>& groupBy = fetch;

    m_xmmsClient->collectionQueryInfos(albumsColl, fetch, albumsModel->sortingOrder(), groupBy)(
                &MedialibBrowser::activePlaylistAddAlbums, this, primaryTag, primaryTagValue, std::placeholders::_1, beQuiet);
}

void MedialibBrowser::activePlaylistAddAlbums(Song::Tag primaryTag,
                                              const std::string& primaryTagValue,
                                              const xmms2::Expected<xmms2::List<xmms2::Dict>>& list,
                                              bool beQuiet)
{
    if (list.isError()) {
        NCXMMS2_LOG_ERROR("%s", list.error());
        return;
    }

    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());

    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        std::string album = dict.value<std::string>("album");
        auto albumColl = songsModel->getSongsCollection(AlbumsListModel::getAlbumsCollection(primaryTag, primaryTagValue), album);
        m_xmmsClient->playlistAddCollection(m_xmmsClient->playlistCurrentActive(), albumColl, songsModel->sortingOrder());
    }

    if (!beQuiet)
        StatusArea::showMessage("Adding all albums of \"%s\" to active playlist", primaryTagValue);
}

void MedialibBrowser::activePlaylistPlaySong(int item)
{
    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    m_xmmsClient->playlistPlayId(m_xmmsClient->playlistCurrentActive(), songsModel->songId(item));
}

void MedialibBrowser::activePlaylistPlayAlbum(int item)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    const auto album  = albumsModel->album(item);

    auto albumColl = SongsListModel::getSongsCollection(albumsModel->getAlbumsCollection(), album);
    activePlaylistPlayAlbumColl(albumColl, songsModel->sortingOrder());
}

void MedialibBrowser::activePlaylistPlayAlbumColl(const xmms2::Collection& songs,
                                                  const std::vector<std::string>& sortingOrder)
{
    m_xmmsClient->collectionQueryInfos(songs, {"id"}, sortingOrder)(
    [this](const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
    {
        if (list.isError()) {
            NCXMMS2_LOG_ERROR("%s", list.error());
            return;
        }
        
        auto it = list->getIterator();
        if (!it.isValid())
            return;
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (!ok)
            return;
        m_xmmsClient->playlistPlayId(m_xmmsClient->playlistCurrentActive(), dict.value<int>("id"));
        it.next();
        
        for (; it.isValid(); it.next()) {
            dict = it.value(&ok);
            if (NCXMMS2_UNLIKELY(!ok))
                continue;
            m_xmmsClient->playlistAddId(m_xmmsClient->playlistCurrentActive(), dict.value<int>("id"));
        }
    });
}

void MedialibBrowser::activePlaylistAddAlbumColl(const xmms2::Collection& songs,
                                                 const std::vector<std::string>& sortingOrder)
{
    m_xmmsClient->collectionQueryInfos(songs, {"id"}, sortingOrder)(
    [this](const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
    {
        if (list.isError()) {
            NCXMMS2_LOG_ERROR("%s", list.error());
            return;
        }
        
        for (auto it = list->getIterator(); it.isValid(); it.next()) {
            bool ok = false;
            xmms2::Dict dict = it.value(&ok);
            if (NCXMMS2_UNLIKELY(!ok))
                continue;
            m_xmmsClient->playlistAddId(m_xmmsClient->playlistCurrentActive(), dict.value<int>("id"));
        }
    });
}

void MedialibBrowser::activePlaylistPlayByPrimaryTag(int item)
{
    TagValueListModel *primaryListModel = static_cast<TagValueListModel*>(m_primaryTagListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());

    const auto primaryTag        = primaryListModel->tag();
    const auto primaryTagValue   = primaryListModel->tagValue(item);
    xmms2::Collection albumsColl = AlbumsListModel::getAlbumsCollection(primaryTag, primaryTagValue);

    const std::vector<std::string>    fetch = {"album"};
    const std::vector<std::string>& groupBy = fetch;
    
    m_xmmsClient->collectionQueryInfos(albumsColl, fetch, albumsModel->sortingOrder(), groupBy)(
    [=](const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
    {
        if (list.isError()) {
            NCXMMS2_LOG_ERROR("%s", list.error());
            return;
        }
        SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
        
        auto it = list->getIterator();
        if (!it.isValid())
            return;
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (!ok)
            return;
        std::string album = dict.value<std::string>("album");
        auto albumColl = songsModel->getSongsCollection(AlbumsListModel::getAlbumsCollection(primaryTag, primaryTagValue), album);
        activePlaylistPlayAlbumColl(albumColl, songsModel->sortingOrder());
        it.next();
        
        for (; it.isValid(); it.next()) {
            dict = it.value(&ok);
            if (NCXMMS2_UNLIKELY(!ok))
                continue;
            album = dict.value<std::string>("album");
            auto albumColl = songsModel->getSongsCollection(AlbumsListModel::getAlbumsCollection(primaryTag, primaryTagValue), album);
            activePlaylistAddAlbumColl(albumColl, songsModel->sortingOrder());
        }
    });
}

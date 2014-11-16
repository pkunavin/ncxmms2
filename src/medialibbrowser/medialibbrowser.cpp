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

#include <vector>
#include <algorithm>
#include <iterator>
#include <assert.h>

#include "medialibbrowser.h"
#include "artistslistmodel.h"
#include "albumslistmodel.h"
#include "songslistmodel.h"

#include "../xmmsutils/client.h"
#include "../listviewappintegrated/listviewappintegrated.h"
#include "../statusarea/statusarea.h"
#include "../hotkeys.h"
#include "../log.h"

#include "../lib/painter.h"
#include "../lib/rectangle.h"
#include "../lib/keyevent.h"

using namespace ncxmms2;

MedialibBrowser::MedialibBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    m_xmmsClient(xmmsClient)
{
    setName("Medialib browser");
    loadPalette("MedialibBrowser");

    const int headerLines = 2;
    const int artistsListViewCols = (cols() - 2) / 3;
    const Rectangle artistsListViewRect(0, headerLines,
                                        artistsListViewCols, lines() - headerLines);
    m_artistsListView = new ListViewAppIntegrated(artistsListViewRect, this);
    m_artistsListView->setModel(new ArtistsListModel(m_xmmsClient, this));
    m_artistsListView->currentItemChanged_Connect(&MedialibBrowser::setAlbumsListViewArtist, this);
    m_artistsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddArtist, this,  std::placeholders::_1, false); //TODO: Play artist
    m_artistsListView->setFocus();

    const int albumsListViewCols = artistsListViewCols;
    const Rectangle albumsListViewRect(artistsListViewCols + 1, headerLines,
                                       albumsListViewCols, lines() - headerLines);
    m_albumsListView = new ListViewAppIntegrated(albumsListViewRect, this);
    m_albumsListView->setModel(new AlbumsListModel(m_xmmsClient, this));
    m_albumsListView->currentItemChanged_Connect(&MedialibBrowser::setSongsListViewAlbum, this);
    m_albumsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddAlbum, this,  std::placeholders::_1, false); //TODO: Play album

    const int songsListViewCols = cols() - artistsListViewCols - albumsListViewCols - 2;
    const Rectangle songsListViewRect(cols() - songsListViewCols, headerLines,
                                      songsListViewCols, lines() - headerLines);
    m_songsListView = new ListViewAppIntegrated(songsListViewRect, this);
    m_songsListView->setModel(new SongsListModel(m_xmmsClient, this));
    m_songsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddSong, this, std::placeholders::_1, false); //TODO: Play song

    // This will reload the whole medialib on adding new medialib entry.
    // TODO: Can it be done in a more clever way?
    m_xmmsClient->medialibEntryAdded_Connect([this](const xmms2::Expected<int>& id)
    {
        NCXMMS2_UNUSED(id);
        m_artistsListView->model()->refresh();
    });
}

void MedialibBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    ListView *listViewSwitchOrder[] =
    {
        nullptr,
        m_artistsListView,
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
            } else if (activeListView == m_artistsListView) {
                ADD_ITEMS_TO_ACTIVE_PLAYLIST(activePlaylistAddArtist, "artists");
            }

#undef ADD_ITEMS_TO_ACTIVE_PLAYLIST
            break;
        }

        case Hotkeys::Screens::MedialibBrowser::ShowSongInfo:
            if (activeListView == m_songsListView) {
                SongsListModel *songsModel =
                        static_cast<SongsListModel*>(m_songsListView->model());
                const int currentItem = m_songsListView->currentItem();
                if (currentItem != -1)
                    showSongInfo(songsModel->id(currentItem));
            }
            break;
            
        case Hotkeys::Screens::MedialibBrowser::Refresh:
            activeListView->model()->refresh();
            break;

        default: Window::keyPressedEvent(keyEvent);
    }
}

void MedialibBrowser::resizeChildren(const Size &size)
{
    const int headerLines = 2;
    const int artistsListViewCols = (cols() - 2) / 3;
    m_artistsListView->resize(Size(artistsListViewCols, size.lines() - headerLines));

    const int albumsListViewCols = artistsListViewCols;
    m_albumsListView->resize(Size(albumsListViewCols, size.lines() - headerLines));
    m_albumsListView->move(artistsListViewCols + 1, headerLines);

    const int songsListViewCols = size.cols() - artistsListViewCols - albumsListViewCols - 2;
    m_songsListView->resize(Size(songsListViewCols, size.lines() - headerLines));
    m_songsListView->move(size.cols() - songsListViewCols, headerLines);
}

void MedialibBrowser::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    const int artistsListViewCols = (cols() - 2) / 3;
    const int albumsListViewCols = artistsListViewCols;
    const int songsListViewCols = cols() - artistsListViewCols - albumsListViewCols - 2;

    Painter painter(this);
    painter.clearLine();
    painter.printString("Artists:");
    painter.move(artistsListViewCols + 1, 0);
    painter.printString("Albums:");
    painter.move(cols() - songsListViewCols, 0);
    painter.printString("Songs:");

    painter.drawHLine(0, 1, cols());

    painter.drawVLine(artistsListViewCols, 0, lines());
    painter.drawVLine(artistsListViewCols + albumsListViewCols + 1, 0, lines());
    painter.flush();
}

void MedialibBrowser::showEvent()
{
    ArtistsListModel *artistsModel = static_cast<ArtistsListModel*>(m_artistsListView->model());
    if (artistsModel->itemsCount() == 0)
        artistsModel->refresh();
    Window::showEvent();
}

void MedialibBrowser::setAlbumsListViewArtist(int item)
{
    ArtistsListModel *artistsModel = static_cast<ArtistsListModel*>(m_artistsListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    if (item != -1)
        albumsModel->setArtist(artistsModel->artist(item));
}

void MedialibBrowser::setSongsListViewAlbum(int item)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());

    songsModel->setAlbumByArtist(albumsModel->artist(),
                                 item != -1 ? albumsModel->album(item) : std::string());
}

void MedialibBrowser::activePlaylistAddSong(int item, bool beQuiet)
{
    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    m_xmmsClient->playlistAddId("_active", songsModel->id(item));
    if (!beQuiet)
        StatusArea::showMessage("Adding \"%s\" song to active playlist", songsModel->title(item));
}

void MedialibBrowser::activePlaylistAddAlbum(int item, bool beQuiet)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    const auto artist = albumsModel->artist();
    const auto album  = albumsModel->album(item);

    xmms2::Collection songs = xmms2::Collection::albumByArtist(artist, album);

    m_xmmsClient->playlistAddCollection("_active", songs, songsModel->sortingOrder());
    if (!beQuiet)
        StatusArea::showMessage("Adding \"%s\" album to active playlist", album);
}

void MedialibBrowser::activePlaylistAddArtist(int item, bool beQuiet)
{
    ArtistsListModel *artistsModel = static_cast<ArtistsListModel*>(m_artistsListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    const auto artist = artistsModel->artist(item);

    xmms2::Collection albums = xmms2::Collection::allByArtist(artist);
    const std::vector<std::string>    fetch = {"album"};
    const std::vector<std::string>& groupBy = fetch;
    
    m_xmmsClient->collectionQueryInfos(albums, fetch, albumsModel->sortingOrder(), groupBy)(
        &MedialibBrowser::activePlaylistAddAlbums, this, artist, std::placeholders::_1, beQuiet);
}

void MedialibBrowser::activePlaylistAddAlbums(const std::string& artist,
                                              const xmms2::Expected<xmms2::List<xmms2::Dict>>& list,
                                              bool beQuiet)
{
    if (list.isError()) {
        NCXMMS2_LOG_ERROR("%s", list.error().c_str());
        return;
    }
    
    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        std::string album = dict.value<std::string>("album");
        xmms2::Collection songs = xmms2::Collection::albumByArtist(artist, album);
        m_xmmsClient->playlistAddCollection("_active", songs, songsModel->sortingOrder());
    }
    
    if (!beQuiet)
        StatusArea::showMessage("Adding all albums by \"%s\" to active playlist", artist);
}

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
#include <algorithm>
#include <iterator>
#include <assert.h>

#include "medialibbrowser.h"
#include "artistslistmodel.h"
#include "albumslistmodel.h"
#include "songslistmodel.h"
#include "../statusarea/statusarea.h"
#include "../hotkeys.h"

#include "../lib/listview.h"
#include "../lib/painter.h"
#include "../lib/rectangle.h"
#include "../lib/keyevent.h"

using namespace ncxmms2;

MedialibBrowser::MedialibBrowser(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    m_xmmsClient(xmmsClient)
{
    setName("Medialib browser");
    loadPalette("MedialibBrowser");

    const int headerLines = 2;
    const int artistsListViewCols = (cols() - 2) / 3;
    const Rectangle artistsListViewRect(0, headerLines,
                                        artistsListViewCols, lines() - headerLines);
    m_artistsListView = new ListView(artistsListViewRect, this);
    m_artistsListView->setModel(new ArtistsListModel(xmmsClient, this));
    m_artistsListView->currentItemChanged_Connect(&MedialibBrowser::setAlbumsListViewArtist, this);
    m_artistsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddArtist, this); //TODO: Play artist
    m_activeListView = m_artistsListView;
    m_artistsListView->setFocus();

    const int albumsListViewCols = artistsListViewCols;
    const Rectangle albumsListViewRect(artistsListViewCols + 1, headerLines,
                                       albumsListViewCols, lines() - headerLines);
    m_albumsListView = new ListView(albumsListViewRect, this);
    m_albumsListView->setModel(new AlbumsListModel(xmmsClient, this));
    m_albumsListView->currentItemChanged_Connect(&MedialibBrowser::setSongsListViewAlbum, this);
    m_albumsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddAlbum, this); //TODO: Play album

    const int songsListViewCols = cols() - artistsListViewCols - albumsListViewCols - 2;
    const Rectangle songsListViewRect(cols() - songsListViewCols, headerLines,
                                      songsListViewCols, lines() - headerLines);
    m_songsListView = new ListView(songsListViewRect, this);
    m_songsListView->setModel(new SongsListModel(xmmsClient, this));
    m_songsListView->itemEntered_Connect(&MedialibBrowser::activePlaylistAddSong, this); //TODO: Play song

    // This will reload the whole medialib on adding new medialib entry.
    // TODO: Can it be done in a more clever way?
    m_xmmsClient->medialib.broadcastEntryAdded()([this](const int& id)
    {
        NCXMMS2_UNUSED(id);
        m_artistsListView->model()->refresh();
        return true;
    });
}

void MedialibBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyRight:
        {
            ListView *rightSwitchOrder[] =
            {
                m_artistsListView,
                m_albumsListView,
                m_songsListView,
                nullptr
            };

            auto it = std::find(std::begin(rightSwitchOrder), std::end(rightSwitchOrder),
                                m_activeListView);
            assert(it != std::end(rightSwitchOrder));
            if (*(++it)) {
                m_activeListView = *it;
                m_activeListView->setFocus();
            }

            break;
        }

        case KeyEvent::KeyLeft:
        {
            ListView *leftSwitchOrder[] =
            {
                nullptr,
                m_artistsListView,
                m_albumsListView,
                m_songsListView
            };

            auto it = std::find(std::begin(leftSwitchOrder), std::end(leftSwitchOrder),
                                m_activeListView);
            assert(it != std::end(leftSwitchOrder));
            if (*(--it)) {
                m_activeListView = *it;
                m_activeListView->setFocus();
            }

            break;
        }

        case Hotkeys::Screens::MedialibBrowser::AddItemToActivePlaylist:
            if (m_activeListView == m_songsListView) {
                activePlaylistAddSong(m_songsListView->currentItem());
            } else if (m_activeListView == m_albumsListView) {
                activePlaylistAddAlbum(m_albumsListView->currentItem());
            } else if (m_activeListView == m_artistsListView) {
                activePlaylistAddArtist(m_artistsListView->currentItem());
            }

            break;

        case Hotkeys::Screens::MedialibBrowser::Refresh:
            assert(m_activeListView);
            m_activeListView->model()->refresh();
            break;

        default: Window::keyPressedEvent(keyEvent);
    }
}

void MedialibBrowser::resize(const Size& size)
{
    // We do not want to update on every move and resize, juct show
    // final result, also there is some bug in move and resize routines,
    // which leads to Segmentation fault
    // TODO: find this bug.
    const bool needHide = !isHidden();
    if (needHide)
        hide();

    Window::resize(size);

    const int headerLines = 2;
    const int artistsListViewCols = (cols() - 2) / 3;
    m_artistsListView->resize(Size(artistsListViewCols, size.lines() - headerLines));

    const int albumsListViewCols = artistsListViewCols;
    m_albumsListView->resize(Size(albumsListViewCols, size.lines() - headerLines));
    m_albumsListView->move(artistsListViewCols + 1, headerLines);

    const int songsListViewCols = size.cols() - artistsListViewCols - albumsListViewCols - 2;
    m_songsListView->resize(Size(songsListViewCols, size.lines() - headerLines));
    m_songsListView->move(size.cols() - songsListViewCols, headerLines);

    if (needHide)
        show();
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

void MedialibBrowser::activePlaylistAddSong(int item)
{
    SongsListModel *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    m_xmmsClient->playlist.addId(songsModel->id(item));
    StatusArea::showMessage(
        (boost::format("Adding \"%1%\" song to active playlist") % songsModel->title(item)).str()
    );
}

void MedialibBrowser::activePlaylistAddAlbum(int item)
{
    AlbumsListModel *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    const auto artist = albumsModel->artist();
    const auto album  = albumsModel->album(item);

    Xmms::Coll::Universe     allMedia;
    Xmms::Coll::Equals       allByArtist(allMedia, "artist", artist, true);
    const Xmms::Coll::Equals albumByArtist(allByArtist, "album", album, true);

    m_xmmsClient->playlist.addCollection(albumByArtist, songsModel->sortingOrder());
    StatusArea::showMessage(
        (boost::format("Adding \"%1%\" album to active playlist") % album).str()
    );
}

void MedialibBrowser::activePlaylistAddArtist(int item)
{
    ArtistsListModel *artistsModel = static_cast<ArtistsListModel*>(m_artistsListView->model());
    AlbumsListModel  *albumsModel = static_cast<AlbumsListModel*>(m_albumsListView->model());
    const auto artist = artistsModel->artist(item);

    Xmms::Coll::Universe     allMedia;
    const Xmms::Coll::Equals allByArtist(allMedia, "artist", artist, true);

    const std::list<std::string>   fetch = {"album"};
    const std::list<std::string> groupBy = {"album"};

    m_xmmsClient->collection.queryInfos(allByArtist, fetch, albumsModel->sortingOrder(), 0, 0, groupBy)(
        boost::bind(&MedialibBrowser::activePlaylistAddAlbums, this, artist, _1)
    );
}

bool MedialibBrowser::activePlaylistAddAlbums(const std::string& artist,
                                              const Xmms::List<Xmms::Dict>& list)
{
    SongsListModel  *songsModel = static_cast<SongsListModel*>(m_songsListView->model());
    Xmms::Coll::Universe allMedia;

    for (auto it = list.begin(), it_end = list.end(); it != it_end; ++it) {
        try {
            const std::string album = (*it).get<std::string>("album");

            Xmms::Coll::Equals       allByArtist(allMedia, "artist", artist, true);
            const Xmms::Coll::Equals albumByArtist(allByArtist, "album", album, true);

            m_xmmsClient->playlist.addCollection(albumByArtist, songsModel->sortingOrder());
        }
        catch (...) {
            continue;
        }
    }

    StatusArea::showMessage(
        (boost::format("Adding all albums by \"%1%\" to active playlist") % artist).str()
    );
    return true;
}

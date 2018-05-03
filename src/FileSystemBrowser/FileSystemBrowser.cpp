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

#include "FileSystemBrowser.h"
#include "AbstractFileSystemModel.h"
#include "../StatusArea/StatusArea.h"
#include "../XmmsUtils/Client.h"
#include "../Utils.h"
#include "../Hotkeys.h"
#include "../Log.h"
#include "../lib/KeyEvent.h"

using namespace ncxmms2;

FileSystemBrowser::FileSystemBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListViewAppIntegrated(rect, parent),
    m_xmmsClient(xmmsClient),
    m_currentDir("/")
{
    itemEntered_Connect(&FileSystemBrowser::onItemEntered, this);
}

void FileSystemBrowser::setFsModel(AbstractFileSystemModel *fsModel)
{
    setModel(fsModel);
    fsModel->directoryLoaded_Connect(&FileSystemBrowser::directoryLoaded, this);
    fsModel->directoryLoadFailed_Connect(&FileSystemBrowser::directoryLoadFailed, this);
}

AbstractFileSystemModel * FileSystemBrowser::fsModel() const
{
    assert(dynamic_cast<AbstractFileSystemModel*>(model()));
    return static_cast<AbstractFileSystemModel*>(model());
}

void FileSystemBrowser::setDirectory(const Dir& dir)
{
    fsModel()->setDirectory(dir);
}

const Dir& FileSystemBrowser::directory() const
{
    return m_currentDir;
}

void FileSystemBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    namespace FsBrowser = Hotkeys::Screens::FileSystemBrowser;
    switch (keyEvent.key()) {
        case FsBrowser::AddItemToActivePlaylist: addItemToActivePlaylist();              break;
        case FsBrowser::GoUp:                    goUp();                                 break;
        case FsBrowser::ChangeDirectory:         askChangeDirectory();                   break;
        case FsBrowser::ReloadDirectory:         reloadDirectory();                      break;
        case FsBrowser::ShowSongInfo:            getFileIdAndShowSongInfo();             break;
        case KeyEvent::KeyInsert:                toggleSelectionWithoutDotDot(keyEvent); break;
        case '*':                                invertSelectionWithoutDotDot(keyEvent); break;
        case '+':                                selectByRegexpWithoutDotDot();          break;
        default: ListViewAppIntegrated::keyPressedEvent(keyEvent);                       break;
    }
}

void FileSystemBrowser::onItemEntered(int item)
{
    if (fsModel()->isDirectory(item)) {
        // FileSystemModel::fileName returns a reference, but next function will
        // modify model content, so we need to copy it
        cd(std::string(fsModel()->fileName(item)));
    } else {
        activePlaylistPlayItem(item);
    }
}

void FileSystemBrowser::cd(const std::string& dir)
{
    Dir newDir = Dir(m_currentDir).cd(dir);
    setDirectory(newDir);
}

void FileSystemBrowser::directoryLoaded(const Dir& dir)
{
    Dir oldDir = m_currentDir;
    m_currentDir = dir;

    if (Dir(oldDir).cdUp() == m_currentDir) { // Old directory is a subdirectory of the current one
        const int index = fsModel()->fileIndex(oldDir.name());
        if (index != -1)
            setCurrentItem(index);
    }
}

void FileSystemBrowser::directoryLoadFailed(const Dir& dir, const std::string& error)
{
    StatusArea::showMessage("Can't open %s : %s", dir.name(), error);
}

void FileSystemBrowser::addItemToActivePlaylist()
{
    const std::vector<int>& selectedItems = this->selectedItems();
    if (!selectedItems.empty()) {
        for (int item : selectedItems) {
            const bool beQuiet = true;
            activePlaylistAddFileOrDirectory(item, beQuiet);
        }
        StatusArea::showMessage("Adding %d items to active playlist", selectedItems.size());
        clearSelection();
    } else {
        const int item = currentItem();
        if (item != -1)
            activePlaylistAddFileOrDirectory(item);
    }    
}

void FileSystemBrowser::activePlaylistAddFileOrDirectory(int item, bool beQuiet)
{
    assert(item >= 0 && item < fsModel()->itemsCount());

    if (fsModel()->isDirectory(item)) {
        m_xmmsClient->playlistAddRecursive(m_xmmsClient->playlistCurrentActive(), fsModel()->fileUrl(item));
        if (!beQuiet) {
            StatusArea::showMessage("Adding \"%s\" directory to active playlist", fsModel()->fileName(item));
        }
     } else {
        activePlaylistAddFile(item, beQuiet);
    }
}

void FileSystemBrowser::activePlaylistAddFile(int item, bool beQuiet)
{
    assert(item >= 0 && item < fsModel()->itemsCount());

    const std::string fileName = fsModel()->fileName(item);
    const std::string fileUrl  = fsModel()->fileUrl(item);

    if (Utils::getFileType(fileName) ==  Utils::FileType::Playlist) {
        m_xmmsClient->playlistAddPlaylistFile(m_xmmsClient->playlistCurrentActive(), fileUrl);
        if (!beQuiet)
            StatusArea::showMessage("Adding \"%s\" playlist file to active playlist", fileName);
    } else {
        m_xmmsClient->playlistAddUrl(m_xmmsClient->playlistCurrentActive(), fileUrl);
        if (!beQuiet)
            StatusArea::showMessage("Adding \"%s\" file to active playlist", fileName);
    }
}

void FileSystemBrowser::activePlaylistPlayItem(int item)
{
    assert(item >= 0 && item < fsModel()->itemsCount());

    const std::string fileName = fsModel()->fileName(item);
    const std::string fileUrl  = fsModel()->fileUrl(item);
    
    if (Utils::getFileType(fileName) == Utils::FileType::Playlist) {
        activePlaylistPlayPlaylistFile(fileUrl);
    } else {
        activePlaylistPlayFile(fileUrl);
    } 
}

void FileSystemBrowser::activePlaylistPlayFile(const std::string& url)
{
    getMedialibIdForFile(url, [this](const xmms2::Expected<int>& id)
    {
        if (id.isValid() && id.value() > 0) {
            m_xmmsClient->playlistPlayId(m_xmmsClient->playlistCurrentActive(), id.value());
        } else {
            StatusArea::showMessage("File not found in medialib and importing failed!");
        }
    });
}

void FileSystemBrowser::activePlaylistPlayPlaylistFile(const std::string& url)
{
    m_xmmsClient->collectionGetIdListFromPlaylistFile(url)([this](const xmms2::Expected<xmms2::Collection>& idlist)
    {
        if (idlist.isError()) {
            NCXMMS2_LOG_ERROR("%s", idlist.error());
            return;
        }
        const int size = idlist->size();
        if (size <= 0)
            return;
        m_xmmsClient->playlistPlayId(m_xmmsClient->playlistCurrentActive(), idlist->at(0));
        
        for (int i = 1; i < size; ++i) {
            m_xmmsClient->playlistAddId(m_xmmsClient->playlistCurrentActive(), idlist->at(i));
        }
    });
}

void FileSystemBrowser::askChangeDirectory()
{
    auto resultCallback = [this](const std::string& path, LineEdit::Result result)
    {
        if (result == LineEdit::Result::Accepted)
            cd(path);
    };
    StatusArea::askQuestion("Change directory: ", resultCallback,
                            m_currentDir.protocol() == "file" ? m_currentDir.path() : m_currentDir.url());
}

void FileSystemBrowser::goUp()
{
    if (!m_currentDir.isRootPath())
        cd("..");
}

void FileSystemBrowser::reloadDirectory()
{
    fsModel()->refresh();
}

void FileSystemBrowser::getFileIdAndShowSongInfo()
{
    const int item = currentItem();
    if (item < 0)
        return;

    if (fsModel()->isDirectory(item))
        return;

    assert(item >= 0 && item < fsModel()->itemsCount());
    const std::string url = fsModel()->fileUrl(item);
    getMedialibIdForFile(url, [this](const xmms2::Expected<int>& id)
    {
        if (id.isValid() && id.value() > 0) {
            showSongInfo(id.value());
        } else {
            StatusArea::showMessage("File not found in medialib and importing failed!");
        }
    });
}

void FileSystemBrowser::getMedialibIdForFile(const std::string& url, const std::function<void (const xmms2::Expected<int>&)>& callback)
{
    m_xmmsClient->medialibGetId(url)([url, callback, this](const xmms2::Expected<int>& id)
    {
        if (id.isError() || id.value() <= 0) {
            m_xmmsClient->medialibAddEntry(url);
            m_xmmsClient->medialibGetId(url)([this, callback](const xmms2::Expected<int>& id)
            {
                callback(id);
            });
            return;
        }
        callback(id);
    });
}

void FileSystemBrowser::toggleSelectionWithoutDotDot(const KeyEvent& keyEvent)
{
    if (fsModel()->fileName(currentItem()) != "..")
        ListViewAppIntegrated::keyPressedEvent(keyEvent);    
}

void FileSystemBrowser::invertSelectionWithoutDotDot(const KeyEvent& keyEvent)
{
    ListView::keyPressedEvent(keyEvent);
    if (fsModel()->fileName(0) == "..") {
        unselectItem(0);
    }
    StatusArea::showMessage("%d items selected", selectedItems().size());    
}

void FileSystemBrowser::selectByRegexpWithoutDotDot()
{
    auto resultCallback = [this](const std::string& pattern, LineEdit::Result result)
    {
        if (result == LineEdit::Result::Accepted) {
            selectItemsByRegExp(pattern);
            if (fsModel()->fileName(0) == "..") {
                unselectItem(0);
            }
            StatusArea::showMessage("%d items selected", selectedItems().size());
        }
    };
    StatusArea::askQuestion("Select items: ", resultCallback, ".*");    
}

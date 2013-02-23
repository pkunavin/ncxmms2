/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <utility>
#include <unistd.h>
#include <assert.h>
#include <boost/cast.hpp>
#include <boost/format.hpp>
#include <xmmsclient/xmmsclient++.h>

#include "localfilesystembrowser.h"
#include "filesystemmodel.h"
#include "filesystemitemdelegate.h"
#include "../statusarea/statusarea.h"
#include "../utils.h"
#include "../xmmsutils.h"
#include "../settings.h"
#include "../hotkeys.h"

#include "../lib/keyevent.h"

using namespace ncxmms2;

LocalFileSystemBrowser::LocalFileSystemBrowser(Xmms::Client    *xmmsClient,
                                               const Rectangle& rect,
                                               Window          *parent) :
    ListViewAppIntegrated(rect, parent),
    m_xmmsClient(xmmsClient),
    m_currentDir("/")
{
    loadPalette("LocalFileSystemBrowser");

    FileSystemModel *fsModel = new FileSystemModel(this);
    setModel(fsModel);
    setItemDelegate(new FileSystemItemDelegate(fsModel));

    const std::string lastPath = Settings::value<std::string>("LocalFileSystemBrowser",
                                                              "lastPath", "/");
    if (!setDirectory(lastPath))
        setDirectory(std::string("/"));

    itemEntered_Connect(&LocalFileSystemBrowser::onItemEntered, this);
}

LocalFileSystemBrowser::~LocalFileSystemBrowser()
{
    Settings::setValue("LocalFileSystemBrowser", "lastPath",
                       m_currentDir.path());
}

void LocalFileSystemBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    FileSystemModel *fsModel =
            boost::polymorphic_downcast<FileSystemModel*>(model());

    switch (keyEvent.key()) {
        case Hotkeys::Screens::LocalFileSystemBrowser::AddFileOrDirectoryToActivePlaylist:
        {
            const std::vector<int>& _selectedItems = selectedItems();
            if (!_selectedItems.empty()) {
                for (int item : _selectedItems) {
                    activePlaylistAddFileOrDirectory(item, true);
                }
                StatusArea::showMessage(
                    (boost::format("Adding %1% items to active playlist")
                                    % _selectedItems.size()).str()
                );
                clearSelection();
            } else {
                const int item = currentItem();
                if (item != -1)
                    activePlaylistAddFileOrDirectory(item);
            }
            break;
        }

        case Hotkeys::Screens::LocalFileSystemBrowser::GoUp:
            if (m_currentDir.path() != "/")
                cd("..");
            break;

        case Hotkeys::Screens::LocalFileSystemBrowser::ChangeDirectory:
        {
            auto resultCallback = [this](const std::string& path,
                                         LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted)
                    cd(path);
            };
            StatusArea::askQuestion("Change directory: ", resultCallback);
            break;
        }

        case KeyEvent::KeyInsert: // Toggle selection
            if (fsModel->fileName(currentItem()) != "..") // Do not select .. item
                ListViewAppIntegrated::keyPressedEvent(keyEvent);
            break;

        case '*': // Invert selection
            ListView::keyPressedEvent(keyEvent);
            if (fsModel->fileName(0) == "..") { // Do not select .. item
                unselectItem(0);
            }
            StatusArea::showMessage(
                (boost::format("%1% items selected") % selectedItems().size()).str()
            );
            break;

        case '+': // Select be regexp
        {
            auto resultCallback = [this, fsModel](const std::string&   pattern,
                                                  LineEdit::ResultCode result)
            {
                if (result == LineEdit::Accepted) {
                    selectItemsByRegExp(pattern);
                    if (fsModel->fileName(0) == "..") { // Do not select .. item
                        unselectItem(0);
                    }
                    StatusArea::showMessage(
                        (boost::format("%1% items selected") % selectedItems().size()).str()
                    );
                }
            };
            StatusArea::askQuestion("Select items: ", resultCallback, ".*");
            break;
        }

        case Hotkeys::Screens::LocalFileSystemBrowser::ReloadDirectory:
            fsModel->refresh();
            break;

        default: ListViewAppIntegrated::keyPressedEvent(keyEvent);
    }
}

bool LocalFileSystemBrowser::setDirectory(const Dir& dir)
{
    if (access(dir.path().c_str(), R_OK) == 0) {
        FileSystemModel *fsModel =
                boost::polymorphic_downcast<FileSystemModel*>(model());
        fsModel->setDirectory(dir.path());
        m_currentDir = dir;

        // FIXME: Path may be too long to display
        setName(std::string("Local file system: ").append(m_currentDir.path()));
        return true;
    }

    StatusArea::showMessage(
        (boost::format("Can't open %1% : %2%") % dir.path()
                                               % strerror(errno)).str()
    );
    return false;
}

void LocalFileSystemBrowser::onItemEntered(int item)
{
    FileSystemModel *fsModel =
            boost::polymorphic_downcast<FileSystemModel*>(model());

    if (fsModel->isDirectory(item)) {
        // FileSystemModel::fileName returns a reference, but next function will
        // modify model content, so we need to copy it
        cd(std::string(fsModel->fileName(item)));
    } else if (fsModel->isRegularFile(item)) {
        //TODO: Play file
        activePlaylistAddFile(item);
    }
}

void LocalFileSystemBrowser::cd(const std::string& dir)
{
    FileSystemModel *fsModel =
            boost::polymorphic_downcast<FileSystemModel*>(model());

    if (dir == "..") { // Cd Up
        const std::string oldDirName = m_currentDir.name();
        if (!setDirectory(Dir(m_currentDir).cd(dir)))
            return;

        if (!m_viewportStateHistory.empty()) {
            const ViewportState& viewportState = m_viewportStateHistory.top();
            setViewportFirstItem(viewportState.viewportFirstItem());

            const int stateCurrentItem = viewportState.currentItem();
            if (stateCurrentItem >= fsModel->itemsCount()
                || fsModel->fileName(stateCurrentItem) != oldDirName) {
                const int index = fsModel->fileIndex(oldDirName);
                if (index != -1)
                    setCurrentItem(index);
            } else {
                setCurrentItem(stateCurrentItem);
            }
            m_viewportStateHistory.pop();
        } else {
            const int index = fsModel->fileIndex(oldDirName);
            if (index != -1)
                setCurrentItem(index);
        }
    } else if (*dir.begin() != '/') { // Subdirectory
        m_viewportStateHistory.emplace(viewportFirstItem(), currentItem());
        if (!setDirectory(Dir(m_currentDir).cd(dir)))
            m_viewportStateHistory.pop();
    } else { // Nor a parent nor sub directory
        // Clear history
        std::stack<ViewportState> empty;
        m_viewportStateHistory.swap(empty);
        setDirectory(Dir(m_currentDir).cd(dir));
    }
}

void LocalFileSystemBrowser::activePlaylistAddFileOrDirectory(int item, bool beQuiet)
{
    FileSystemModel *fsModel =
            boost::polymorphic_downcast<FileSystemModel*>(model());
    assert(item >= 0 && item < fsModel->itemsCount());

    if (fsModel->isDirectory(item)) {
        m_xmmsClient->playlist.addRecursive(
            std::string("file://").append(fsModel->filePath(item))
        );
        if (!beQuiet) {
            StatusArea::showMessage(
                (boost::format("Adding \"%1%\" directory to active playlist")
                                % fsModel->fileName(item)).str()
            );
        }
     } else if (fsModel->isRegularFile(item)) {
        activePlaylistAddFile(item, beQuiet);
    }
}

void LocalFileSystemBrowser::activePlaylistAddFile(int item, bool beQuiet)
{
    FileSystemModel *fsModel =
            boost::polymorphic_downcast<FileSystemModel*>(model());
    assert(item >= 0 && item < fsModel->itemsCount());

    const std::string fileName = fsModel->fileName(item);
    const std::string filePath = std::string("file://").append(fsModel->filePath(item));

    switch (Utils::getFileType(fileName)) {
        case Utils::PlaylistFile:
            XmmsUtils::playlistAddPlaylistFile(m_xmmsClient,
                                               XMMS_ACTIVE_PLAYLIST,
                                               filePath);
            if (!beQuiet) {
                StatusArea::showMessage(
                    (boost::format("Adding \"%1%\" playlist file to active playlist")
                                    % fileName).str()
                );
            }
            break;

        case Utils::MediaFile:
            m_xmmsClient->playlist.addUrl(filePath);
            if (!beQuiet) {
                StatusArea::showMessage(
                    (boost::format("Adding \"%1%\" file to active playlist")
                                    % fileName).str()
                );
            }
            break;

        case Utils::UnknownFile:
            if (!beQuiet)
                StatusArea::showMessage("Unknown file type!");
            break;
    }
}

LocalFileSystemBrowser::Dir& LocalFileSystemBrowser::Dir::cd(const std::string& dir)
{
    if (dir.empty())
        return *this;

    if (dir[0] == '/') {
        m_path = dir;
    } else {
        if (*m_path.rbegin() != '/')
            m_path.push_back('/');
        m_path.append(dir);
    }

    char *canonicalPath = canonicalize_file_name(m_path.c_str());
    if (canonicalPath)
        m_path = canonicalPath;
    free(canonicalPath);

    if (*m_path.rbegin() == '/' && m_path.size() > 1)
        m_path.resize(m_path.size() - 1);

    return *this;
}

std::string LocalFileSystemBrowser::Dir::name() const
{
    const std::string::size_type slashPos = m_path.rfind('/');
    if (slashPos == std::string::npos || slashPos + 1 >= m_path.size())
        return std::string();

    return std::move(m_path.substr(slashPos + 1));
}


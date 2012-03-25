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

#include <cstring>
#include <algorithm>
#include <set>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <boost/format.hpp>
#include <xmmsclient/xmmsclient++.h>

#include "localfilesystembrowser.h"
#include "statusarea.h"
#include "settings.h"

#include "lib/painter.h"
#include "lib/keyevent.h"

using namespace ncxmms2;

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

    return *this;
}

std::string LocalFileSystemBrowser::Dir::name() const
{
    const std::string::size_type slashPos = m_path.rfind('/');
    if (slashPos == std::string::npos || slashPos + 1 >= m_path.size())
        return std::string();

    return m_path.substr(slashPos + 1);
}

LocalFileSystemBrowser::LocalFileSystemBrowser(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent) :
    AbstractItemView(lines, cols, yPos, xPos, parent),
    m_xmmsClient(xmmsClient),
    m_currentDir("/")
{
    const std::string lastPath = Settings::value<std::string>("LocalFileSystemBrowser", "lastPath", "/");
    m_currentDir.cd(lastPath);
    if (!setCurrentDir(m_currentDir)) {
        m_currentDir.cd("/");
        setCurrentDir(m_currentDir);
    }
}

LocalFileSystemBrowser::~LocalFileSystemBrowser()
{
    Settings::setValue("LocalFileSystemBrowser", "lastPath", m_currentDir.path());
}

bool LocalFileSystemBrowser::setCurrentDir(const LocalFileSystemBrowser::Dir& dir)
{
    const char *currentPath = dir.path().c_str();
    DIR *newDir = opendir(currentPath);
    if (!newDir)
        return false;

    m_currentDirEntries.clear();
    if (dir.path() != "/")
        m_currentDirEntries.push_back(FileSystemItem("..", FileSystemItem::ItemDirectory));

    struct dirent *dirEntry;
    while ((dirEntry = readdir(newDir))) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        FileSystemItem::ItemType itemType;
        switch(dirEntry->d_type) {
            case DT_REG     : itemType = FileSystemItem::ItemFile;      break;
            case DT_DIR     : itemType = FileSystemItem::ItemDirectory; break;
            case DT_UNKNOWN : //TODO call stat
            default         : itemType = FileSystemItem::ItemOther;     break;
        }
        m_currentDirEntries.push_back(FileSystemItem(dirEntry->d_name, itemType));
    }
    closedir(newDir);

    std::sort(m_currentDirEntries.begin() + (dir.path() != "/" ? 1 : 0), m_currentDirEntries.end(),
              [](const FileSystemItem& item1, const FileSystemItem& item2)
              {
                  if (item1.isDirectory() && !item2.isDirectory())
                      return true;

                  if (!item1.isDirectory() && item2.isDirectory())
                      return false;

                  return item1.name()<item2.name();
              });

    m_currentDir = dir;
    setTitle(std::string("Local file system: ").append(m_currentDir.path()));
    reset();
    return true;
}

std::string LocalFileSystemBrowser::filePath(int item) const
{
    std::string path = m_currentDir.path();
    if (*path.rbegin() != '/')
        path.push_back('/');

    return path.append(m_currentDirEntries[item].name());
}

void LocalFileSystemBrowser::cd(const std::string& dir)
{
    if (dir == ".." && m_currentDir.path() != "/") {
        std::string currentDirName = m_currentDir.name();
        setCurrentDir(Dir(m_currentDir).cd(dir));

        if (!m_viewportStateHistory.empty()) {
            const ViewportState& viewportState = m_viewportStateHistory.top();
            setViewportFirstItem(viewportState.viewportBegin());

            std::vector<FileSystemItem>::size_type stateCurrentItem = viewportState.currentItem();
            if (stateCurrentItem >= m_currentDirEntries.size()
                || m_currentDirEntries[stateCurrentItem].name() != currentDirName) {

                auto it = std::find_if(m_currentDirEntries.begin(), m_currentDirEntries.end(),
                                      [&currentDirName](const FileSystemItem& item)
                                      {
                                          return item.name() == currentDirName;
                                      });
                setCurrentItem(it != m_currentDirEntries.end()
                               ? it - m_currentDirEntries.begin()
                               : 0);

            } else {
                setCurrentItem(viewportState.currentItem());
            }

            m_viewportStateHistory.pop();
        } else {
            auto it=std::find_if(m_currentDirEntries.begin(), m_currentDirEntries.end(),
                                 [&currentDirName](const FileSystemItem& item)
                                 {
                                     return item.name() == currentDirName;
                                 });
            setCurrentItem(it != m_currentDirEntries.end()
                           ? it - m_currentDirEntries.begin()
                           : 0);
        }
    } else {
        m_viewportStateHistory.push(ViewportState(viewportFirstItem(), currentItem()));
        if (!setCurrentDir(Dir(m_currentDir).cd(dir)))
            m_viewportStateHistory.pop();
    }

}

void LocalFileSystemBrowser::drawItem(int item)
{
    Painter painter(this);

    if (item == currentItem()) {
        painter.fillLine(itemLine(item), ColorYellow);
        painter.setReverse(true);
    } else {
        painter.clearLine(itemLine(item));
    }

    painter.setColor(ColorYellow);
    if (m_currentDirEntries[item].isDirectory()) {
        if (item != currentItem())
            painter.setBold(true);
        painter.squeezedPrint(std::string("/").append(m_currentDirEntries[item].name()), cols());
    } else {
        painter.squeezedPrint(m_currentDirEntries[item].name(), cols());
    }
}

int LocalFileSystemBrowser::itemsCount() const
{
    return m_currentDirEntries.size();
}

void LocalFileSystemBrowser::itemEntered(int item)
{
    switch(m_currentDirEntries[item].type()) {
        case FileSystemItem::ItemDirectory:
            cd(m_currentDirEntries[item].name());
            break;

        case FileSystemItem::ItemFile:
            //TODO Play item
            if (isPlaylistFile(m_currentDirEntries[item].name())) {
                addPlaylistFile(std::string("file://").append(filePath(item)));
            } else {
                m_xmmsClient->playlist.addUrl(std::string("file://").append(filePath(item)));
            }
            StatusArea::showMessage((boost::format("Adding %1% to active playlist")
                                     % m_currentDirEntries[item].name()).str());
            break;

        case FileSystemItem::ItemOther:
            break;
    }
}

void LocalFileSystemBrowser::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case ' ':
        {
            const int item = currentItem();
            if (m_currentDirEntries[item].isDirectory()) {
                m_xmmsClient->playlist.addRecursive(std::string("file://").append(filePath(item)));
                StatusArea::showMessage((boost::format("Adding %1% directory to active playlist")
                                         % m_currentDirEntries[item].name()).str());
            } else if (m_currentDirEntries[item].isFile()) {
                if (isPlaylistFile(m_currentDirEntries[item].name())) {
                    addPlaylistFile(std::string("file://").append(filePath(item)));
                } else {
                    m_xmmsClient->playlist.addUrl(std::string("file://").append(filePath(item)));
                }
                StatusArea::showMessage((boost::format("Adding %1% to active playlist")
                                         % m_currentDirEntries[item].name()).str());
            }
            break;
        }

        case KeyEvent::KeyLeft:
            cd("..");
            break;

        default:
            AbstractItemView::keyPressedEvent(keyEvent);
    }
}

bool LocalFileSystemBrowser::isPlaylistFile(const std::string& fileName) const
{
    const std::string::size_type dotPos = fileName.rfind('.');
    if (dotPos == std::string::npos || dotPos + 1 >= fileName.size() || dotPos == 0)
        return false;

    const std::string suffix = fileName.substr(dotPos + 1);

    static const std::set<std::string> playlistFileSuffixes{"cue", "m3u", "pls", "asx"};
    return playlistFileSuffixes.find(suffix) != playlistFileSuffixes.end();
}

void LocalFileSystemBrowser::addPlaylistFile(const std::string& fileName)
{
    m_xmmsClient->collection.idlistFromPlaylistFile(fileName)(Xmms::bind(&LocalFileSystemBrowser::addIdList, this));
}

bool LocalFileSystemBrowser::addIdList(const Xmms::Coll::Coll& idlist)
{
    m_xmmsClient->playlist.addIdlist(idlist);
    return true;
}

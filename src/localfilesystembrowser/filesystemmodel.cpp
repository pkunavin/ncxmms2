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
#include <algorithm>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>

#include "filesystemmodel.h"

#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

FileSystemModel::FileSystemModel(Object *parent) :
    ListModel(parent)
{

}

void FileSystemModel::setDirectory(const std::string& path)
{
    m_currentDir = path;
    refresh();
}

const std::string& FileSystemModel::fileName(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].name;
}

std::string FileSystemModel::filePath(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());

    std::string path = m_currentDir;
    if (*path.rbegin() != '/')
        path.push_back('/');
    path.append(m_currentDirEntries[item].name);

    return std::move(path);
}

bool FileSystemModel::isDirectory(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isDirectory();
}

bool FileSystemModel::isRegularFile(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isRegularFile();
}

bool FileSystemModel::isBlockFile(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isBlockFile();
}

bool FileSystemModel::isCharacterFile(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isCharacterFile();
}

bool FileSystemModel::isFifoFile(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isFifoFile();
}

bool FileSystemModel::isSymbolicLink(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isSymbolicLink();
}

bool FileSystemModel::isSocketFile(int item) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    return m_currentDirEntries[item].isSocketFile();
}

int FileSystemModel::fileIndex(const std::string& name) const
{
    auto pred = [&name](const FileSystemItem& item)
    {
        return item.name == name;
    };
    auto it = std::find_if(m_currentDirEntries.begin(), m_currentDirEntries.end(),
                           pred);
    return it != m_currentDirEntries.end() ? it - m_currentDirEntries.begin() : -1;
}

void FileSystemModel::data(int item, ListModelItemData *itemData) const
{
    assert(item >= 0 && (size_t)item < m_currentDirEntries.size());
    itemData->textPtr = &m_currentDirEntries[item].name;
}

int FileSystemModel::itemsCount() const
{
    return m_currentDirEntries.size();
}

void FileSystemModel::refresh()
{
    // TODO: Automatic refresh via inotify on Linux

    m_currentDirEntries.clear();

    DIR *newDir = opendir(m_currentDir.c_str());
    if (!newDir) {
        reset();
        return;
    }

    std::string fileDir = m_currentDir;
    if (*fileDir.rbegin() != '/')
        fileDir.push_back('/');
    const std::string::size_type fileDirStrSize = fileDir.size();

    // Explicitly add .. item, because directory stream may not contain it.
    if (!isRootPath(m_currentDir)) {
        m_currentDirEntries.emplace_back("..");
        fileDir.append("..");
        struct stat64 *info = &m_currentDirEntries.rbegin()->info;
        stat64(fileDir.c_str(), info); // FIXME: What to do if it fails?
    }

    struct dirent *dirEntry;
    while ((dirEntry = readdir(newDir))) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        m_currentDirEntries.emplace_back(dirEntry->d_name);
        struct stat64 *info = &m_currentDirEntries.rbegin()->info;
        fileDir.resize(fileDirStrSize);
        fileDir.append(dirEntry->d_name);
        if (stat64(fileDir.c_str(), info) == -1) {
            m_currentDirEntries.pop_back();
        }
    }
    closedir(newDir);

    auto itBegin = m_currentDirEntries.begin();
    if (!isRootPath(m_currentDir) && !m_currentDirEntries.empty())
        ++itBegin; // Skip .. item

    auto fileCompare = [](const FileSystemItem& item1, const FileSystemItem& item2)
    {
        if (item1.isDirectory() && !item2.isDirectory())
            return true;

        if (!item1.isDirectory() && item2.isDirectory())
            return false;

        return item1.name < item2.name;
    };

    std::sort(itBegin, m_currentDirEntries.end(), fileCompare);
    reset();
}

bool FileSystemModel::isRootPath(const std::string& path)
{
    if (path == "/")
        return true;

    char *canonicalPath = canonicalize_file_name(path.c_str());
    const bool res = strcmp(canonicalPath, "/") == 0;
    free(canonicalPath);
    return res;
}

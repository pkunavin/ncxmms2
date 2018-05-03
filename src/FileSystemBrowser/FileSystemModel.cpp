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
#include <utility>
#include <algorithm>
#include <dirent.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "FileSystemModel.h"
#include "Dir.h"
#include "FileSystemWatcher.h"

#include "../lib/ListModelItemData.h"
#include "../lib/StringAlgo.h"

namespace ncxmms2 {

class FileSystemModelPrivate
{
public:
    explicit FileSystemModelPrivate(FileSystemModel *q_);
    
    FileSystemModelPrivate(const FileSystemModelPrivate & other) = delete;
    FileSystemModelPrivate& operator=(const FileSystemModelPrivate & other) = delete;

    FileSystemModel *q;
    FileSystemWatcher *m_fsWatcher;
    Dir m_dir;

    struct FileSystemItem
    {
        template <typename T>
        explicit FileSystemItem(T&& name_) : name(std::forward<T>(name_)) {}

        bool isDirectory() const     {return S_ISDIR(info.st_mode);}
        bool isRegularFile() const   {return S_ISREG(info.st_mode);}
        bool isBlockFile() const     {return S_ISBLK(info.st_mode);}
        bool isCharacterFile() const {return S_ISCHR(info.st_mode);}
        bool isFifoFile() const      {return S_ISFIFO(info.st_mode);}
        bool isSymbolicLink() const  {return S_ISLNK(info.st_mode);}
        bool isSocketFile() const    {return S_ISSOCK(info.st_mode);}

        std::string name;
        struct stat64 info;
        
        friend bool operator<(const FileSystemItem& item1, const FileSystemItem& item2)
        {
            if (item1.name == "..")
                return true;
            
            if (item2.name == "..")
                return false;

            if (item1.isDirectory() && !item2.isDirectory())
                return true;
    
            if (!item1.isDirectory() && item2.isDirectory())
                return false;
    
            return item1.name < item2.name;
        }
    };
    std::vector<FileSystemItem> m_dirEntries;

    void loadDirectory(const Dir& dir);
    
    struct FindFileCmp
    {
        bool operator()(const FileSystemItem& item1, const std::string& item2)
        {
            if (item1.name == "..")
                return true;
            
            if (item2 == "..")
                return false;

            if (item1.isDirectory())
                return true;
    
           return item1.name < item2;
        }
    };
    
    struct FindDirCmp
    {
        bool operator()(const FileSystemItem& item1, const std::string& item2)
        {
            if (item1.name == "..")
                return true;
            
            if (item2 == "..")
                return false;

            if (!item1.isDirectory())
                return false;
    
           return item1.name < item2;
        }
    };
    
    template <typename Cmp>
    void itemCreated(const std::string& file)
    {
        auto it = std::lower_bound(m_dirEntries.begin(), m_dirEntries.end(), file, Cmp());
        it = m_dirEntries.emplace(it, file);
        
        std::string filePatch = m_dir.path();
        if (!endsWith(filePatch, '/'))
            filePatch.push_back('/');
        filePatch.append(file);
        if (stat64(filePatch.c_str(), &it->info) == -1) {
            m_dirEntries.erase(it);
            return;
        }
        
        q->itemInserted(it - m_dirEntries.begin());
    }
    
    template <typename Cmp>
    void itemRemoved(const std::string& file)
    {
        auto it = std::lower_bound(m_dirEntries.begin(), m_dirEntries.end(), file, Cmp());
        if (it != m_dirEntries.end() && it->name == file) {
            m_dirEntries.erase(it);
            q->itemRemoved(it - m_dirEntries.begin());
        }
    }
};

} // ncxmms2

using namespace ncxmms2;

FileSystemModel::FileSystemModel(Object *parent) :
    AbstractFileSystemModel(parent),
    d(new FileSystemModelPrivate(this))
{
    
}

FileSystemModel::~FileSystemModel()
{
    
}

void FileSystemModel::setDirectory(const Dir& dir)
{
    d->loadDirectory(dir);
}

const Dir& FileSystemModel::directory() const
{
    return d->m_dir;
}

const std::string& FileSystemModel::fileName(int item) const
{
    assert(item >= 0 && (size_t)item < d->m_dirEntries.size());
    return d->m_dirEntries[item].name;
}

std::string FileSystemModel::fileUrl(int item) const
{
    assert(item >= 0 && (size_t)item < d->m_dirEntries.size());
    // NOTE: Item may not be a directory, but Dir class doesn't care ...
    return Dir(d->m_dir).cd(d->m_dirEntries[item].name).url();
}

bool FileSystemModel::isDirectory(int item) const
{
    assert(item >= 0 && (size_t)item < d->m_dirEntries.size());
    return d->m_dirEntries[item].isDirectory();
}

int FileSystemModel::fileIndex(const std::string& name) const
{
    auto it = std::lower_bound(d->m_dirEntries.begin(), d->m_dirEntries.end(),
                               name, FileSystemModelPrivate::FindDirCmp());
    if (it != d->m_dirEntries.end() && it->name == name) {
        return it - d->m_dirEntries.begin();
    }
    
    it = std::lower_bound(d->m_dirEntries.begin(), d->m_dirEntries.end(),
                          name, FileSystemModelPrivate::FindFileCmp());
    if (it != d->m_dirEntries.end() && it->name == name) {
        return it - d->m_dirEntries.begin();
    }
    
    return -1;
}

void FileSystemModel::data(int item, ListModelItemData *itemData) const
{
    assert(item >= 0 && (size_t)item < d->m_dirEntries.size());
    itemData->textPtr = &d->m_dirEntries[item].name;
}

int FileSystemModel::itemsCount() const
{
    return d->m_dirEntries.size();
}

void FileSystemModel::refresh()
{
    d->loadDirectory(d->m_dir);
}

FileSystemModelPrivate::FileSystemModelPrivate(FileSystemModel *q_) :
    q(q_),
    m_dir("/")
{
    m_fsWatcher = new FileSystemWatcher(q);
    m_fsWatcher->fileCreated_Connect(&FileSystemModelPrivate::itemCreated<FindFileCmp>, this);
    m_fsWatcher->directoryCreated_Connect(&FileSystemModelPrivate::itemCreated<FindDirCmp>, this);
    m_fsWatcher->fileDeleted_Connect(&FileSystemModelPrivate::itemRemoved<FindFileCmp>, this);
    m_fsWatcher->directoryDeleted_Connect(&FileSystemModelPrivate::itemRemoved<FindDirCmp>, this);
}

void FileSystemModelPrivate::loadDirectory(const Dir& dir)
{
    DIR *dirStream = opendir(dir.path().c_str());
    if (!dirStream) {
        q->directoryLoadFailed(dir, std::strerror(errno));
        return;
    }

    m_dir = dir;
    m_dirEntries.clear();
    
    std::string filePath = m_dir.path();
    if (!endsWith(filePath, '/'))
        filePath.push_back('/');
    const auto filePathStrSize = filePath.size();

    // Explicitly add .. item, because directory stream may not contain it.
    if (!m_dir.isRootPath()) {
        m_dirEntries.emplace_back("..");
        filePath.append("..");
        struct stat64 *info = &m_dirEntries.back().info;
        stat64(filePath.c_str(), info); // FIXME: What to do if it fails?
    }

    struct dirent *dirEntry;
    while ((dirEntry = readdir(dirStream))) {
        if (stringsEqual(dirEntry->d_name, ".") || stringsEqual(dirEntry->d_name, ".."))
            continue;

        m_dirEntries.emplace_back(dirEntry->d_name);
        struct stat64 *info = &m_dirEntries.back().info;
        filePath.resize(filePathStrSize);
        filePath.append(dirEntry->d_name);
        if (stat64(filePath.c_str(), info) == -1) {
            m_dirEntries.pop_back();
        }
    }
    closedir(dirStream);

    std::sort(m_dirEntries.begin(), m_dirEntries.end());
    m_fsWatcher->watch(m_dir.path());
    q->reset();
    q->directoryLoaded(m_dir);
}

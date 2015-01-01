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
#include <utility>
#include <algorithm>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "filesystemmodel.h"
#include "filesystemwatcher.h"

#include "../lib/listmodelitemdata.h"
#include "../lib/stringalgo.h"

namespace ncxmms2 {

class FileSystemModelPrivate
{
public:
    FileSystemModelPrivate(FileSystemModel *q_);
    
    FileSystemModel *q;
    FileSystemWatcher *fsWatcher;
    std::string dir;

    struct FileSystemItem
    {
        template <typename T>
        FileSystemItem(T&& name_) : name(std::forward<T>(name_)) {}

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
            if (item1.isDirectory() && !item2.isDirectory())
                return true;
    
            if (!item1.isDirectory() && item2.isDirectory())
                return false;
    
            return item1.name < item2.name;
        }
    };
    std::vector<FileSystemItem> dirEntries;

    struct FindFileCmp
    {
        bool operator()(const FileSystemItem& item1, const std::string& item2)
        {
            if (item1.isDirectory())
                return true;
    
           return item1.name < item2;
        }
    };
    
    struct FindDirCmp
    {
        bool operator()(const FileSystemItem& item1, const std::string& item2)
        {
            if (!item1.isDirectory())
                return false;
    
           return item1.name < item2;
        }
    };
    
    template <typename Cmp>
    void itemCreated(const std::string& file)
    {
        auto it = std::lower_bound(dirEntries.begin(), dirEntries.end(), file, Cmp());
        it = dirEntries.emplace(it, file);
        
        std::string filePatch = dir;
        if (!endsWith(filePatch, '/'))
            filePatch.push_back('/');
        filePatch.append(file);
        if (stat64(filePatch.c_str(), &it->info) == -1) {
            dirEntries.erase(it);
            return;
        }
        
        q->itemInserted(it - dirEntries.begin());
    }
    
    template <typename Cmp>
    void itemRemoved(const std::string& file)
    {
        auto it = std::lower_bound(dirEntries.begin(), dirEntries.end(), file, Cmp());
        if (it != dirEntries.end() && it->name == file) {
            dirEntries.erase(it);
            q->itemRemoved(it - dirEntries.begin());
        }
    }
    
    static bool isRootPath(const std::string& path);
};

} // ncxmms2

using namespace ncxmms2;

FileSystemModel::FileSystemModel(Object *parent) :
    ListModel(parent),
    d(new FileSystemModelPrivate(this))
{
    
}

FileSystemModel::~FileSystemModel()
{
    
}

void FileSystemModel::setDirectory(const std::string& path)
{
    d->dir = path;
    d->fsWatcher->watch(path);
    refresh();
}

const std::string& FileSystemModel::fileName(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].name;
}

std::string FileSystemModel::filePath(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());

    std::string path = d->dir;
    if (!endsWith(path, '/'))
        path.push_back('/');
    path.append(d->dirEntries[item].name);

    return path;
}

bool FileSystemModel::isDirectory(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isDirectory();
}

bool FileSystemModel::isRegularFile(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isRegularFile();
}

bool FileSystemModel::isBlockFile(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isBlockFile();
}

bool FileSystemModel::isCharacterFile(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isCharacterFile();
}

bool FileSystemModel::isFifoFile(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isFifoFile();
}

bool FileSystemModel::isSymbolicLink(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isSymbolicLink();
}

bool FileSystemModel::isSocketFile(int item) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    return d->dirEntries[item].isSocketFile();
}

int FileSystemModel::fileIndex(const std::string& name) const
{
    auto pred = [&name](const FileSystemModelPrivate::FileSystemItem& item)
    {
        return item.name == name;
    };
    auto it = std::find_if(d->dirEntries.begin(), d->dirEntries.end(), pred);
    return it != d->dirEntries.end() ? it - d->dirEntries.begin() : -1;
}

void FileSystemModel::data(int item, ListModelItemData *itemData) const
{
    assert(item >= 0 && (size_t)item < d->dirEntries.size());
    itemData->textPtr = &d->dirEntries[item].name;
}

int FileSystemModel::itemsCount() const
{
    return d->dirEntries.size();
}

void FileSystemModel::refresh()
{
    d->dirEntries.clear();

    DIR *newDir = opendir(d->dir.c_str());
    if (!newDir) {
        reset();
        return;
    }

    std::string filePath = d->dir;
    if (!endsWith(filePath, '/'))
        filePath.push_back('/');
    const auto filePathStrSize = filePath.size();

    // Explicitly add .. item, because directory stream may not contain it.
    if (!FileSystemModelPrivate::isRootPath(d->dir)) {
        d->dirEntries.emplace_back("..");
        filePath.append("..");
        struct stat64 *info = &d->dirEntries.back().info;
        stat64(filePath.c_str(), info); // FIXME: What to do if it fails?
    }

    struct dirent *dirEntry;
    while ((dirEntry = readdir(newDir))) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        d->dirEntries.emplace_back(dirEntry->d_name);
        struct stat64 *info = &d->dirEntries.back().info;
        filePath.resize(filePathStrSize);
        filePath.append(dirEntry->d_name);
        if (stat64(filePath.c_str(), info) == -1) {
            d->dirEntries.pop_back();
        }
    }
    closedir(newDir);

    auto itBegin = d->dirEntries.begin();
    if (!FileSystemModelPrivate::isRootPath(d->dir) && !d->dirEntries.empty())
        ++itBegin; // Skip .. item

    std::sort(itBegin, d->dirEntries.end());
    reset();
}

FileSystemModelPrivate::FileSystemModelPrivate(FileSystemModel *q_) :
    q(q_)
{
    fsWatcher = new FileSystemWatcher(q);
    fsWatcher->fileCreated_Connect(&FileSystemModelPrivate::itemCreated<FindFileCmp>, this);
    fsWatcher->directoryCreated_Connect(&FileSystemModelPrivate::itemCreated<FindDirCmp>, this);
    fsWatcher->fileDeleted_Connect(&FileSystemModelPrivate::itemRemoved<FindFileCmp>, this);
    fsWatcher->directoryDeleted_Connect(&FileSystemModelPrivate::itemRemoved<FindDirCmp>, this);
    fsWatcher->selfDeleted_Connect([this](){q->deleted();});
}

bool FileSystemModelPrivate::isRootPath(const std::string& path)
{
    if (path == "/")
        return true;

    char *canonicalPath = canonicalize_file_name(path.c_str());
    const bool res = strcmp(canonicalPath, "/") == 0;
    free(canonicalPath);
    return res;
}

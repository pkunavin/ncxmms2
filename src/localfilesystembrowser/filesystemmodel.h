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

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <vector>
#include <sys/stat.h>

#include "../lib/listmodel.h"

namespace ncxmms2 {

class FileSystemModel : public ListModel
{
public:
    FileSystemModel(Object *parent = nullptr);

    void setDirectory(const std::string& path);

    const std::string& fileName(int item) const;
    std::string filePath(int item) const;
    bool isDirectory(int item) const;
    bool isRegularFile(int item) const;
    bool isBlockFile(int item) const;
    bool isCharacterFile(int item) const;
    bool isFifoFile(int item) const;
    bool isSymbolicLink(int item) const;
    bool isSocketFile(int item) const;

    int fileIndex(const std::string& name) const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;

    virtual void refresh();

private:
    std::string m_currentDir;

    struct FileSystemItem
    {
        FileSystemItem(const char *name_) : name(name_){}

        bool isDirectory() const     {return S_ISDIR(info.st_mode);}
        bool isRegularFile() const   {return S_ISREG(info.st_mode);}
        bool isBlockFile() const     {return S_ISBLK(info.st_mode);}
        bool isCharacterFile() const {return S_ISCHR(info.st_mode);}
        bool isFifoFile() const      {return S_ISFIFO(info.st_mode);}
        bool isSymbolicLink() const  {return S_ISLNK(info.st_mode);}
        bool isSocketFile() const    {return S_ISSOCK(info.st_mode);}

        std::string name;
        struct stat64 info;
    };
    std::vector<FileSystemItem> m_currentDirEntries;

    static bool isRootPath(const std::string& path);

};
} // ncxmms2

#endif // FILESYSTEMMODEL_H

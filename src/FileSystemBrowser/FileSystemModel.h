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

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include "AbstractFileSystemModel.h"

namespace ncxmms2 {

class FileSystemModelPrivate;

class FileSystemModel : public AbstractFileSystemModel
{
public:
    explicit FileSystemModel(Object *parent = nullptr);
    ~FileSystemModel();
    
    virtual void setDirectory(const Dir& dir);
    virtual const Dir& directory() const;

    virtual const std::string& fileName(int item) const;
    virtual std::string fileUrl(int item) const;
    virtual bool isDirectory(int item) const;
    
    int fileIndex(const std::string& name) const;

    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;
    virtual void refresh();
    
private:
    std::unique_ptr<FileSystemModelPrivate> d;
    friend class FileSystemModelPrivate;
};
} // ncxmms2

#endif // FILESYSTEMMODEL_H

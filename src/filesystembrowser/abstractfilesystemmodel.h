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

#ifndef ABSTRACTFILESYSTEMMODEL_H
#define ABSTRACTFILESYSTEMMODEL_H

#include "../lib/listmodel.h"

namespace ncxmms2 {

class Dir;

class AbstractFileSystemModel : public ListModel
{
public:
    AbstractFileSystemModel(Object *parent = nullptr) : ListModel(parent) {}
    
    virtual void setDirectory(const Dir& dir) = 0;
    virtual const Dir& directory() const = 0;
    
    virtual const std::string& fileName(int item) const = 0;
    virtual std::string fileUrl(int item) const = 0;
    virtual int fileIndex(const std::string& name) const = 0;
    virtual bool isDirectory(int item) const = 0;
    
    // Signals
    NCXMMS2_SIGNAL(directoryLoaded, const Dir&)
    NCXMMS2_SIGNAL(directoryLoadFailed, const Dir&, const std::string& /* error */)
};
} // ncxmms2

#endif // ABSTRACTFILESYSTEMMODEL_H

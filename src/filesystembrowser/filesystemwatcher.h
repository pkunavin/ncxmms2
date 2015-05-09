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

#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include "../lib/object.h"

namespace ncxmms2 {

class FileSystemWatcherPrivate;

class FileSystemWatcher : public Object
{
public:
    explicit FileSystemWatcher(Object *parent);
    ~FileSystemWatcher();
    
    void watch(const std::string& path);
    
    // Signals
    NCXMMS2_SIGNAL(selfDeleted)
    NCXMMS2_SIGNAL(fileDeleted, const std::string&)
    NCXMMS2_SIGNAL(directoryDeleted, const std::string&)
    NCXMMS2_SIGNAL(fileCreated, const std::string&)
    NCXMMS2_SIGNAL(directoryCreated, const std::string&)
    
private:
    std::unique_ptr<FileSystemWatcherPrivate> d;
    friend class FileSystemWatcherPrivate;
};

} // ncxmms2

#endif // FILESYSTEMWATCHER_H

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

#ifndef SERVERSIDEBROWSERMODEL_H
#define SERVERSIDEBROWSERMODEL_H

#include <vector>

#include "abstractfilesystemmodel.h"
#include "dir.h"
#include "../xmmsutils/client.h"

namespace ncxmms2 {

class ServerSideBrowserModel : public AbstractFileSystemModel
{
public:
    ServerSideBrowserModel(xmms2::Client *xmmsClient, Object *parent = nullptr);
    ~ServerSideBrowserModel();
    
    virtual void setDirectory(const Dir& dir);
    virtual const Dir& directory() const;
    
    virtual const std::string& fileName(int item) const;
    virtual std::string fileUrl(int item) const;
    virtual bool isDirectory(int item) const;
    
    virtual int fileIndex(const std::string& name) const;
    
    virtual void data(int item, ListModelItemData *itemData) const;
    virtual int itemsCount() const;
    virtual void refresh();
    
private:
    xmms2::Client *m_xmmsClient;
    Dir m_dir;
    
    struct Item
    {
        std::string name;
        bool isDir;
        
        template <typename Str>
        Item(Str&& name, bool isDir_) : name(std::forward<Str>(name)), isDir(isDir_) {}
        
        friend bool operator<(const Item& item1, const Item& item2)
        {
            if (item1.name == "..")
                return true;
            
            if (item1.isDir && !item2.isDir)
                return true;
    
            if (!item1.isDir && item2.isDir)
                return false;
    
            return item1.name < item2.name;
        }
    };
    std::vector<Item> m_items;
        
    void getDirectoryItems(const Dir& dir, const xmms2::Expected<xmms2::List<xmms2::Dict>>& list);
};
} // ncxmms2

#endif // SERVERSIDEBROWSERMODEL_H

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

#include <algorithm>
#include "serversidebrowsermodel.h"
#include "../lib/listmodelitemdata.h"

using namespace ncxmms2;

ServerSideBrowserModel::ServerSideBrowserModel(xmms2::Client *xmmsClient, Object *parent) :
    AbstractFileSystemModel(parent),
    m_xmmsClient(xmmsClient),
    m_dir("/")
{
    
}

ServerSideBrowserModel::~ServerSideBrowserModel()
{
    
}

void ServerSideBrowserModel::setDirectory(const Dir& dir)
{
    m_xmmsClient->xformMediaBrowse(dir.url())(&ServerSideBrowserModel::getDirectoryItems, this,
                                              dir, std::placeholders::_1);
}

const std::string& ServerSideBrowserModel::fileName(int item) const
{
    assert(item >= 0 && (size_t)item < m_items.size());
    return m_items[item].name;
}

std::string ServerSideBrowserModel::fileUrl(int item) const
{
    assert(item >= 0 && (size_t)item < m_items.size());
    // NOTE: Item may not be a directory, but Dir class doesn't care ...
    return Dir(m_dir).cd(m_items[item].name).url();
}

bool ServerSideBrowserModel::isDirectory(int item) const
{
    assert(item >= 0 && (size_t)item < m_items.size());
    return m_items[item].isDir;
}

int ServerSideBrowserModel::fileIndex(const std::string& name) const
{
    Item item(name, true);
    auto it = std::lower_bound(m_items.begin(), m_items.end(), item);
    if (it != m_items.end() && it->name == name) {
        return it - m_items.begin();
    }
    
    item.isDir = false;
    it = std::lower_bound(m_items.begin(), m_items.end(), item);
    if (it != m_items.end() && it->name == name) {
        return it - m_items.begin();
    }
    
    return -1;
}

const Dir& ServerSideBrowserModel::directory() const
{
    return m_dir;
}

void ServerSideBrowserModel::data(int item, ListModelItemData *itemData) const
{
    itemData->textPtr = &m_items[item].name;
}

int ServerSideBrowserModel::itemsCount() const
{
    return m_items.size();
}

void ServerSideBrowserModel::refresh()
{
    setDirectory(m_dir);
}

void ServerSideBrowserModel::getDirectoryItems(const Dir& dir,
                                               const xmms2::Expected<xmms2::List<xmms2::Dict>>& list)
{
    if (list.isError()) {
        directoryLoadFailed(dir, list.error().toString());
        return;
    }
    
    m_dir = dir;
    m_items.clear();
    
    // Explicitly add .. item
    if (!m_dir.isRootPath()) {
        m_items.emplace_back("..", true);
    }
    
    for (auto it = list->getIterator(); it.isValid(); it.next()) {
        bool ok = false;
        xmms2::Dict dict = it.value(&ok);
        if (NCXMMS2_UNLIKELY(!ok))
            continue;
        
        StringRef path = dict.value<StringRef>("path");
        if (NCXMMS2_UNLIKELY(path.isNull()))
            continue;
        
        if (NCXMMS2_UNLIKELY(path == "." || path == ".."))
            continue;
        
        bool isDir = dict.value<int>("isdir", 0);
        m_items.emplace_back(xmms2::getFileNameFromUrl(xmms2::decodeUrl(path.c_str())), isDir);
    }
    
    std::sort(m_items.begin(), m_items.end());
    
    reset();
    directoryLoaded(m_dir);
}

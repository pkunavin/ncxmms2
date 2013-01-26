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

#ifndef LOCALFILESYSTEMBROWSER_H
#define LOCALFILESYSTEMBROWSER_H

#include <stack>
#include "../listviewappintegrated/listviewappintegrated.h"

namespace Xmms {
class Client;
}

namespace ncxmms2 {

class LocalFileSystemBrowser : public ListViewAppIntegrated
{
public:
    LocalFileSystemBrowser(Xmms::Client    *xmmsClient,
                           const Rectangle& rect,
                           Window          *parent = nullptr);
    ~LocalFileSystemBrowser();

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

private:
    Xmms::Client *m_xmmsClient;

    class Dir
    {
    public:
        Dir(const std::string& path) :
            m_path(path) {}

        const std::string& path() const {return m_path;}
        std::string name() const;
        Dir& cd(const std::string& dir);

    private:
        std::string m_path;
    };
    Dir m_currentDir;

    bool setDirectory(const Dir& dir);
    void onItemEntered(int item);
    void cd(const std::string& dir);
    void activePlaylistAddFileOrDirectory(int item, bool beQuiet = false);
    void activePlaylistAddFile(int item, bool beQuiet = false);

    class ViewportState
    {
    public:
        ViewportState(int viewportFirstItem, int currentItem) :
            m_viewportFirstItem(viewportFirstItem),
            m_currentItem(currentItem) {}

        int viewportFirstItem() const {return m_viewportFirstItem;}
        int currentItem() const       {return m_currentItem;}

    private:
        int m_viewportFirstItem;
        int m_currentItem;
    };
    std::stack<ViewportState> m_viewportStateHistory;
};
} // ncxmms2

#endif // LOCALFILESYSTEMBROWSER_H

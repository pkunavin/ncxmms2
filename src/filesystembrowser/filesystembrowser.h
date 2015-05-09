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

#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include "dir.h"
#include "../xmmsutils/result.h"
#include "../listviewappintegrated/listviewappintegrated.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class AbstractFileSystemModel;

class FileSystemBrowser : public ListViewAppIntegrated
{
public:
    FileSystemBrowser(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);
    
    void setFsModel(AbstractFileSystemModel *fsModel);
    AbstractFileSystemModel *fsModel() const;
    
    void setDirectory(const Dir& dir);
    const Dir& directory() const;
    
    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    
    // Signals:
    NCXMMS2_SIGNAL(showSongInfo, int)

private:
    xmms2::Client *m_xmmsClient;
    Dir m_currentDir;
    
    void onItemEntered(int item);
    void cd(const std::string& dir);
    void directoryLoaded(const Dir& dir);
    void directoryLoadFailed(const Dir& dir, const std::string& error);
    
    void addItemToActivePlaylist();
    void activePlaylistAddFileOrDirectory(int item, bool beQuiet = false);
    void activePlaylistAddFile(int item, bool beQuiet = false);
    void activePlaylistPlayItem(int item);
    void activePlaylistPlayFile(const std::string& url);
    void activePlaylistPlayPlaylistFile(const std::string& url);
    void askChangeDirectory();
    void goUp();
    void toggleSelectionWithoutDotDot(const KeyEvent& keyEvent);
    void invertSelectionWithoutDotDot(const KeyEvent& keyEvent);
    void selectByRegexpWithoutDotDot();
    void reloadDirectory();
    void getFileIdAndShowSongInfo();

    void getMedialibIdForFile(const std::string& url, const std::function<void (const xmms2::Expected<int> &)>& callback);
};
} // ncxmms2

#endif // FILESYSTEMBROWSER_H

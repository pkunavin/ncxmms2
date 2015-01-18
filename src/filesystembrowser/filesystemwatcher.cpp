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

#include "filesystemwatcher.h"
#include "../log.h"

#ifdef __linux__

#include <glib.h>
#include <unistd.h>
#include <sys/inotify.h>

namespace ncxmms2 {

class FileSystemWatcherPrivate
{
public:
    FileSystemWatcherPrivate(FileSystemWatcher *_q);
    ~FileSystemWatcherPrivate();
    
    FileSystemWatcher *q;
    int fd;
    int wd;
    int fdPollSource;
    
    void watch(const std::string& path);
    void readInotify();
    static gboolean readInotifyEvent(GIOChannel *iochan, GIOCondition cond, gpointer data);
};

FileSystemWatcherPrivate::FileSystemWatcherPrivate(FileSystemWatcher *_q) :
    q(_q),
    fd(-1),
    wd(-1)
{
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        NCXMMS2_LOG_ERROR("inotify_init failed");
        return;
    }
    
    GIOChannel *iochan = g_io_channel_unix_new(fd);
    fdPollSource = g_io_add_watch(iochan, G_IO_IN, FileSystemWatcherPrivate::readInotifyEvent, this);
    g_io_channel_unref(iochan);
}

FileSystemWatcherPrivate::~FileSystemWatcherPrivate()
{
    if (fd != -1) {
        g_source_remove(fdPollSource);
        close(fd);
    }
}

void FileSystemWatcherPrivate::watch(const std::string& path)
{
    if (fd == -1)
        return;
    
    if (wd != -1)
        inotify_rm_watch(fd, wd);
    
    wd = inotify_add_watch(fd, path.c_str(), IN_CREATE | IN_DELETE
                                           | IN_MOVED_FROM | IN_MOVED_TO
                                           | IN_DELETE_SELF | IN_MOVE_SELF);
}

void FileSystemWatcherPrivate::readInotify()
{
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    
    for (;;) {
        auto len = read(fd, buf, sizeof(buf));
        if (len <= 0)
            break;
        
        char *ptr = buf;
        char *end = buf + len;
        while (ptr < end) {
            struct inotify_event *event = reinterpret_cast<struct inotify_event*>(ptr);
            
            if (event->mask & IN_CREATE || event->mask & IN_MOVED_TO) {
                if (event->mask & IN_ISDIR) {
                    q->directoryCreated(event->name);
                } else {
                    q->fileCreated(event->name);
                }
            }
            
            if (event->mask & IN_DELETE || event->mask & IN_MOVED_FROM) {
                if (event->mask & IN_ISDIR) {
                    q->directoryDeleted(event->name);
                } else {
                    q->fileDeleted(event->name);
                }
            }
            
            if (event->mask & IN_DELETE_SELF || event->mask & IN_MOVE_SELF) {
                q->selfDeleted();
            }
            
            ptr += sizeof(struct inotify_event) + event->len;
        }
    }
}

gboolean FileSystemWatcherPrivate::readInotifyEvent(GIOChannel *iochan, GIOCondition cond, gpointer data)
{
    NCXMMS2_UNUSED(iochan);
    NCXMMS2_UNUSED(cond);
    
    FileSystemWatcherPrivate *p = static_cast<FileSystemWatcherPrivate*>(data);
    p->readInotify();
    return TRUE;
}
} // ncxmms2

#else

namespace ncxmms2 {

class FileSystemWatcherPrivate
{
public:
    FileSystemWatcherPrivate(FileSystemWatcher *_q) : q(_q) {}
    
    FileSystemWatcher *q;

    void watch(const std::string& path)
    {
        NCXMMS2_UNUSED(path);
    }
};
} // ncxmms2
#endif



using namespace ncxmms2;

FileSystemWatcher::FileSystemWatcher(Object *parent) :
    Object(parent),
    d(new FileSystemWatcherPrivate(this))
{
    
}

FileSystemWatcher::~FileSystemWatcher()
{
    
}

void FileSystemWatcher::watch(const std::string& path)
{
    d->watch(path);
}


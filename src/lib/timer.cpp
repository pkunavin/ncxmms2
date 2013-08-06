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

#include <glib.h>
#include "timer.h"

namespace ncxmms2 {

class TimerPrivate
{
public:
    TimerPrivate() :
        id(0),
        isSingleShot(false) {}

    guint id;
    bool isSingleShot;
    static gboolean timeoutCallback(gpointer data);
};
} // ncxmms2

using namespace ncxmms2;

Timer::Timer(Object *parent) :
    Object(parent),
    d(new TimerPrivate())
{

}

void Timer::start(unsigned int sec)
{
    stop();
    d->id = g_timeout_add_seconds(sec, TimerPrivate::timeoutCallback, this);
}

void Timer::startMs(unsigned int msec)
{
    stop();
    d->id = g_timeout_add(msec, TimerPrivate::timeoutCallback, this);
}

void Timer::stop()
{
    if (d->id) {
        g_source_remove(d->id);
        d->id = 0;
    }
}

void Timer::setSingleShot(bool singleShot)
{
    d->isSingleShot = singleShot;
}

bool Timer::isSingleShot() const
{
    return d->isSingleShot;
}

gboolean TimerPrivate::timeoutCallback(gpointer data)
{
    Timer *q = static_cast<Timer*>(data);
    q->timeout();
    if (q->isSingleShot()) {
        q->d->id = 0;
        return FALSE;
    }
    return TRUE;
}

Timer::~Timer()
{
    if (d->id)
        g_source_remove(d->id);
}


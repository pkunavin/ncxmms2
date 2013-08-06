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

#ifndef TIMER_H
#define TIMER_H

#include "object.h"

namespace ncxmms2 {

class TimerPrivate;

class Timer : public Object
{
public:
    Timer(Object *parent = nullptr);
    ~Timer();

    void start(unsigned int sec);
    void startMs(unsigned int msec);
    void stop();

    void setSingleShot(bool singleShot);
    bool isSingleShot() const;

    // Signals
    NCXMMS2_SIGNAL(timeout)

private:
    friend class TimerPrivate;
    std::unique_ptr<TimerPrivate> d;
};
} // ncxmms2

#endif // TIMER_H

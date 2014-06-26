/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef LOG_H
#define LOG_H

#include <cstdio>

namespace ncxmms2 {

class Log
{
public:
    static void logPrintf(const char *format, ...)
#ifdef __GNUC__
    __attribute__((format (printf, 1, 2)))
#endif
;

    static void debugPrintf(const char *format, ...)
#ifdef __GNUC__
    __attribute__((format (printf, 1, 2)))
#endif
;

private:
    Log();
    Log(const Log&);
    Log& operator=(const Log&);
    ~Log();

    static Log& instance()
    {
        static Log inst;
        return inst;
    }

    std::FILE *m_logFile;
};
} // ncxmms2

#define NCXMMS2_LOG(...) ncxmms2::Log::logPrintf(__VA_ARGS__)
#define NCXMMS2_LOG_ERROR(msg) NCXMMS2_LOG("Error in %s at line %d : %s : %s.\n", __FILE__, __LINE__, __func__, msg)
#ifndef NDEBUG
#define NCXMMS2_DEBUG(...) ncxmms2::Log::debugPrintf(__VA_ARGS__)
#else
#define NCXMMS2_DEBUG(...)
#endif

#endif // LOG_H

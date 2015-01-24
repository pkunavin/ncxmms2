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

#ifndef LOG_H
#define LOG_H

#include <fstream>
#include "../3rdparty/tinyformat/tinyformat.h"

namespace ncxmms2 {

class Log
{
public:
    template <typename ... Args>
    static void logPrintf(const char *format, Args&&... args)
    {
        tfm::format(instance().m_logFile, format, std::forward<Args>(args)...);
        instance().m_logFile.flush();
    }

    template <typename ... Args>
    static void debugPrintf(const char *format, Args&&... args)
    {
        instance().m_logFile << "[DEBUG] ";
        logPrintf(format, std::forward<Args>(args)...);
    }

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

    std::fstream m_logFile;
};
} // ncxmms2

#define NCXMMS2_LOG(...) ncxmms2::Log::logPrintf(__VA_ARGS__)
#define NCXMMS2_LOG_ERROR(msg, ...) NCXMMS2_LOG("Error in %s at line %d : %s : " msg "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
#ifndef NDEBUG
#define NCXMMS2_DEBUG(...) ncxmms2::Log::debugPrintf(__VA_ARGS__)
#else
#define NCXMMS2_DEBUG(...)
#endif

#endif // LOG_H

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
#include <stdarg.h>
#include <string>
#include <stdexcept>

#include "log.h"

using namespace ncxmms2;

void Log::logPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::vfprintf(instance().m_logFile, format, args);
    va_end(args);
    std::fflush(instance().m_logFile);
}

void Log::debugPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::fprintf(instance().m_logFile, "[DEBUG] ");
    std::vfprintf(instance().m_logFile, format, args);
    va_end(args);
    std::fflush(instance().m_logFile);
}

Log::Log() : m_logFile(nullptr)
{
    std::string logFilePath(g_get_user_config_dir());
    logFilePath.append("/ncxmms2/ncxmms2.log");

    gchar *logDir = g_path_get_dirname(logFilePath.c_str());
    int err = 0;
    if (!g_file_test(logDir, G_FILE_TEST_EXISTS))
        err = g_mkdir_with_parents(logDir, 0755);
    g_free(logDir);
    if (err == -1)
        throw std::runtime_error("Can't create config directory!");

    if (!(m_logFile = std::fopen(logFilePath.c_str(), "a")))
        throw std::runtime_error("Can't create log file!");

    std::fprintf(m_logFile, "ncxmms2 logging system started!\n");
}

Log::~Log()
{
    std::fprintf(m_logFile, "ncxmms2 logging system halted!\n");
    std::fclose(m_logFile);
}

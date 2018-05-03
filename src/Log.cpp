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

#include <glib.h>
#include <string>
#include <stdexcept>

#include "Log.h"

using namespace ncxmms2;

Log::Log()
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

    m_logFile.open(logFilePath.c_str(), std::ios_base::app);
    if (!m_logFile.is_open())
        throw std::runtime_error("Can't create log file!");

    m_logFile << "ncxmms2 logging system started!" << std::endl;
}

Log::~Log()
{
    m_logFile << "ncxmms2 logging system halted!\n";
}

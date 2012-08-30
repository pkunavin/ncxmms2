/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <stdexcept>

#include "settings.h"

using namespace ncxmms2;

Settings::Settings()
{
    const std::string configFilePath =
            std::string(g_get_user_config_dir()).append("/ncxmms2/ncxmms2.conf");

    if (!g_file_test(configFilePath.c_str(), G_FILE_TEST_EXISTS)) {
        gchar *configDir = g_path_get_dirname(configFilePath.c_str());
        int err = g_mkdir_with_parents(configDir, 0755);
        g_free(configDir);
        if (err == -1)
            throw std::runtime_error("Can't create config directory!");

        const std::string configTitle("# ncxmms2 config file");
        if (!g_file_set_contents(configFilePath.c_str(), configTitle.c_str(), configTitle.size(), NULL))
            throw std::runtime_error("Can't create config file!");
    }

    m_iniParser.setFile(configFilePath);
    if (!m_iniParser.isOk()) {
        throw std::runtime_error(
            std::string("Can't load config file: ").append(m_iniParser.errorString())
        );
    }
}

Settings::~Settings()
{

}


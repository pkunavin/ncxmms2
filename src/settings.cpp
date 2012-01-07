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

#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <stdexcept>
#include <fstream>
#include "settings.h"

using namespace ncxmms2;

Settings::Settings()
{
    m_configFilePath.append(g_get_user_config_dir()).append("/ncxmms2/ncxmms2.conf");

    if (!g_file_test(m_configFilePath.c_str(), G_FILE_TEST_EXISTS)) {
        gchar *configDir = g_path_get_dirname(m_configFilePath.c_str());
        int err = g_mkdir_with_parents(configDir, 0755);
        g_free(configDir);
        if (err == -1)
            throw std::runtime_error("Can't create config directory!");

        const std::string configTitle("# ncxmms2 config file");
        if (!g_file_set_contents(m_configFilePath.c_str(), configTitle.c_str(), configTitle.size(), NULL))
            throw std::runtime_error("Can't create config file!");
    }

    m_configFile = g_key_file_new();
    GError *error = NULL;
    if (!g_key_file_load_from_file(m_configFile, m_configFilePath.c_str(), G_KEY_FILE_KEEP_COMMENTS, &error))
        throw std::runtime_error(std::string("Can't load config file: ").append(error->message));
}

Settings::~Settings()
{
    gsize size;
    gchar *data = g_key_file_to_data(m_configFile, &size, NULL);

    if (!g_file_set_contents(m_configFilePath.c_str(), data, size, NULL))
        throw std::runtime_error("Can't save config file!");

    g_free(data);
    g_key_file_free(m_configFile);
}

void Settings::setValue(const std::string& group, const std::string& key, int value)
{
    g_key_file_set_integer(instance().m_configFile, group.c_str(), key.c_str(), value);
}

void Settings::setValue(const std::string& group, const std::string& key, const std::string& value)
{
    g_key_file_set_string(instance().m_configFile, group.c_str(), key.c_str(), value.c_str());
}

void Settings::setValue(const std::string& group, const std::string& key, bool value)
{
    g_key_file_set_boolean(instance().m_configFile, group.c_str(), key.c_str(), value);
}

void Settings::getValue(const std::string& group, const std::string& key, int *value)
{
    GError *error = NULL;
    int val = g_key_file_get_integer(m_configFile, group.c_str(), key.c_str(), &error);
    if (error) {
        g_error_free(error);;
    } else {
        *value = val;
    }
}

void Settings::getValue(const std::string& group, const std::string& key, std::string *value)
{
    gchar *string = g_key_file_get_string(m_configFile, group.c_str(), key.c_str(), NULL);
    if (string) {
        value->assign(string);
        g_free(string);
    }
}

void Settings::getValue(const std::string& group, const std::string& key, bool *value)
{
    GError *error = NULL;
    gboolean val = g_key_file_get_boolean(m_configFile, group.c_str(), key.c_str(), &error);
    if (error) {
        g_error_free(error);;
    } else {
        *value = val;
    }
}


/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

typedef struct _GKeyFile GKeyFile;

namespace ncxmms2 {

class Settings
{
public:

    template<class T>
    static T value(const std::string& group, const std::string& key, T defaultValue = T())
    {
        instance().getValue(group, key, &defaultValue);
        return defaultValue;
    }


    static void setValue(const std::string& group, const std::string& key, int value);
    static void setValue(const std::string& group, const std::string& key, const std::string& value);
    static void setValue(const std::string& group, const std::string& key, bool value);

private:
    static Settings& instance()
    {
        static Settings inst;
        return inst;
    }

    Settings();
    ~Settings();
    Settings(const Settings& other);
    Settings& operator=(const Settings& other);

    void getValue(const std::string& group, const std::string& key, int *value);
    void getValue(const std::string& group, const std::string& key, std::string *value);
    void getValue(const std::string& group, const std::string& key, bool *value);

    GKeyFile *m_configFile;
    std::string m_configFilePath;
};
} // ncxmms2

#endif // SETTINGS_H

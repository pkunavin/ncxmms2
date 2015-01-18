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
#include "iniparser.h"

namespace ncxmms2 {

class IniParserPrivate
{
public:
    IniParserPrivate() :
        keyFile(nullptr) {}

    std::string errorString;
    std::string filePath;
    GKeyFile *keyFile;

    void closeFile();
};
} // ncxmms2

using namespace ncxmms2;

void IniParserPrivate::closeFile()
{
    errorString.clear();
    if (keyFile) {
        g_key_file_free(keyFile);
        keyFile = nullptr;
    }
}

IniParser::IniParser() :
    d(new IniParserPrivate())
{

}

IniParser::~IniParser()
{
    sync();
    d->closeFile();
}

void IniParser::setFile(const std::string& file)
{
    sync();
    d->closeFile();

    d->filePath = file;
    d->keyFile = g_key_file_new();
    GError *error = nullptr;
    if (!g_key_file_load_from_file(d->keyFile, d->filePath.c_str(),
                                   G_KEY_FILE_KEEP_COMMENTS, &error)) {
        d->errorString = error->message;
        g_error_free(error);
    }
}

void IniParser::sync()
{
    if (!d->keyFile)
        return;

    gsize size;
    gchar *data = g_key_file_to_data(d->keyFile, &size, nullptr);

    GError *error = nullptr;
    g_file_set_contents(d->filePath.c_str(), data, size, &error);
    if (error) {
        d->errorString = error->message;
        g_error_free(error);
    }
    g_free(data);
}

bool IniParser::isOk() const
{
    return d->errorString.empty();
}

std::string IniParser::errorString() const
{
    return d->errorString;
}

void IniParser::setValue(const std::string& group, const std::string& key, int value)
{
    if (!d->keyFile)
        return;

    g_key_file_set_integer(d->keyFile, group.c_str(), key.c_str(), value);
}

void IniParser::setValue(const std::string& group, const std::string& key, const std::string& value)
{
    if (!d->keyFile)
        return;

    g_key_file_set_string(d->keyFile, group.c_str(), key.c_str(), value.c_str());
}

void IniParser::setValue(const std::string& group, const std::string& key, bool value)
{
    if (!d->keyFile)
        return;

    g_key_file_set_boolean(d->keyFile, group.c_str(), key.c_str(), value);
}

void IniParser::getValue(const std::string& group, const std::string& key, int *value) const
{
    if (!d->keyFile)
        return;

    GError *error = nullptr;
    int val = g_key_file_get_integer(d->keyFile, group.c_str(), key.c_str(), &error);
    if (error) {
        g_error_free(error);;
    } else {
        *value = val;
    }
}

void IniParser::getValue(const std::string& group, const std::string& key, std::string *value) const
{
    if (!d->keyFile)
        return;

    gchar *string = g_key_file_get_string(d->keyFile, group.c_str(), key.c_str(), nullptr);
    if (string) {
        value->assign(string);
        g_free(string);
    }
}

void IniParser::getValue(const std::string& group, const std::string& key, bool *value) const
{
    if (!d->keyFile)
        return;

    GError *error = nullptr;
    gboolean val = g_key_file_get_boolean(d->keyFile, group.c_str(), key.c_str(), &error);
    if (error) {
        g_error_free(error);;
    } else {
        *value = val;
    }
}



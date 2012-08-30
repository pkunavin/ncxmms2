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

#ifndef INIPARSER_H
#define INIPARSER_H

#include <string>
#include <memory>

namespace ncxmms2 {

class IniParserPrivate;

class IniParser
{
public:
    IniParser();
    ~IniParser();

    void setFile(const std::string& file);
    void sync();

    bool isOk() const;
    std::string errorString() const;

    template <class T>
    T value(const std::string& group, const std::string& key, T defaultValue = T()) const
    {
        getValue(group, key, &defaultValue);
        return defaultValue;
    }

     void setValue(const std::string& group, const std::string& key, int value);
     void setValue(const std::string& group, const std::string& key, const std::string& value);
     void setValue(const std::string& group, const std::string& key, bool value);

private:
    std::unique_ptr<IniParserPrivate> d;

    void getValue(const std::string& group, const std::string& key, int *value) const;
    void getValue(const std::string& group, const std::string& key, std::string *value) const;
    void getValue(const std::string& group, const std::string& key, bool *value) const;
};
} // ncxmms2

#endif // INIPARSER_H

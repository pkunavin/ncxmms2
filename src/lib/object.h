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

#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include "ncxmms2.h"
#include "signals.h"

namespace ncxmms2 {

class ObjectPrivate;

class Object
{
public:
    Object(Object *parent = nullptr);
    Object(const Object& other) = delete;
    Object& operator=(const Object& other) = delete;
    virtual ~Object();

    Object *parent() const;

    void setName(const std::string& name);
    const std::string& name() const;

    void registerConnection(Signals::Connection connection);

    // Signals:
    NCXMMS2_SIGNAL(nameChanged, const std::string&)

private:
    std::unique_ptr<ObjectPrivate> d;
};
} // ncxmms2

#endif // OBJECT_H

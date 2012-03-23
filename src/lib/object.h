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

#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include "signals.h"

namespace ncxmms2 {

class ObjectPrivate;

class Object
{
public:
    Object(Object *parent = nullptr);
    virtual ~Object();

    Object *parent() const;

    void registerConnection(const Signals::connection& connection);

private:
    Object(const Object& other);
    Object& operator=(const Object& other);
    std::unique_ptr<ObjectPrivate> d;
};
} // ncxmms2

#endif // OBJECT_H

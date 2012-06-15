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

#include <algorithm>
#include <vector>
#include <assert.h>

#include "object.h"

namespace ncxmms2 {

class ObjectPrivate
{
public:
    ObjectPrivate(Object *parent_) : parent(parent_), aboutToBeDestroyedByParent(false) {}

    Object *parent;
    std::vector<Object*> children;
    bool aboutToBeDestroyedByParent;
    std::string name;
    std::vector<Signals::connection> connections;
};
} // ncxmms2

using namespace ncxmms2;

Object::Object(Object *parent) : d(new ObjectPrivate(parent))
{
    if (parent)
        d->parent->d->children.push_back(this);
}

void Object::registerConnection(const Signals::connection& connection)
{
    d->connections.push_back(connection);
}

Object::~Object()
{
    for (auto& connection : d->connections) {
        connection.disconnect();
    }

    if (d->parent && !d->aboutToBeDestroyedByParent) {
        std::vector<Object*>& childrenOfParent = d->parent->d->children;
        auto it = std::find(childrenOfParent.begin(), childrenOfParent.end(), this);
        assert(it != childrenOfParent.end());
        childrenOfParent.erase(it);
    }

    for (Object *obj : d->children) {
        obj->d->aboutToBeDestroyedByParent = true;
        delete obj;
    }
}

Object *Object::parent() const
{
    return d->parent;
}

void Object::setName(const std::string& name)
{
    d->name = name;
    nameChanged(d->name);
}

const std::string& Object::name() const
{
    return d->name;
}

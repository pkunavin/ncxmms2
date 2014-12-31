/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <limits>
#include <stdexcept>

#include "signals.h"

namespace ncxmms2 {
namespace Signals {
namespace SignalsImpl {

uint32_t lastConnectionId = 0;
folly::sorted_vector_map<Connection, SignalBase*> connectionsMap;

} // SignalsImpl
} // Signals
} // ncxmms2

using namespace ncxmms2;

void Signals::Connection::disconnect()
{
    using namespace SignalsImpl;
    auto it = connectionsMap.find(*this);
    if (it != connectionsMap.end()) {
        SignalBase *signal = it->second;
        signal->eraseConnection(*this);
        connectionsMap.erase(it);
    }
}

bool Signals::Connection::isConnected() const
{
    using namespace SignalsImpl;
    return connectionsMap.find(*this) != connectionsMap.end();
}

void Signals::Connection::block()
{
    using namespace SignalsImpl;
    auto it = connectionsMap.find(*this);
    if (it != connectionsMap.end()) {
        SignalBase *signal = it->second;
        signal->blockConnection(*this, true);
    }
}

void Signals::Connection::unblock()
{
    using namespace SignalsImpl;
    auto it = connectionsMap.find(*this);
    if (it != connectionsMap.end()) {
        SignalBase *signal = it->second;
        signal->blockConnection(*this, false);
    }
}

bool Signals::Connection::isBlocked() const
{
    using namespace SignalsImpl;
    auto it = connectionsMap.find(*this);
    if (it != connectionsMap.end()) {
        SignalBase *signal = it->second;
        return signal->isBlockedConnection(*this);
    }
    return false;
}

Signals::SignalBase::~SignalBase()
{
    
}

Signals::Connection Signals::SignalBase::creatConnection(SignalBase *signal)
{
    using namespace SignalsImpl;
    if (lastConnectionId == std::numeric_limits<decltype(lastConnectionId)>::max()) {
        throw std::runtime_error("No more connection id's available!");
    }
    ++lastConnectionId;
    Connection conn(lastConnectionId);
    connectionsMap.insert(std::make_pair(conn, signal));
    return conn;
}

void Signals::SignalBase::destroyConnection(Signals::Connection connection)
{
    SignalsImpl::connectionsMap.erase(connection);
}

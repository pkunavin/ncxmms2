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

#include <xmmsclient/xmmsclient++-glib.h>
#include "xmms2client.h"
#include "lib/application.h"

using namespace ncxmms2;

Xmms2Client::Xmms2Client() : m_xmmsClient("ncxmms2")
{
}

bool Xmms2Client::connect(const std::string& ipcPath)
{
    bool connected = false;

    try
    {
        m_xmmsClient.connect(!ipcPath.empty() ? ipcPath.c_str() : NULL);
        m_xmmsClient.setMainloop(new Xmms::GMainloop(m_xmmsClient.getConnection()));
        m_xmmsClient.setDisconnectCallback(boost::bind(&Xmms2Client::disconnectCallback, this));
        connected = true;
    }
    catch (Xmms::connection_error& error)
    {
        std::cerr << "Connection failed: " << error.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Some error occurred while trying to connect to xmms2!" << std::endl;
    }

    return connected;
}

Xmms::Client *Xmms2Client::client()
{
    return &m_xmmsClient;
}

void Xmms2Client::disconnectCallback()
{
    Application::shutdown();
    std::cerr << "Disconnected!" << std::endl;
}


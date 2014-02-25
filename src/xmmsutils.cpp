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

#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient++-glib.h>
#include <boost/lexical_cast.hpp>

#include "xmmsutils.h"
#include "lib/application.h"

using namespace ncxmms2;

std::unique_ptr<Xmms::Client> XmmsUtils::clientCreateAndConnect(const std::string& ipcPath)
{
    bool connected = false;
    std::unique_ptr<Xmms::Client> xmmsClient(new Xmms::Client("ncxmms2"));
    try
    {
        xmmsClient->connect(!ipcPath.empty() ? ipcPath.c_str() : nullptr);
        xmmsClient->setMainloop(new Xmms::GMainloop(xmmsClient->getConnection()));
        xmmsClient->setDisconnectCallback([](){
            Application::shutdown();
            std::cerr << "Disconnected!" << std::endl;
        });
        connected = true;
    }
    catch (const Xmms::connection_error& error)
    {
        std::cerr << "Connection failed: " << error.what()
                  << " (ipcpath = " << ipcPath << ')' << std::endl;
    }
    catch(...)
    {
        std::cerr << "Some error occurred while trying to connect to xmms2!" << std::endl;
    }

    return connected ? std::move(xmmsClient) : std::unique_ptr<Xmms::Client>();
}

void XmmsUtils::playlistAddPlaylistFile(Xmms::Client *xmmsClient,
                                        const std::string& playlist, const std::string& file)
{
    auto getIdList = [xmmsClient, playlist](const Xmms::Coll::Coll& idlist)
    {
        xmmsClient->playlist.addIdlist(idlist, playlist);
        return true;
    };

    xmmsClient->collection.idlistFromPlaylistFile(file)(getIdList);
}


XmmsUtils::XmmsConfig::XmmsConfig(Xmms::Client *xmmsClient) :
    Object(nullptr),
    m_xmmsClient(xmmsClient)
{
    m_xmmsClient->config.valueList()(Xmms::bind(&XmmsConfig::getConfig, this));
    m_xmmsClient->config.broadcastValueChanged()(Xmms::bind(&XmmsConfig::handleConfigChange, this));
}

std::string XmmsUtils::XmmsConfig::getValue(const std::string& key, const std::string& defaultValue) const
{
    auto it = m_config.find(key);
    return it != m_config.end() ? it->second : defaultValue;
}

void XmmsUtils::XmmsConfig::setValue(const std::string& key, const std::string& value)
{
    m_xmmsClient->config.valueSet(key, value);
}

namespace {
class XmmsConfigVariantVisitor : public boost::static_visitor<std::string>
{
public:
    std::string operator()(const int32_t& i) const
    {
        return boost::lexical_cast<std::string>(i);
    }

    std::string operator()(const std::string& str) const
    {
        return str;
    }
};
}

bool XmmsUtils::XmmsConfig::getConfig(const Xmms::Dict& dict)
{
    for (const Xmms::Dict::Pair& pair : dict) {
        m_config[pair.first] = boost::apply_visitor(XmmsConfigVariantVisitor(), pair.second);
    }
    configLoaded();
    return true;
}

bool XmmsUtils::XmmsConfig::handleConfigChange(const Xmms::Dict &dict)
{
    for (const Xmms::Dict::Pair& pair : dict) {
        valueChanged(pair.first,
                     m_config[pair.first] = boost::apply_visitor(XmmsConfigVariantVisitor(), pair.second));
    }
    return true;
}

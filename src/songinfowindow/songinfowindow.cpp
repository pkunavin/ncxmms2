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
#include <boost/lexical_cast.hpp>

#include "songinfowindow.h"
#include "../songdisplayformatparser.h"

#include "../lib/htmlparser.h"
#include "../lib/keyevent.h"

using namespace ncxmms2;

SongInfoWindow::SongInfoWindow(Xmms::Client *xmmsClient, const Rectangle &rect, Window *parent) :
    TextView(rect, parent),
    m_xmmsClient(xmmsClient),
    m_id(-1)
{
    loadPalette("SongInfoWindow");
    setName("Song info");
    setMode(Mode::RichText);
    
    m_xmmsClient->medialib.broadcastEntryChanged()(
        Xmms::bind(&SongInfoWindow::handleSongInfoUpdate, this)
    );
}

void SongInfoWindow::showSongInfo(int id)
{
    m_id = id;
    m_xmmsClient->medialib.getInfo(m_id)(
        Xmms::bind(&SongInfoWindow::getSongInfo, this)
    );
    setText("Loading...");
}

void SongInfoWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyEscape:
            hideRequested();
            break;
        
        default:
            TextView::keyPressedEvent(keyEvent);
            break;
    }
}

bool SongInfoWindow::getSongInfo(const Xmms::PropDict& info)
{
    Song song;
    song.loadInfo(info);
    
    const char songVariables[] = {'t', 'a', 'b', 'p', 'g', 'y', 'F', 'i', 'n', 'N', 'l', 'B', 'S'};
    std::string text;
    text.append("<pre>");
    for (char var : songVariables) {
        std::string varValue = SongDisplayFormatParser::getSongVariableValue(song, var);
        if (varValue.empty())
            continue;
        
        text.append("<b>");
        text.append(SongDisplayFormatParser::getSongVariableName(var));
        text.append(":</b> ");
        text.append(HtmlParser::encodeEntities(varValue));
        text.append("\n");
    }
    text.append("</pre>");
    setText(text);
    return true;
}

bool SongInfoWindow::handleSongInfoUpdate(const int& id)
{
    if (id == m_id) {
        m_xmmsClient->medialib.getInfo(m_id)(
            Xmms::bind(&SongInfoWindow::getSongInfo, this)
        );
    }
    return true;
}

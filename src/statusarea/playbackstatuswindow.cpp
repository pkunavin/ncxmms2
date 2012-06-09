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

#include <boost/format.hpp>

#include "playbackstatuswindow.h"
#include "../utils.h"

#include "../lib/painter.h"
#include "../lib/rectangle.h"

using namespace ncxmms2;

PlaybackStatusWindow::PlaybackStatusWindow(Xmms::Client *client, int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent),
    m_xmmsClient(client),
    m_playbackStatus(Xmms::Playback::STOPPED),
    m_playbackPlaytime(0)
{
    setMinumumLines(1);
    setMaximumLines(1);
    setMinumumCols(strlen("[Stopped] ...[xx::xx::xx/xx::xx::xx]"));

    m_xmmsClient->playback.getStatus()(Xmms::bind(&PlaybackStatusWindow::getPlaybackStatus, this));
    m_xmmsClient->playback.broadcastStatus()(Xmms::bind(&PlaybackStatusWindow::getPlaybackStatus, this));

    m_xmmsClient->playback.currentID()(Xmms::bind(&PlaybackStatusWindow::getCurrentId, this));
    m_xmmsClient->playback.broadcastCurrentID()(Xmms::bind(&PlaybackStatusWindow::getCurrentId, this));
    m_xmmsClient->medialib.broadcastEntryChanged()(Xmms::bind(&PlaybackStatusWindow::handleIdInfoChanged, this));

    m_xmmsClient->playback.getPlaytime()(Xmms::bind(&PlaybackStatusWindow::getPlaytime, this));
    m_xmmsClient->playback.signalPlaytime()(Xmms::bind(&PlaybackStatusWindow::getPlaytime, this));
}

bool PlaybackStatusWindow::getPlaybackStatus(const Xmms::Playback::Status& status)
{
    m_playbackStatus = status;
    update();
    return true;
}

bool PlaybackStatusWindow::getCurrentId(const int& id)
{
    m_xmmsClient->medialib.getInfo(id)(Xmms::bind(&PlaybackStatusWindow::getCurrentIdInfo, this));
    return true;
}

bool PlaybackStatusWindow::getCurrentIdInfo(const Xmms::PropDict& info)
{
    m_currentSong.loadInfo(info);
    currentSongChanged(m_currentSong);
    update();
    return true;
}

bool PlaybackStatusWindow::getPlaytime(const int& playtime)
{
    m_playbackPlaytime = playtime;
    playtimeChanged(playtime);
    update();
    return true;
}

bool PlaybackStatusWindow::handleIdInfoChanged(const int& id)
{
    if (m_currentSong.id() == id)
        m_xmmsClient->medialib.getInfo(id)(Xmms::bind(&PlaybackStatusWindow::getCurrentIdInfo, this));
    return true;
}

void PlaybackStatusWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearLine(0);

    painter.setBold(true);
    switch (m_playbackStatus) {
        case Xmms::Playback::PLAYING : painter.printString("Playing: " ); break;
        case Xmms::Playback::STOPPED : painter.printString("[Stopped] "); break;
        case Xmms::Playback::PAUSED  : painter.printString("[Paused] " ); break;
    }
    painter.setBold(false);

    std::string timeString;
    timeString.reserve(19); // [xx:xx;xx/xx;xx;xx]
    timeString.push_back('[');
    timeString.append(Utils::getTimeStringFromInt(m_playbackPlaytime));
    if (m_currentSong.duration() > 0) {
        timeString.push_back('/');
        timeString.append(m_currentSong.durationString());
    }
    timeString.push_back(']');

    if (!m_currentSong.artist().empty()) {
        painter.squeezedPrint((boost::format("%1% - %2%") % m_currentSong.artist() % m_currentSong.title()).str(),
                              cols() - painter.x() - timeString.size() - 1);
    } else {
        painter.squeezedPrint(m_currentSong.title(), cols() - painter.x() - timeString.size() - 1);
    }

    painter.move(cols() - timeString.size(), 0);
    painter.setBold(true);
    painter.printString(timeString);

    painter.flush();
}

Xmms::Playback::Status PlaybackStatusWindow::playbackStatus() const
{
    return m_playbackStatus;
}

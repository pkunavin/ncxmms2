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

#include <cstring>

#include "playbackstatuswindow.h"
#include "../utils.h"
#include "../settings.h"
#include "../log.h"

#include "../lib/painter.h"
#include "../lib/rectangle.h"
#include "../lib/palette.h"
#include "../lib/application.h"

using namespace ncxmms2;

PlaybackStatusWindow::PlaybackStatusWindow(xmms2::Client *client, int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent),
    m_xmmsClient(client),
    m_playbackStatus(xmms2::PlaybackStatus::Stopped),
    m_playbackPlaytime(Utils::getTimeStringFromInt(0)),
    m_useTerminalWindowTitle(true)
{
    loadPalette("PlaybackStatusWindow");
    setMinumumLines(1);
    setMaximumLines(1);
    setMinumumCols(std::strlen("[Stopped] ...[xx::xx::xx/xx::xx::xx]"));

    std::string defaultDisplayFormat = "[l:1:0]{$a - $t}|{$t}|{$f}";
    const std::string displayFormat = Settings::value("PlaybackStatusWindow", "displayFormat",
                                                      defaultDisplayFormat);

    if (!m_songDisplayFormatter.setDisplayFormat(displayFormat)) {
        throw std::runtime_error(
            std::string("PlaybackStatusWindow: Parsing format string failed: ").append(
                m_songDisplayFormatter.errorString()
            )
        );
    }

    m_useTerminalWindowTitle = Settings::value("General", "useTerminalWindowTitle", true);
    std::string terminalWindowTitleFormat = "[l:1:0]";
    terminalWindowTitleFormat.append(
        Settings::value<std::string>("General", "terminalWindowTitleFormat", "{$a - $t}|{$t}|{$f}")
    );
    
    if (!m_terminalWindowTitleFormatter.setDisplayFormat(terminalWindowTitleFormat)) {
        throw std::runtime_error(
            std::string("PlaybackStatusWindow: Parsing terminal title format string failed: ").append(
                m_terminalWindowTitleFormatter.errorString()
            )
        );
    }
    
    m_xmmsClient->playbackGetStatus()(&PlaybackStatusWindow::getPlaybackStatus, this);
    m_xmmsClient->playbackStatusChanged_Connect(&PlaybackStatusWindow::getPlaybackStatus, this);

    m_xmmsClient->playbackGetCurrentId()(&PlaybackStatusWindow::getCurrentId, this);
    m_xmmsClient->playbackCurrentIdChanged_Connect(&PlaybackStatusWindow::getCurrentId, this);
    m_xmmsClient->medialibEntryChanged_Connect(&PlaybackStatusWindow::handleIdInfoChanged, this);

    m_xmmsClient->playbackGetPlaytime()(&PlaybackStatusWindow::getPlaytime, this);
    m_xmmsClient->playbackPlaytimeChanged_Connect(&PlaybackStatusWindow::getPlaytime, this);
}

void PlaybackStatusWindow::getPlaybackStatus(const xmms2::Expected<xmms2::PlaybackStatus>& status)
{
    if (status.isError()) {
        NCXMMS2_LOG_ERROR("%s", status.error());
        return;
    }
    
    m_playbackStatus = status.value();
    update();
}

void PlaybackStatusWindow::getCurrentId(const xmms2::Expected<int>& id)
{
    if (id.isError()) {
        NCXMMS2_LOG_ERROR("%s", id.error());
        return;
    }
    
    m_xmmsClient->medialibGetInfo(id.value())(&PlaybackStatusWindow::getCurrentIdInfo, this);
}

void PlaybackStatusWindow::getCurrentIdInfo(const xmms2::Expected<xmms2::PropDict>& info)
{
    if (info.isError()) {
        NCXMMS2_LOG_ERROR("%s", info.error());
        return;
    }
    
    m_currentSong.loadInfo(info.value());
    currentSongChanged(m_currentSong);
    if (m_useTerminalWindowTitle)
        updateTerminalWindowTitle();
    update();
}

void PlaybackStatusWindow::getPlaytime(const xmms2::Expected<int>& playtime)
{
    if (playtime.isError())
        return;
    
    std::string playtimeStr = Utils::getTimeStringFromInt(playtime.value());
    if (m_playbackPlaytime != playtimeStr) {
        m_playbackPlaytime.swap(playtimeStr);
        update();
    }
}

void PlaybackStatusWindow::handleIdInfoChanged(const xmms2::Expected<int>& id)
{
    if (id.isError()) {
        NCXMMS2_LOG_ERROR("%s", id.error());
        return;
    }
    
    if (m_currentSong.id() == id.value())
        m_xmmsClient->medialibGetInfo(id.value())(&PlaybackStatusWindow::getCurrentIdInfo, this);
}

void PlaybackStatusWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);
    
    const Palette::ColorGroup colorGroup = hasFocus()
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;
    Painter painter(this);
    painter.clearLine(0);

    painter.setColor(palette().color(colorGroup, Palette::RoleText));
    painter.setBold(true);
    switch (m_playbackStatus) {
        case xmms2::PlaybackStatus::Playing : painter.printString("Playing: " ); break;
        case xmms2::PlaybackStatus::Stopped : painter.printString("[Stopped] "); break;
        case xmms2::PlaybackStatus::Paused  : painter.printString("[Paused] " ); break;
    }
    painter.setBold(false);

    std::string timeString;
    timeString.reserve(19); // [xx:xx:xx/xx:xx:xx]
    timeString.push_back('[');
    timeString.append(m_playbackPlaytime);
    if (m_currentSong.duration() > 0) {
        timeString.push_back('/');
        timeString.append(Utils::getTimeStringFromInt(m_currentSong.duration()));
    }
    timeString.push_back(']');

    const Rectangle songDisplayRect = Rectangle(painter.x(), painter.y(),
                                                cols() - painter.x() - timeString.size() - 1, 1);
    m_songDisplayFormatter.paint(m_currentSong, &painter, songDisplayRect);

    painter.move(cols() - timeString.size(), 0);
    painter.setColor(palette().color(colorGroup, Palette::RoleText));
    painter.setBold(true);
    painter.printString(timeString);

    painter.flush();
}

void PlaybackStatusWindow::updateTerminalWindowTitle()
{
    std::string title = m_terminalWindowTitleFormatter.formattedString(m_currentSong, 0);
    Application::setTerminalWindowTitle(title);
}

xmms2::PlaybackStatus PlaybackStatusWindow::playbackStatus() const
{
    return m_playbackStatus;
}

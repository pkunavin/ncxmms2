/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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
#include "utils.h"
#include "lib/painter.h"

using namespace ncxmms2;

PlaybackStatusWindow::PlaybackStatusWindow(Xmms::Client* client, int lines, int cols, int yPos, int xPos, Window* parent) : 
	Window(lines, cols, yPos, xPos, parent),
	m_xmmsClient(client),
	m_playbackStatus(Xmms::Playback::STOPPED),
	m_playbackPlaytime(0),
	m_isShowingMessage(false)
{
	m_xmmsClient->playback.getStatus()(Xmms::bind(&PlaybackStatusWindow::getPlaybackStatus, this));
	m_xmmsClient->playback.broadcastStatus()(Xmms::bind(&PlaybackStatusWindow::getPlaybackStatus, this));
	
	m_xmmsClient->playback.currentID()(Xmms::bind(&PlaybackStatusWindow::getCurrentId, this));
	m_xmmsClient->playback.broadcastCurrentID()(Xmms::bind(&PlaybackStatusWindow::getCurrentId, this));
	m_xmmsClient->medialib.broadcastEntryChanged()(Xmms::bind(&PlaybackStatusWindow::handleIdInfoChanged, this));
	
	m_xmmsClient->playback.getPlaytime()(Xmms::bind(&PlaybackStatusWindow::getPlaytime, this));
	m_xmmsClient->playback.signalPlaytime()(Xmms::bind(&PlaybackStatusWindow::getPlaytime, this));
	
	m_messageTimer.connectTimeoutSignal(boost::bind(&PlaybackStatusWindow::hideMessage, this));
}

bool PlaybackStatusWindow::getPlaybackStatus(const Xmms::Playback::Status& status)
{
	m_playbackStatus=status;
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
	update();
	return true;
}

bool PlaybackStatusWindow::getPlaytime(const int& playtime)
{
	m_playbackPlaytime=playtime;
	update();
	return true;
}

bool PlaybackStatusWindow::handleIdInfoChanged(const int& id)
{
	if (m_currentSong.id()==id)
		m_xmmsClient->medialib.getInfo(id)(Xmms::bind(&PlaybackStatusWindow::getCurrentIdInfo, this));
	return true;
}

void PlaybackStatusWindow::showEvent()
{
	Painter painter(this);
	painter.drawHLine(0, 0, cols());

	if (m_currentSong.duration()>0) {
		const unsigned int pos=((double)m_playbackPlaytime/(double)m_currentSong.duration())*cols();
		painter.setBold(true);
		painter.drawHLine(0, 0, pos);
		painter.setBold(false);
	}

	painter.clearLine(1);
	
	if (!m_isShowingMessage) {
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
		if (m_currentSong.duration()>0) {
			timeString.push_back('/');
			timeString.append(m_currentSong.durationString());
		}
		timeString.push_back(']');
	
		if (!m_currentSong.artist().empty()) {
			painter.squeezedPrint((boost::format("%1% - %2%") % m_currentSong.artist() % m_currentSong.title()).str(),
								  cols()-painter.xPosition()-timeString.size()-1);
		} else {
			painter.squeezedPrint(m_currentSong.title(), cols()-painter.xPosition()-timeString.size()-1);
		}
		
		painter.move(cols()-timeString.size(), 1);
		painter.setBold(true);
		painter.printString(timeString);
	} else {
		painter.setBold(true);
		painter.printString(m_message);
	}
	
	painter.flush();
}

void PlaybackStatusWindow::showMessage(const std::string& message)
{
	m_message=message;
	m_isShowingMessage=true;
	m_messageTimer.start(5);
	update();
}

void PlaybackStatusWindow::hideMessage()
{
	m_isShowingMessage=false;
	update();
}

Xmms::Playback::Status PlaybackStatusWindow::playbackStatus() const
{
	return m_playbackStatus;
}

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

#include <stdexcept>

#include "notificationarea.h"
#include "playbackstatuswindow.h"

using namespace ncxmms2;

NotificationArea *NotificationArea::inst=0;

void NotificationArea::start(PlaybackStatusWindow* statusWindow)
{
	delete inst;
	inst=new NotificationArea(statusWindow);
}

void NotificationArea::shutdown()
{
	delete inst;
	inst=0;
}

void NotificationArea::showMessage(const std::string& message)
{
	if (inst) {
		inst->m_statusWindow->showMessage(message);
	} else {
		throw(std::logic_error(std::string(__PRETTY_FUNCTION__).append(": There is no instance of NotificationArea")));
	}
}

NotificationArea::NotificationArea(PlaybackStatusWindow *statusWindow) : m_statusWindow(statusWindow)
{

}

NotificationArea::~NotificationArea()
{

}

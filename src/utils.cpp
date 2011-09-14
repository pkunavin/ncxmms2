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

#include "utils.h"
#include <boost/lexical_cast.hpp>

using namespace ncxmms2;

std::string ncxmms2::Utils::getTimeStringFromInt(int msec)
{
	msec/=1000;
	std::string timeString;
	
	if (msec<3600) {
		timeString.reserve(5); // xx:xx
		
		const int min=msec/60;
		timeString.append(boost::lexical_cast<std::string>(min));
		
		timeString.push_back(':');
		
		const int sec=msec%60;
		if (sec<10)
			timeString.push_back('0');
		timeString.append(boost::lexical_cast<std::string>(sec));
	} else {
		timeString.reserve(8); // xx:xx:xx
		
		const int hours=msec/3600;
		timeString.append(boost::lexical_cast<std::string>(hours));
		
		timeString.push_back(':');
		
		const int min=msec/60-60*hours;
		if (min<10) 
			timeString.push_back('0');
		timeString.append(boost::lexical_cast<std::string>(min));
		
		timeString.push_back(':');
		
		const int sec=msec%60;
		if (sec<10)
			timeString.push_back('0');
		timeString.append(boost::lexical_cast<std::string>(sec));
	}
	
	return timeString;
}

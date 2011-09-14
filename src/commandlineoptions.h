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

#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include <string>

typedef struct _GOptionContext GOptionContext;

namespace ncxmms2
{
	class CommandLineOptions
	{
	public:
		CommandLineOptions(int argc, char **argv);
		~CommandLineOptions();
		
		bool parsingErrored() const;
		const std::string& errorString() const;
		
		const std::string& ipcPath() const;
		bool useColors() const;
		
	private:
		GOptionContext *optionContext;
		std::string m_errorString;
		
		std::string m_ipcPath;
		bool m_useColors;
	};
}

#endif // COMMANDLINEOPTIONS_H

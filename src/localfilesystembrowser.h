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

#ifndef LOCALFILESYSTEMBROWSER_H
#define LOCALFILESYSTEMBROWSER_H

#include <stack>
#include "lib/abstractitemview.h"

namespace Xmms
{
	class Client;
}

namespace ncxmms2
{
	class LocalFileSystemBrowser : public AbstractItemView
	{
	public:
		LocalFileSystemBrowser(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent=0);
		~LocalFileSystemBrowser();
		
		virtual void keyPressedEvent(const KeyEvent& keyEvent);
		
	protected:
		virtual void drawItem(int item);
		virtual int itemsCount() const;
		virtual void itemEntered(int item);
		
	private:
		Xmms::Client *m_xmmsClient;
		
		class FileSystemItem
		{
		public:
			enum ItemType
			{
				ItemFile,
				ItemDirectory,
				ItemOther
			};
			
			FileSystemItem(const std::string& name, ItemType type) : m_name(name), m_type(type){};
			std::string name() const {return m_name;}
			ItemType type() const    {return m_type;}
			bool isDirectory() const {return m_type==ItemDirectory;}
			bool isFile() const      {return m_type==ItemFile;}
		private:
			std::string m_name;
			ItemType m_type;
		};
		
		class Dir
		{
		public:
			Dir(const std::string& path) : m_path(path) {};
			Dir(const Dir& other) : m_path(other.m_path) {};
			std::string path() const {return m_path;}
			std::string name() const;
			Dir& cd(const std::string& dir);
		
		private:
			std::string m_path;
		};
		
		std::vector<FileSystemItem> m_currentDirEntries;
		Dir m_currentDir;
		
		bool setCurrentDir(const Dir& dir);
		std::string filePath(int item) const;
		void cd(const std::string& dir);
		
		class ViewportState
		{
		public:
			ViewportState(int viewportBegin, int currentItem) : m_viewportBegin(viewportBegin), m_currentItem(currentItem){};
			int viewportBegin() const {return m_viewportBegin;}
			int currentItem() const   {return m_currentItem;}
		
		private:
			int m_viewportBegin;
			int m_currentItem;
		};
		std::stack<ViewportState> m_viewportStateHistory;
	};

}

#endif // LOCALFILESYSTEMBROWSER_H

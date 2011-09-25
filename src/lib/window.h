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

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <memory>
#include <boost/function.hpp>
#include "keyevent.h"
#include "size.h"

namespace ncxmms2
{
	class WindowPrivate;

	class Window
	{
	public:
		Window(int lines, int cols, int yPos, int xPos, Window *parent=0);
		virtual ~Window();
		
		virtual void keyPressedEvent(const KeyEvent& keyEvent);
		virtual void resizeEvent(const Size& size);
		
		int lines() const;
		int cols() const;
		
		void move(int yPos, int xPos);
		
		void hide();
		void show();
		bool isHidden() const;
		
		void setFocus();
		bool hasFocus() const;
		
		void setTitle(const std::string& title);
		const std::string& title() const;
		typedef boost::function<void (const std::string&)> TitleChangedCallback;
		void setTitleChangedCallback(const TitleChangedCallback& callback);
			
	protected:
		virtual void showEvent();
		void update();
						
	private:
		Window(const Window& other);
		Window& operator=(const Window& other);
		std::unique_ptr<WindowPrivate> d;
		friend class Painter;
	};
}


#endif // WINDOW_H

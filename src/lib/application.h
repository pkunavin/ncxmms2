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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "size.h"

namespace ncxmms2 {

class Window;
class ApplicationPrivate;

class Application
{
public:
    static void init(bool useColors = true);
    static void run();
    static void shutdown();
    static void setMainWindow(Window *window);

    static void grabFocus(Window *window);
    static void releaseFocus();

    static bool useColors();
    static Size terminalSize();

private:
    Application(bool useColors);
    ~Application();
    Application(const Application& other);
    Application& operator=(const Application& other);
    static Application *inst;
    std::unique_ptr<ApplicationPrivate> d;
    friend class ApplicationPrivate;
};
} // ncxmms2

#endif // APPLICATION_H

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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <memory>
#include <map>

#include "size.h"

namespace ncxmms2 {

class Window;
class ApplicationPrivate;
class ColorScheme;
class Palette;

class Application
{
public:
    static void init(bool useColors = true, bool mouseEnable = false);
    static void run();
    static void shutdown();
    static void setMainWindow(Window *window);

    static void grabFocus(Window *window);
    static void releaseFocus();

    static bool useColors();
    static Size terminalSize();

    static void setMouseDoubleClickInterval(int msec);

    static void setColorSchemeFile(const std::string& file);
    static std::shared_ptr<Palette> getPalette(const std::string&                className,
                                               const std::shared_ptr<Palette>&   oldPalette);
    static std::shared_ptr<Palette> getPalette(const std::string&                className,
                                               const std::shared_ptr<Palette>&   oldPalette,
                                               const std::map<std::string, int>& userRolesMap);

private:
    Application(bool useColors, bool mouseEnable);
    ~Application();
    Application(const Application& other);
    Application& operator=(const Application& other);
    static Application *inst;
    std::unique_ptr<ApplicationPrivate> d;
    friend class ApplicationPrivate;
};
} // ncxmms2

#endif // APPLICATION_H

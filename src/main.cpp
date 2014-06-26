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

#include <glib.h>
#include <cstdlib>
#include <iostream>

#include "config.h"
#include "settings.h"
#include "commandlineoptions.h"
#include "mainwindow/mainwindow.h"
#include "xmmsutils/client.h"

#include "lib/application.h"
#include "lib/exceptions.h"

int main(int argc, char **argv)
{
    ncxmms2::CommandLineOptions options(argc, argv);
    if (options.parsingErrored()) {
        std::cerr << "Option parsing failed: " << options.errorString() << std::endl;
        return EXIT_FAILURE;
    }

    std::string ipcPath;
    if (!options.ipcPath().empty()) {
        ipcPath = options.ipcPath();
    } else {
        const char *xmmsPathEnv = std::getenv("XMMS_PATH");
        ipcPath = xmmsPathEnv
                  ? xmmsPathEnv
                  : ncxmms2::Settings::value<std::string>("General", "ipcpath");
    }

    ncxmms2::xmms2::Client xmmsClient;
    if (!xmmsClient.connect(ipcPath)) {
        std::cerr << "Connection failed (ipcpath = " << ipcPath << ')' << std::endl;
        return EXIT_FAILURE;        
    }
    
    xmmsClient.disconnected_Connect([]{
        ncxmms2::Application::shutdown();
        std::cerr << "Disconnected!" << std::endl;
    });
    
    const bool mouseEnable = ncxmms2::Settings::value("General", "mouseEnable", true);
    const int mouseDoubleClickInterval = ncxmms2::Settings::value("General",
                                                                  "mouseDoubleClickInterval", 300);

    try
    {
        ncxmms2::Application::init(options.useColors(), mouseEnable);
        ncxmms2::Application::setMouseDoubleClickInterval(mouseDoubleClickInterval);
        if (options.useColors()) {
            // Try user color scheme config
            const std::string userColorSchemeFilePath =
                    std::string(g_get_user_config_dir()).append("/ncxmms2/ncxmms2.colors");
            if (g_file_test(userColorSchemeFilePath.c_str(), G_FILE_TEST_EXISTS)) {
                ncxmms2::Application::setColorSchemeFile(userColorSchemeFilePath);
            } else {
                // Try global color scheme config
                const std::string globalColorSchemeFilePath =
                        std::string(CMAKE_INSTALL_PREFIX "/share/ncxmms2/ncxmms2.colors");
                if (g_file_test(globalColorSchemeFilePath.c_str(), G_FILE_TEST_EXISTS)) {
                    ncxmms2::Application::setColorSchemeFile(globalColorSchemeFilePath);
                }
            }
        }

        ncxmms2::Window *mainWindow = new ncxmms2::MainWindow(&xmmsClient);
        ncxmms2::Application::setMainWindow(mainWindow);
        mainWindow->show();
        ncxmms2::Application::run();
        ncxmms2::Application::shutdown();
    }
    catch (const ncxmms2::DesiredWindowSizeTooSmall& error)
    {
        ncxmms2::Application::shutdown();
        std::cerr << "Terminal too small!" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error& error)
    {
        ncxmms2::Application::shutdown();
        std::cerr << "Runtime error: " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

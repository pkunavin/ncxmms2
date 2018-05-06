/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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
#include "CommandLineOptions.h"

using namespace ncxmms2;

CommandLineOptions::CommandLineOptions(int argc, char **argv)
{
    gchar *ipcPath = NULL;
    gboolean noColors = FALSE;

    GOptionEntry entries[]=
    {
        {"ipcpath", 'i', 0, G_OPTION_ARG_STRING, &ipcPath, "Xmms2 IPC path", "path"},
        {"no-colors", 'n', 0, G_OPTION_ARG_NONE, &noColors, "Do not use colors", NULL},
        {NULL, ' ', 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
    };

    GError *error = NULL;

    optionContext = g_option_context_new("- ncxmms2 - An ncurses xmms2 client");
    g_option_context_add_main_entries(optionContext, entries, NULL);
    if (!g_option_context_parse(optionContext, &argc, &argv, &error)) {
        m_errorString.assign(error->message);
        g_error_free(error);
        return;
    }

    if (ipcPath) {
        m_ipcPath.assign(ipcPath);
        g_free(ipcPath);
    }

    m_useColors = !noColors;
}

CommandLineOptions::~CommandLineOptions()
{
    g_option_context_free(optionContext);
}

bool CommandLineOptions::parsingErrored() const
{
    return !m_errorString.empty();
}

const std::string& CommandLineOptions::errorString() const
{
    return m_errorString;
}

const std::string& CommandLineOptions::ipcPath() const
{
    return m_ipcPath;
}

bool CommandLineOptions::useColors() const
{
    return m_useColors;
}

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

#ifndef VISUALIZATIONCLIENT_H
#define VISUALIZATIONCLIENT_H

#include <string>

typedef struct xmmsc_connection_St xmmsc_connection_t;
typedef struct xmmsc_result_St xmmsc_result_t;

namespace ncxmms2 {
namespace xmms2 {

/* Visualization data is delivered through a separate XMMS2 connection.
 * The main client connection is integrated with the GLib main loop, and
 * visualization setup requires synchronous calls, which conflict with it.
 */
class VisualizationClient
{
public:
    VisualizationClient();
    ~VisualizationClient();

    bool connect(const std::string& patch);
    void disconnect();
    bool isConnected() const;
    
    bool start();
    bool isStarted() const;
    void stop();

    int chunkGet(short *buffer, int drawTimeMs, unsigned int blockingMs);
    const std::string& lastError() const;

private:
    VisualizationClient(const VisualizationClient&) = delete;
    VisualizationClient& operator=(const VisualizationClient&) = delete;

    bool checkResult(xmmsc_result_t *result);

    xmmsc_connection_t *m_connection;
    int m_visualizationId;
    std::string m_error;
};

} // xmms2
} // ncxmms2

#endif // VISUALIZATIONCLIENT_H

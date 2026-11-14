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

#include <xmmsclient/xmmsclient.h>

#include "VisualizationClient.h"
#include "../Utils.h"

using namespace ncxmms2;

namespace {

std::string resultError(xmmsc_connection_t *connection, xmmsc_result_t *result)
{
    const char *error = nullptr;
    xmmsv_t *value = xmmsc_result_get_value(result);
    if (value && xmmsv_get_error(value, &error) && error)
        return error;
    
    const char *lastError = xmmsc_get_last_error(connection);
    return lastError ? lastError : "unknown error";
}

} // namespace

xmms2::VisualizationClient::VisualizationClient() :
    m_connection(nullptr),
    m_visualizationId(-1)
{

}

xmms2::VisualizationClient::~VisualizationClient()
{
    stop();
    if (m_connection)
        xmmsc_unref(m_connection);
}

void xmms2::VisualizationClient::disconnect()
{
    stop();
    if (m_connection) {
        xmmsc_unref(m_connection);
        m_connection = nullptr;
    }
}

bool xmms2::VisualizationClient::connect(const std::string& patch)
{
    disconnect();
    m_error.clear();
    
    m_connection = xmmsc_init("ncxmms2-visualizer");
    if (!m_connection) {
        m_error = "can't create visualization client";
        return false;
    }
    
    if (!xmmsc_connect(m_connection, !patch.empty() ? patch.c_str() : nullptr)) {
        const char *error = xmmsc_get_last_error(m_connection);
        m_error = error ? error : "connection failed";
        xmmsc_unref(m_connection);
        m_connection = nullptr;
        return false;
    }
    return true;
}

bool xmms2::VisualizationClient::isConnected() const
{
    return m_connection != nullptr;
}

bool xmms2::VisualizationClient::checkResult(xmmsc_result_t *result)
{
    if (!result) {
        const char *error = xmmsc_get_last_error(m_connection);
        m_error = error ? error : "visualization request failed";
        return false;
    }
    
    xmmsc_result_wait(result);
    if (xmmsc_result_iserror(result)) {
        m_error = resultError(m_connection, result);
        xmmsc_result_unref(result);
        return false;
    }
    
    xmmsc_result_unref(result);
    return true;
}

bool xmms2::VisualizationClient::start()
{
    if (!m_connection) {
        m_error = "visualization client is not connected";
        return false;
    }
    
    if (m_visualizationId >= 0) {
        if (xmmsc_visualization_started(m_connection, m_visualizationId))
            return true;
        stop();
    }
    
    m_error.clear();
    
    // Check visualization protocol version
    xmmsc_result_t *result = xmmsc_visualization_version(m_connection);
    if (!result) {
        m_error = "can't query visualization version";
        return false;
    }
    
    xmmsc_result_wait(result);
    if (xmmsc_result_iserror(result)) {
        m_error = resultError(m_connection, result);
        xmmsc_result_unref(result);
        return false;
    }
    
    xmmsv_t *value = xmmsc_result_get_value(result);
    int version = 0;
    if (!value || !xmmsv_get_int(value, &version)) {
        m_error = "can't decode visualization version";
        xmmsc_result_unref(result);
        return false;
    }
    xmmsc_result_unref(result);
    
    if (version < 1) {
        m_error = Utils::format("unsupported visualization version: %d", version);
        return false;
    }
    
    // Register visualization client
    result = xmmsc_visualization_init(m_connection);
    if (!result) {
        m_error = "can't initialize visualization";
        return false;
    }
    
    xmmsc_result_wait(result);
    if (xmmsc_result_iserror(result)) {
        m_error = resultError(m_connection, result);
        xmmsc_result_unref(result);
        return false;
    }
    
    m_visualizationId = xmmsc_visualization_init_handle(result);
    xmmsc_result_unref(result);
    if (m_visualizationId < 0) {
        m_error = "can't initialize visualization";
        return false;
    }
    
    // Ask the server to send stereo PCM data; the spectrum is calculated
    // in the client so that extra server-side modifications are not needed.
    xmmsv_t *properties = xmmsv_build_dict(
        XMMSV_DICT_ENTRY_STR("type", "pcm"),
        XMMSV_DICT_ENTRY_STR("stereo", "1"),
        XMMSV_DICT_END
    );
    result = xmmsc_visualization_properties_set(m_connection, m_visualizationId, properties);
    xmmsv_unref(properties);
    if (!checkResult(result)) {
        stop();
        return false;
    }
    
    // Set up data transfer (shared memory or UDP)
    for (int i = 0; i < 4 && !xmmsc_visualization_started(m_connection, m_visualizationId); ++i) {
        if (xmmsc_visualization_errored(m_connection, m_visualizationId)) {
            m_error = "visualization is not available";
            stop();
            return false;
        }
        
        result = xmmsc_visualization_start(m_connection, m_visualizationId);
        if (result) {
            xmmsc_result_wait(result);
            xmmsc_visualization_start_handle(m_connection, result);
            xmmsc_result_unref(result);
        }
    }
    
    if (!xmmsc_visualization_started(m_connection, m_visualizationId)) {
        m_error = "can't start visualization";
        stop();
        return false;
    }
    return true;
}

bool xmms2::VisualizationClient::isStarted() const
{
    return m_connection && m_visualizationId >= 0
        && xmmsc_visualization_started(m_connection, m_visualizationId);
}

void xmms2::VisualizationClient::stop()
{
    if (m_connection && m_visualizationId >= 0) {
        xmmsc_visualization_shutdown(m_connection, m_visualizationId);
        m_visualizationId = -1;
    }
}

int xmms2::VisualizationClient::chunkGet(short *buffer, int drawTimeMs, unsigned int blockingMs)
{
    if (!isStarted())
        return 0;
    
    const int read = xmmsc_visualization_chunk_get(m_connection, m_visualizationId,
                                                   buffer, drawTimeMs, blockingMs);
    if (read < 0) {
        const char *error = xmmsc_get_last_error(m_connection);
        m_error = error ? error : "visualization data is not available";
    }
    return read;
}

const std::string& xmms2::VisualizationClient::lastError() const
{
    return m_error;
}

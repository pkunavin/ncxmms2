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

#include <xmmsclient/xmmsclient.h>
#include <string>

#include "result.h"
#include "../log.h"

using namespace ncxmms2;

xmms2::ResultBase::ResultBase(xmmsc_connection_t *connection, xmmsc_result_t *result) :
    m_connection(connection),
    m_result(result)
{
    xmmsc_ref(m_connection);
}

xmms2::ResultBase::~ResultBase()
{
    xmmsc_result_unref(m_result);
    xmmsc_unref(m_connection);
}

void xmms2::ResultBase::setResultCallback(int (*callback)(xmmsv_t *, void *), void *userData,
                                          void (*freeCallback)(void *))
{
    xmmsc_result_notifier_set_full(m_result, callback, userData, freeCallback);
}

void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (int)> &callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    int result;
    if (!xmmsv_get_int(value, &result)) {
         NCXMMS2_LOG_ERROR("xmmsv_get_int failed");
         return;
    }
    callback(result);
}

void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (xmms2::PlaybackStatus)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    int intStatus;
    if (!xmmsv_get_int(value, &intStatus)) {
         NCXMMS2_LOG_ERROR("xmmsv_get_int failed");
         return;
    }
    
    xmms2::PlaybackStatus status;
    switch (intStatus) {
        case XMMS_PLAYBACK_STATUS_STOP:
            status = xmms2::PlaybackStatus::Stopped;
            break;
            
        case XMMS_PLAYBACK_STATUS_PLAY:
            status = xmms2::PlaybackStatus::Playing;
            break;
            
        case XMMS_PLAYBACK_STATUS_PAUSE:
            status = xmms2::PlaybackStatus::Paused;
            break;
            
        default:
            return;
    }
    callback(status);
}


void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const xmms2::Dict&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    callback(Dict(value));
}

void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const xmms2::PropDict &)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    xmmsv_t *dict;
    dict = xmmsv_propdict_to_dict(value, NULL);
    callback(PropDict(dict));
    xmmsv_unref(dict);
}

void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (StringRef)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    const char *str = nullptr;
    if (!xmmsv_get_string(value, &str)) {
        NCXMMS2_LOG_ERROR("xmmsv_get_string failed");
        return;
    }
    callback(StringRef(str));
}

void xmms2::detail::decodeValue(xmmsv_t *value,
                                const std::function<void (const xmms2::PlaylistChangeEvent&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    PlaylistChangeEvent plsChangeEvent;
    if (plsChangeEvent.init(value))
        callback(plsChangeEvent);
}

void xmms2::detail::decodeValue(xmmsv_t *value,
                                const std::function<void (const xmms2::CollectionChangeEvent&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    CollectionChangeEvent collChangeEvent;
    if (collChangeEvent.init(value))
        callback(collChangeEvent);
}

void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const List<int>&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    callback(List<int>(value));
}


void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const List<StringRef>&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    callback(List<StringRef>(value));
}



void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const List<xmms2::Dict>&)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    callback(List<Dict>(value));
}


void xmms2::detail::decodeValue(xmmsv_t *value, const std::function<void (const xmms2::Collection &)>& callback)
{
    if (xmmsv_is_error(value)) {
        NCXMMS2_LOG_ERROR("value is an error");
        return;
    }
    
    xmmsv_coll_t *coll;
    if (!xmmsv_get_coll(value, &coll)) {
        NCXMMS2_LOG_ERROR("xmmsv_get_coll failed");
        return;
    }
    xmmsv_coll_ref(coll);
    callback(Collection(coll));
}

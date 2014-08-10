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

#ifndef RESULT_H
#define RESULT_H

#include <functional>
#include <vector>

#include "types.h"
#include "../lib/signals.h"

typedef struct xmmsc_connection_St xmmsc_connection_t;
typedef struct xmmsc_result_St xmmsc_result_t;
typedef struct xmmsv_St xmmsv_t;

namespace ncxmms2 {
namespace xmms2 {

namespace detail {
void decodeValue(xmmsv_t *value, const std::function<void (int)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (PlaybackStatus)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Dict&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const PropDict&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (StringRef)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const PlaylistChangeEvent&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const CollectionChangeEvent&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const List<int>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const List<StringRef>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const List<Dict>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Collection&)>& callback);
} // detail

template <typename T>
class XmmsValueFunctionWrapper
{
public:
    typedef std::function<void (T)> FunctionType;
    typedef int (*PlainFunctionType)(xmmsv_t*, void*);

    template <typename F>
    XmmsValueFunctionWrapper(F&& f) :
        m_function(std::forward<F>(f)) {}

    PlainFunctionType get() const {return &plainFunction;}

    static void free(void *ptr)
    {
        delete static_cast<XmmsValueFunctionWrapper*>(ptr);
    }
    
private:
    FunctionType m_function;

    static int plainFunction(xmmsv_t *value, void *data)
    {
        detail::decodeValue(value, static_cast<XmmsValueFunctionWrapper*>(data)->m_function);
        return 1;
    }
};

class ResultBase
{
    xmmsc_connection_t *m_connection;
    xmmsc_result_t *m_result;
    
public:
    ResultBase(const ResultBase&) = delete;
    ResultBase& operator=(const ResultBase&) = delete;
    
protected:
    ResultBase(xmmsc_connection_t *connection, xmmsc_result_t *result);
    ~ResultBase();
    
    void setResultCallback(int (*callback)(xmmsv_t*, void*), void *userData, 
                           void (*freeCallback)(void*));
};

template <typename T>
class Result : private ResultBase
{
public:
    Result(xmmsc_connection_t *connection, xmmsc_result_t *result) :
        ResultBase(connection, result) {}
    
    template <typename F>
    void operator()(F&& f)
    {
        auto *callback = new XmmsValueFunctionWrapper<T>(std::forward<F>(f));
        setResultCallback(callback->get(), callback, XmmsValueFunctionWrapper<T>::free);
    }
    
    template <typename Obj>
    void operator()(void (Obj::*func)(T), Obj *obj)
    {
        //   NOTE: Potentially unsafe, as there is no guarantee
        // that at the moment of callback call the object will be alive
        operator()(Signals::memFnBind(func, obj));
    }
    
    template <typename Obj, typename... Signature, typename Arg0, typename... Args>
    void operator()(void (Obj::*func)(Signature...), Obj *obj, Arg0&& arg0, Args&&... args)
    {
        //   NOTE: Potentially unsafe, as there is no guarantee
        // that at the moment of callback call the object will be alive
        operator()(std::bind(func, obj, std::forward<Arg0>(arg0), std::forward<Args>(args)...));
    }
};

template <>
class Result<void> : private ResultBase
{
public:
    Result(xmmsc_connection_t *connection, xmmsc_result_t *result) :
        ResultBase(connection, result) {}
};

typedef Result<void> VoidResult;
typedef Result<int> IntResult;
typedef Result<PlaybackStatus> PlaybackStatusResult;
typedef Result<const Dict&> DictResult;
typedef Result<const PropDict&> PropDictResult;
typedef Result<StringRef> StringResult;
typedef Result<const List<int>&> IntListResult;
typedef Result<const List<StringRef>&> StringListResult;
typedef Result<const List<Dict>&> DictListResult;
typedef Result<const Collection&> CollectionResult;

} // xmms2
} // ncxmms2

#endif // RESULT_H

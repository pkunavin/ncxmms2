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

#ifndef RESULT_H
#define RESULT_H

#include <functional>
#include <vector>
#include <string>
#include <type_traits>
#include <ostream>

#include "Types.h"
#include "../lib/Signals.h"

typedef struct xmmsc_connection_St xmmsc_connection_t;
typedef struct xmmsc_result_St xmmsc_result_t;
typedef struct xmmsv_St xmmsv_t;

namespace ncxmms2 {
namespace xmms2 {

class Error
{
public:
    template <typename Str>
    explicit Error(Str&& error) : m_error(std::forward<Str>(error)) {}
    
    const std::string& toString() const {return m_error;}
    
private:
    std::string m_error;
};

std::ostream& operator<<(std::ostream& os, const Error& error);

/*   Expected<T> is a variant type to represent that xmms2 can either
 * return a value (of type T) or an error (represented by string).
 * This class is inspired by Alexandrescu's Expected<T>.
 */
template <typename T>
class Expected
{
public:
    typedef T Type;
    
    Expected(const T& value) :
        m_value(value),
        m_isValid(true) {}
    
    Expected(T&& value) :
        m_value(std::move(value)),
        m_isValid(true) {}
    
    Expected(const Error& error) :
        m_error(error),
        m_isValid(false) {}
    
    Expected(Error&& error) :
        m_error(std::move(error)),
        m_isValid(false) {}
    
    bool isValid() const  {return m_isValid;}
    bool isError() const  {return !isValid();}
    
    T& value()
    {
        assert(isValid());
        return m_value;
    }
    
    const T& value() const
    {
        assert(isValid());
        return m_value;
    }
    
          T * operator->()       {return &value();}
    const T * operator->() const {return &value();}
    
          T& operator*()       {return value();}
    const T& operator*() const {return value();}
    
    const Error& error() const
    {
        assert(isError());
        return m_error;
    }
    
    ~Expected()
    {
        destruct();
    }
    
    Expected(const Expected& other) = delete;
    Expected& operator=(const Expected& other) = delete;
    
    Expected(Expected&& other) noexcept:
        m_isValid(other.m_isValid)
    {
        construct(std::move(other));
    }
    
    Expected& operator=(Expected&& other) noexcept
    {
        if (this != &other) {
            destruct();
            m_isValid = other.m_isValid;
            construct(std::move(other));
        }
        return *this;
    } 
    
private:
    union
    {
        T m_value;
        Error m_error;
    };
    bool m_isValid;
    
    void construct(Expected&& other)
    {
        if (m_isValid) {
            new (&m_value) T(std::move(other.m_value));
        } else {
            new (&m_error) Error(std::move(other.m_error));
        }
    }
    
    void destruct()
    {
        if (m_isValid) {
            m_value.~T();
        } else {
            m_error.~Error();
        }
    }
    
};

template <typename T>
inline Expected<typename std::decay<T>::type> expectedFromValue(T&& value)
{
    return Expected<typename std::decay<T>::type>(std::forward<T>(value));
}

template <typename T, typename... Args>
inline Expected<T> expectedConstructValue(Args&&... args)
{
    return Expected<T>(T(std::forward<Args>(args)...));
}

template <typename T, typename Str>
inline Expected<T> expectedFromError(Str&& error)
{
    return Expected<T>(Error(std::forward<Str>(error)));
}

namespace detail {
StringRef getErrorString(xmmsv_t *value);

template <typename T>
void decodeValue(xmmsv_t *value, const std::function<void (const Expected<T>&)>& callback)
{
    callback(T(value));
}

void decodeValue(xmmsv_t *value, const std::function<void (const Expected<int>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Expected<PlaybackStatus>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Expected<PropDict>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Expected<StringRef>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const Expected<Collection>&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const PlaylistChangeEvent&)>& callback);
void decodeValue(xmmsv_t *value, const std::function<void (const CollectionChangeEvent&)>& callback);
} // detail

template <typename T>
class XmmsValueFunctionWrapper;

template <typename T>
class XmmsValueFunctionWrapper<const T&>
{
public:
    typedef std::function<void (const T&)> FunctionType;
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
        auto *wrapper = static_cast<XmmsValueFunctionWrapper*>(data);
        detail::decodeValue(value, wrapper->m_function);
        return 1;
    }
};

template <typename T>
class XmmsValueFunctionWrapper<const Expected<T>&>
{
public:
    typedef std::function<void (const Expected<T>&)> FunctionType;
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
        auto *wrapper = static_cast<XmmsValueFunctionWrapper*>(data);
        const StringRef error = detail::getErrorString(value);
        if (!error.isNull()) {
            wrapper->m_function(Error(error.c_str()));
        } else {
            detail::decodeValue(value, wrapper->m_function);
        }
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

typedef Result<void>                             VoidResult;
typedef Result<const Expected<int>&>             IntResult;
typedef Result<const Expected<PlaybackStatus>&>  PlaybackStatusResult;
typedef Result<const Expected<Dict>&>            DictResult;
typedef Result<const Expected<PropDict>&>        PropDictResult;
typedef Result<const Expected<StringRef>&>       StringResult;
typedef Result<const Expected<List<int>>&>       IntListResult;
typedef Result<const Expected<List<StringRef>>&> StringListResult;
typedef Result<const Expected<List<Dict>>&>      DictListResult;
typedef Result<const Expected<Collection>&>      CollectionResult;

} // xmms2
} // ncxmms2

#endif // RESULT_H

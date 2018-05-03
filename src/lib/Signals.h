/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef SIGNALS_H
#define SIGNALS_H

/**
    This file incudes a slot-signal idea realization.

    Using signals and slots:
  1. Declaring signals
    To declare a signal use NCXMMS2_SIGNAL macro. But be careful it is macro, which
  adds a protected signal member to your class and some public functions for signal
  connection managment. For example, if you want to declare signal without para-
  meters, you should add to a class definition:
    NCXMMS2_SIGNAL(someVoidSignal)
  Now you can emit signal in your class (or class friends) member functions, just
  like in Qt, but without emit macro (yes, emit is a macro, which means nothing):
    someVoidSignal();
  To declare a signal with parameters, pass them to NCXMMS2_SIGNAL macro, for
  example, signal with int and string parameters:
    NCXMMS2_SIGNAL(someSignal, int, const std::string&)

  2. Connecting to signal
    - Stand-alone function
        if you have stand-alone function:
            void someFunction();
        and a signal in object:
            NCXMMS2_SIGNAL(someSignal)
        Connecting is easy:
            object.someSignal_Connect(someFunction);

     - Function object or lambda function
        Just like previous case, for signal:
            NCXMMS2_SIGNAL(someSignal, int)
        Connecting:
        object.someSignal_Connect([](int x){do_something();});

     - Arbitrary class member function
        Use boost::bind or std::bind for this purpose.
        Example:
          class ObjectDerivedClass : public Object
          {
           ...
           NCXMMS2_SIGNAL(someVoidSignal);
           NCXMMS2_SIGNAL(someIntSignal, int);
           ...
          };

          struct X
          {
              void f1();
              void f2(int a);
          };

          X x;
          ObjectDerivedClass obj;

          obj.someVoidSignal_Connect(std::bind(&X:f1, &x));
          obj.someIntSignal_Connect(std::bind(&X::f2, &x, std::placeholders::_1));

          // Use 10 as an argument for f2
          obj.someVoidSignal_Connect(std::bind(&X:f2, &x, 10));

      - ncxmms2::Object derived class member function
         For ncxmms2::Object derived classes simplified syntax and also automatic
         disconnecting on object destruction are provided.

        Example:
          class SignalEmitter : public Object
          {
           ...
           NCXMMS2_SIGNAL(someVoidSignal);
           NCXMMS2_SIGNAL(someIntSignal, int);
           NCXMMS2_SIGNAL(someIntIntSignal, int, int);
           ...
          };

          class SlotsHolder : public Object
          {
           ...
          public:
              void f1();
              void f2(int a);
              void f3(int a, int b);
           ...
          };

          SignalEmitter obj1;
          SlotsHolder obj2;

          // Simplified syntax:
          obj1.someVoidSignal_Connect(&SlotsHolder::f1, &obj2);
          obj1.someIntSignal_Connect(&SlotsHolder::f2, &obj2);
          obj1.someIntIntSignal_Connect(&SlotsHolder::f3, &obj2);


          // If your member function has more than five args or you need
             to bind something, use general syntax (std::bind syntax):
          obj1.someIntSignal_Connect(&SlotsHolder::f2, &obj2, std::placeholders::_1);
          obj1.someVoidSignal_Connect(&SlotsHolder::f2, &obj2, 10);
          obj1.someIntIntSignal_Connect(&SlotsHolder::f3, &obj2, std::placeholders::_1, std::placeholders::_2);
          obj1.someIntSignal_Connect(&SlotsHolder::f3, &obj2, 10, std::placeholders::_1);

 */

#include <cstdint>
#include <utility>
#include <vector>
#include <functional>
#include <type_traits>

#include "../../3rdparty/folly/sorted_vector_types.h"

namespace ncxmms2 {
class Object;

namespace Signals {

class SignalBase;

class Connection
{
    friend class SignalBase;
    uint32_t m_id;
    explicit Connection(uint32_t id) : m_id(id) {}
    
public:
    Connection() : m_id(0) {}
    
    void disconnect();
    bool isConnected() const;
    
    void block();
    void unblock();
    bool isBlocked() const;
    
    bool operator==(const Connection& other) const
    {
        return m_id == other.m_id;
    }
    
    bool operator<(const Connection& other) const
    {
        return m_id < other.m_id;
    }
    
    bool operator>(const Connection& other) const
    {
        return m_id > other.m_id;
    }
};

class ScopedConnectionBlock
{
    Connection m_connection;
public:
    explicit ScopedConnectionBlock(Connection connection) :
        m_connection(connection)
    {
        m_connection.block();
    }

    ~ScopedConnectionBlock()
    {
        m_connection.unblock();
    }
    
    ScopedConnectionBlock(const ScopedConnectionBlock& other) = delete;
    ScopedConnectionBlock& operator=(const ScopedConnectionBlock& other) = delete;
};

class SignalBase
{
public:
    SignalBase(){}
    SignalBase(const SignalBase& other) = delete;
    SignalBase& operator=(const SignalBase& other) = delete;
    virtual ~SignalBase();
    
protected:
    virtual void eraseConnection(Connection connection) = 0;
    virtual void blockConnection(Connection connection, bool block) = 0;
    virtual bool isBlockedConnection(Connection connection) = 0;
    
    static Connection creatConnection(SignalBase *signal);
    static void destroyConnection(Connection connection);
    
    friend class Connection;
};

template <typename F, typename T>
class MemFnBindImpl
{
    F f;
    T *m_obj;
public:
    template <typename U>
    MemFnBindImpl(U&& memFnResult, T *obj) : 
        f(std::forward<U>(memFnResult)),
        m_obj(obj) {}
    
    template <typename... Args>
    void operator()(Args&&... args)
    {
        f(*m_obj, std::forward<Args>(args)...);
    }
};

template <typename T, typename... Signature>
inline auto memFnBind(void (T::*func)(Signature...), T *obj)
-> MemFnBindImpl<decltype(std::mem_fn(func)), T>
{
    return MemFnBindImpl<decltype(std::mem_fn(func)), T>(std::mem_fn(func), obj);
}

template <typename... Args>
class Signal : public SignalBase
{
    struct Slot
    {
        std::function<void (Args...)> func;
        bool blocked;
        
        template <typename F>
        Slot(F&& f) :
            func(std::forward<F>(f)),
            blocked(false) {}
    };
    
    folly::sorted_vector_map<Connection, Slot> m_slots;
    
protected:
    virtual void eraseConnection(Connection connection)
    {
        m_slots.erase(connection);
    }
    
    virtual void blockConnection(Connection connection, bool block)
    {
        auto it = m_slots.find(connection);
        if (it != m_slots.end()) {
            it->second.blocked = block;
        }
    }
    
    virtual bool isBlockedConnection(Connection connection)
    {
        auto it = m_slots.find(connection);
        return it != m_slots.end() ? it->second.blocked : false;
    }

public:
    template <typename F>
    Connection connect(F&& f)
    {
        Connection conn = creatConnection(this);
        m_slots.insert(std::make_pair(conn, std::forward<F>(f)));
        return conn;
    }
    
    template <typename T>
    typename std::enable_if<std::is_base_of<Object, T>::value, Connection>::type
    connect(void (T::*func)(Args...), T *obj)
    {
        Connection conn = connect(memFnBind(func, obj));
        obj->registerConnection(conn);
        return conn;
    }
    
    template <typename T>
    typename std::enable_if<!std::is_base_of<Object, T>::value, Connection>::type
    connect(void (T::*func)(Args...), T *obj)
    {
        return connect(memFnBind(func, obj));
    }
    
    template <typename T, typename... Signature, typename T0, typename... Ts>
    typename std::enable_if<std::is_base_of<Object, T>::value, Connection>::type
    connect(void (T::*func)(Signature...), T *obj, T0&& t0, Ts&&... ts)
    {
        Connection conn = connect(std::bind(func, obj, std::forward<T0>(t0), std::forward<Ts>(ts)...));
        obj->registerConnection(conn);
        return conn;
    }
    
    template <typename T, typename... Signature, typename T0, typename... Ts>
    typename std::enable_if<!std::is_base_of<Object, T>::value, Connection>::type
    connect(void (T::*func)(Signature...), T *obj, T0&& t0, Ts&&... ts)
    {
        return connect(std::bind(func, obj, std::forward<T0>(t0), std::forward<Ts>(ts)...));
    }
    
    void operator()(Args... args)
    {
        for (auto& slot : m_slots) {
            if (!slot.second.blocked)
                slot.second.func(args...);
        }
    }
    
    ~Signal()
    {
        for (auto& slot : m_slots)
            destroyConnection(slot.first);
    }
};


} // Signals
} // ncxmms2

/* The only purpose of this macro is to allow to emit signal only from the class
 * that defines the signal and its subclasses.
 */
#define NCXMMS2_SIGNAL(NAME, ...)                             \
    protected:                                                \
        Signals::Signal<__VA_ARGS__> NAME;                    \
    public:                                                   \
        template <typename... Args>                           \
        Signals::Connection NAME ## _Connect(Args&&... args)  \
        {                                                     \
            return NAME.connect(std::forward<Args>(args)...); \
        }                                                     \

#endif // SIGNALS_H

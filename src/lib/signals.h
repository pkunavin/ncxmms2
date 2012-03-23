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

#ifndef SIGNALS_H
#define SIGNALS_H

/**
    This file incudes a slot-signal idea realization using boost::signals2
  library.

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

          obj.someVoidSignal_Connect(boost::bind(&X:f1, &x));
          obj.someIntSignal_Connect(boost::bind(&X::f2, &x, _1));

          // Use 10 as an argument for f2
          obj.someVoidSignal_Connect(boost::bind(&X:f2, &x, 10));

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
             to bind something, use general syntax (boost::bind syntax):
          obj1.someIntSignal_Connect(&SlotsHolder::f2, &obj2, _1);
          obj1.someVoidSignal_Connect(&SlotsHolder::f2, &obj2, 10);
          obj1.someIntIntSignal_Connect(&SlotsHolder::f3, &obj2, _1, _2);
          obj1.someIntSignal_Connect(&SlotsHolder::f3, &obj2, 10, _1);

 */

#include <boost/signals2.hpp>
#include <utility>

namespace ncxmms2 {
namespace Signals = boost::signals2;
class Object;
}

#define NCXMMS2_SIGNAL(NAME, ...) \
    protected: \
        ncxmms2::Signals::signal<void (__VA_ARGS__)> NAME; \
    public: \
        ncxmms2::Signals::connection NAME ## _Connect(const ncxmms2::Signals::signal<void (__VA_ARGS__)>::slot_type& slot) \
        { \
            return NAME.connect(slot); \
        } \
        template <typename T> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename Arg0> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Arg0), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, _1)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename Arg0, typename Arg1> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Arg0, Arg1), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, _1, _2)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename Arg0, typename Arg1, typename Arg2> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Arg0, Arg1, Arg2), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, _1, _2, _3)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Arg0, Arg1, Arg2, Arg3), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, _1, _2, _3, _4)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Arg0, Arg1, Arg2, Arg3, Arg4), ncxmms2::Object *obj) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, _1, _2, _3, _4, _5)); \
            obj->registerConnection(conn); \
            return conn; \
        } \
        template <typename T, typename ... Signature, typename Arg, typename ... Args> \
        ncxmms2::Signals::connection NAME ## _Connect(void (T::*func)(Signature ...), ncxmms2::Object *obj, Arg&& arg0, Args&& ... args) \
        { \
            ncxmms2::Signals::connection conn = NAME.connect(boost::bind(func, (T*)obj, std::forward<Arg>(arg0), std::forward<Args>(args)...)); \
            obj->registerConnection(conn); \
            return conn; \
        } \

#endif // SIGNALS_H

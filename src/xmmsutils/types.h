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

#ifndef TYPES_H
#define TYPES_H

#include <xmmsclient/xmmsclient.h>

#include <string>
#include <functional>
#include <assert.h>

#include "../lib/stringref.h"

namespace ncxmms2 {
namespace xmms2 {

/* **************************************
   ********* PlaybackStatus* ************
   ************************************** */
enum class PlaybackStatus
{
    Playing,
    Stopped,
    Paused
};

/* **************************************
   ************** Dict ******************
   ************************************** */
class Variant
{
public:
    explicit Variant(xmmsv_t *value) : m_value(value) {}
    
    enum class Type
    {
        None,
        Int,
        String
    };
    
    Type type() const;
    
    template <typename T>
    T value(T defaultValue = T()) const
    {
        getValue(&defaultValue);
        return defaultValue;
    }
    
private:
    xmmsv_t *m_value;
    
    void getValue(int *value) const;
    void getValue(std::string *value) const;
    void getValue(StringRef *value) const;
};

class Dict
{
public:
    Dict() : m_dict(nullptr) {}
    explicit Dict(xmmsv_t *dict);
    ~Dict();
    
    Dict(const Dict&) = delete;
    Dict& operator=(const Dict&) = delete;
    
    Dict(Dict&& other) noexcept :
        m_dict(other.m_dict)
    {
        other.m_dict = nullptr;
    }
    
    Dict& operator=(Dict&& other) noexcept
    {
        if (this != &other) {
            m_dict = other.m_dict;
            other.m_dict = nullptr;
        }
        return *this;
    }
    
    Variant operator[](const char *key) const;
    
    Variant operator[](const std::string& key) const
    {
        return operator[](key.c_str());
    }
    
    template <typename T>
    T value(const char *key, T defaultValue = T()) const
    {
        return operator[](key).value<T>(defaultValue);
    }
    
    template <typename T>
    T value(const std::string& key, T defaultValue = T()) const
    {
        return value<T>(key.c_str(), &defaultValue);
    }
    
    typedef std::function<void (StringRef,  Variant)> ForEachFunction;
    void forEach(const ForEachFunction& func) const;
    
private:
    xmmsv_t *m_dict;
    
    static void forEachPlain(const char *key, xmmsv_t *value, void *func);
};

class PropDict : public Dict
{
public:
    explicit PropDict(xmmsv_t *dict) : Dict(dict) {}
};

/* **************************************
   ************** List ******************
   ************************************** */
class ListIteratorBase
{
public:
    explicit ListIteratorBase(xmmsv_t *list);
    
    bool isValid() const;
    void next();
    
protected:
    xmmsv_list_iter_t *m_it;
    
    bool getValue(int *val) const;
    bool getValue(StringRef *val) const;
    bool getValue(Dict *val) const;
};

template <typename T>
class ListIterator : public ListIteratorBase
{
public:
    explicit ListIterator(xmmsv_t *list) : ListIteratorBase(list) {}
    
    T value(bool *ok) const
    {
        T val;
        *ok = getValue(&val);
        return val;
    }
};

class ListBase
{
public:
    explicit ListBase(xmmsv_t *list);
    ~ListBase();
    
    ListBase(const ListBase&) = delete;
    ListBase& operator=(const ListBase&) = delete;
    
    ListBase(ListBase&& other) noexcept :
        m_list(other.m_list)
    {
        other.m_list = nullptr;
    }
    
    ListBase& operator=(ListBase&& other) noexcept
    {
        if (this != &other) {
            m_list = other.m_list;
            other.m_list = nullptr;
        }
        return *this;
    }
    
    int size() const;
    
protected:
     xmmsv_t *m_list;
};

template <typename T>
class List : public ListBase
{
public:
    explicit List(xmmsv_t *list) : ListBase(list) {}
    
    ListIterator<T> getIterator() const {return ListIterator<T>(m_list);}
};

/* **************************************
   ******** PlaylistChangeEvent *********
   ************************************** */
class PlaylistChangeEvent
{
public:
    enum class Type
    {
        Unknown,
        Add,
        Insert,
        Remove,
        Move,
        Replace
    };

    PlaylistChangeEvent() :
        m_type(Type::Unknown),
        m_id(-1),
        m_position(-1),
        m_newPosition(-1)
    {
    }

    Type type() const          {return m_type;}

    const std::string& playlist() const {return m_playlist;}

    int id() const
    {
        assert(m_type == Type::Add || m_type == Type::Insert);
        return m_id;
    }

    int position() const
    {
        assert(m_type == Type::Add
            || m_type == Type::Insert
            || m_type == Type::Move
            || m_type == Type::Remove);
        return m_position;
    }

    int newPosition() const
    {
        assert(m_type == Type::Move);
        return m_newPosition;
    }

    bool init(xmmsv_t *dict);
    
private:
    std::string m_playlist;
    Type m_type;
    int m_id;
    int m_position;
    int m_newPosition;
};

/* **************************************
   ******* CollectionChangeEvent ********
   ************************************** */
class CollectionChangeEvent
{
public:
    enum class Type
    {
        Unknown,
        Add,
        Update,
        Rename,
        Remove
    };
    
    CollectionChangeEvent() :
        m_type(Type::Unknown)
    {
    }

    Type type() const {return m_type;}
    
    const std::string& kind() const {return m_kind;}
    
    const std::string& name() const {return m_name;}
    
    const std::string& newName() const
    {
        assert(m_type == Type::Rename);
        return m_newName;
    }
    
    bool init(xmmsv_t *dict);
    
private:
    std::string m_kind;
    std::string m_name;
    std::string m_newName;
    Type m_type;
};

/* **************************************
   *********** Collection ***************
   ************************************** */
class Collection
{
public:
    enum class Type
    {
        Unknown,
        Reference,
        Union,
        Intersection,
        Complement,
        Has,
        Equals,
        Match,
        Smaller,
        Greater,
        Idlist
    };

    Collection(Type type);
    explicit Collection(xmmsv_coll_t *coll);
    ~Collection();
    
    Collection(const Collection&) = delete;
    Collection& operator=(const Collection&) = delete;
    
    Collection(Collection&& other) noexcept :
        m_coll(other.m_coll),
        m_type(other.m_type)
    {
        other.m_coll = nullptr;
    }
    
    Collection& operator=(Collection&& other) noexcept
    {
        if (this != &other) {
            m_coll = other.m_coll;
            m_type = other.m_type;
            other.m_coll = nullptr;
        }
        return *this;
    }
    
    void setAttribute(const std::string& key, const std::string& value);
    void addOperand(const Collection& operand);

    // For Idlist type
    int size() const;
    int at(int index) const;
    
    static Collection universe();
    static Collection allByArtist(const std::string& artist, const Collection& source = universe());
    static Collection albumByArtist(const std::string& artist, const std::string& album,
                                    const Collection& source = universe());

private:
    friend class Client;
    xmmsv_coll_t *m_coll;
    Type m_type;
};

/* **************************************
   ************* Misc *******************
   ************************************** */
std::string decodeUrl(const char *url);
std::string getFileNameFromUrl(const std::string& url); // url is decoded

} // xmms2
} // ncxmms2

#endif // TYPES_H

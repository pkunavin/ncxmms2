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

#ifndef STRINGPTR_H
#define STRINGPTR_H

#include <glib.h>
#include <cstring>
#include <string>

namespace ncxmms2 {

/*  StringPtr is a pointer to a string (or a part of the string) which implements
 * different owning policies. Available policies:
 *  - NonOwn: Non owning policy. In this mode StringPtr is similiar to boost::string_ref.
 *  - OwnCopy: StringPtr will take the ownership of the copy of the string.
 */
class StringPtr
{
public:
    enum class Policy
    {
        NonOwn,
        OwnCopy
    };
    
    StringPtr() :
        m_policy(Policy::NonOwn),
        m_begin(nullptr),
        m_end(nullptr)
    {
        
    }
    
    StringPtr(Policy policy, const char *begin, const char *end) :
        m_policy(Policy::NonOwn),
        m_begin(nullptr),
        m_end(nullptr)
        
    {
        assign(policy, begin, end);
    }
    
    StringPtr(Policy policy, const char *str) :
        m_policy(Policy::NonOwn),
        m_begin(nullptr),
        m_end(nullptr)
    {
        assign(policy, str);
    }
    
    StringPtr(Policy policy, const std::string& str) :
        m_policy(Policy::NonOwn),
        m_begin(nullptr),
        m_end(nullptr)
    {
        assign(policy, str);
    }
    
    ~StringPtr()
    {
        release();
    }
    
    void assign(Policy policy, const char *begin, const char *end)
    {
        release();
        m_policy = policy;
        switch (m_policy) {
            case Policy::NonOwn:
                m_begin = (char*)begin;
                m_end = (char*)end;
                break;
            
            case Policy::OwnCopy:
            {
                size_t n = end - begin;
                m_begin = g_strndup(begin, n);
                m_end = m_begin ? m_begin + n : nullptr;
                break;
            }
        }
    }
    
    void assign(Policy policy, const char *str)
    {
        release();
        m_policy = policy;
        switch (m_policy) {
            case Policy::NonOwn:
                m_begin = (char*)str;
                m_end = m_begin ? m_begin + std::strlen(str) : nullptr;
                break;
            
            case Policy::OwnCopy:
            {
                size_t n = str ? std::strlen(str) : 0;
                m_begin = g_strndup(str, n);
                m_end = m_begin ? m_begin + n : nullptr;
                break;
            }
        }
    }
    
    void assign(Policy policy, const std::string& str)
    {
        release();
        m_policy = policy;
        switch (m_policy) {
            case Policy::NonOwn:
                m_begin = (char*)str.c_str();
                m_end = m_begin ? m_begin + str.size() : nullptr;
                break;
            
            case Policy::OwnCopy:
                m_begin = g_strndup(str.c_str(), str.size());
                m_end = m_begin ? m_begin + str.size() : nullptr;
                break;
        }
    }
    
    void release()
    {
        switch (m_policy) {
            case Policy::NonOwn:
                break;
            
            case Policy::OwnCopy:
                g_free(m_begin);
                break;
        }
        m_begin = nullptr;
        m_end = nullptr;
    }
    
    const char *begin() const {return m_begin;}
    const char *end() const   {return m_end;}
    
    bool isEmpty() const {return m_begin == m_end;}
    
    size_t size() const {return m_end - m_begin;}
    
    StringPtr(const StringPtr& other) = delete;
    StringPtr& operator=(const StringPtr& other) = delete;
    
    StringPtr(StringPtr&& other) noexcept :
        m_policy(other.m_policy),
        m_begin(other.m_begin),
        m_end(other.m_end)
    {
        other.m_policy = Policy::NonOwn;
        other.m_begin = nullptr;
        other.m_end = nullptr;
    }
    
    StringPtr& operator=(StringPtr&& other) noexcept
    {
        if (this != &other) {
            release();
            
            m_policy = other.m_policy;
            m_begin = other.m_begin;
            m_end = other.m_end;
            
            other.m_policy = Policy::NonOwn;
            other.m_begin = nullptr;
            other.m_end = nullptr;
        }
        return *this;
    }
    
private:
    Policy m_policy;
    char *m_begin;
    char *m_end;
};
} // ncxmms2

#endif // STRINGPTR_H

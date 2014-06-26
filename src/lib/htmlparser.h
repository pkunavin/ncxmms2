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

#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <string>
#include <cstring>
#include <vector>
#include <utility>
#include <assert.h>

#include "../../3rdparty/folly/sorted_vector_types.h"

namespace ncxmms2 {

/*  The purpose of HtmlParser class is to tokenize HTML document, i.e. it doesn't
 * build DOM tree. 
 */ 
class HtmlParser
{
public:
    class TokenText
    {
    public:
        template <typename... Args>
        TokenText(Args&&... args) :
            m_text(std::forward<Args>(args)...) {}
        
        const std::string& text() const {return m_text;}
        
        TokenText(const TokenText& other) = delete;
        TokenText& operator=(const TokenText& other) = delete;
        TokenText(TokenText&&) = default;
        TokenText& operator=(TokenText&&) = default;
        
    private:
        std::string m_text;
    };
    
    class TokenTag
    {
    public:
        enum class Type
        {
            Bold,       // <b>
            Underline,  // <u>
            Blink,      // <blink>
            Font,       // <font>
            
            Heading_1,  // <h1>
            Heading_2,  // <h2>
            Heading_3,  // <h3>
            Heading_4,  // <h4>
            Heading_5,  // <h5>
            Heading_6,  // <h6>
            
            LineBreak,  // <br>
            Paragraph,  // <p>
            Division,   // <div>
            Article,    // <article>
            BlockQuote, // <blockquote>
            
            ListItem,   // <li>
            
            PreformattedText, // <pre>
            
            Quotation, // <q>
            
            Script, // <script>
            Style,  // <style>
            
            Title,  // <title>
            
            Unknown
        };
        
        TokenTag(const char *p, const char *tagEnd);
        
        Type type() const       {return m_type;}
        bool isStartTag() const {return m_isStartTag;}
        bool isEndTag() const   {return !m_isStartTag;}
        
        bool hasAttribute(const std::string& attr) const;
        const std::string& attribute(const std::string& attr) const;
        
        TokenTag(const TokenTag& other) = delete;
        TokenTag& operator=(const TokenTag& other) = delete;
        TokenTag(TokenTag&& other) :
            m_type(other.m_type),
            m_isStartTag(other.m_isStartTag)
        {
            m_attributes.swap(other.m_attributes);
        }
        TokenTag& operator=(TokenTag&& other)
        {
            if (this != &other) {
                m_type = other.m_type;
                m_isStartTag = other.m_isStartTag;
                m_attributes.swap(other.m_attributes);
            }
            return *this;
        }

    private:
        Type m_type;
        bool m_isStartTag;
        folly::sorted_vector_map<std::string, std::string> m_attributes;
    };
     
    class TokenEntity
    {
    public:
        TokenEntity(const std::string& entity);
        
        bool isKnown() const       {return m_character;}
        char32_t character() const {return m_character;}
        const char *utf8() const   {return m_utf8;}
        
        TokenEntity(const TokenEntity& other) = delete;
        TokenEntity& operator=(const TokenEntity& other) = delete;
        TokenEntity(TokenEntity&& other) :
            m_character(other.m_character)
        {
            std::strcpy(m_utf8, other.m_utf8);
        }
        TokenEntity& operator=(TokenEntity&& other)
        {
            if (this != &other) {
                 m_character = other.m_character;
                 std::strcpy(m_utf8, other.m_utf8);
            }
            return *this;
        }

    private:
        char32_t m_character;
        char m_utf8[6 + 1];
    };
    
    class Token
    {
    public:
        enum class Type
        {
            Text,
            Tag,
            Entity
        };
        
        Type type() const {return m_type;}
        
        const TokenText& text() const
        {
            assert(m_type == Type::Text);
            return m_tokenText;
        }
        
        const TokenTag& tag() const
        {
            assert(m_type == Type::Tag);
            return m_tokenTag;
        }
        
        const TokenEntity& entity() const
        {
            assert(m_type == Type::Entity);
            return m_tokenEntity;
        }
        
        Token(const Token& other) = delete;
        Token& operator=(const Token& other) = delete;
        Token(Token&& other) noexcept :
            m_type(other.m_type)
        {
            construct(std::move(other));
        }
        Token& operator=(Token&& other) noexcept
        {
            if (this != &other) {
                destruct();
                m_type = other.m_type;
                construct(std::move(other));
            }
            return *this;
        }

        class TextTypeTag{};
        class TagTypeTag{};
        class EntityTypeTag{};
        
        template <typename... Args>
        Token(class TextTypeTag, Args&&... args) :
            m_type(Type::Text),
            m_tokenText(std::forward<Args>(args)...) {}
        
        template <typename... Args>
        Token(class TagTypeTag, Args&&... args) :
            m_type(Type::Tag),
            m_tokenTag(std::forward<Args>(args)...) {}
        
        template <typename... Args>
        Token(class EntityTypeTag, Args&&... args) :
            m_type(Type::Entity),
            m_tokenEntity(std::forward<Args>(args)...) {}
        
        ~Token()
        {
            destruct();
        }
        
    private:
        Type m_type;
        union
        {
            TokenText m_tokenText;
            TokenTag m_tokenTag;
            TokenEntity m_tokenEntity;
        };
        
        void construct(Token&& other)
        {
            switch (m_type) {
                case Type::Text:
                    new (&m_tokenText) TokenText(std::move(other.m_tokenText));
                    break;
                case Type::Tag:
                    new (&m_tokenTag) TokenTag(std::move(other.m_tokenTag));
                    break;
                case Type::Entity:
                    new (&m_tokenEntity) TokenEntity(std::move(other.m_tokenEntity));
                    break;
            }
        }
        
        void destruct()
        {
            switch (m_type) {
                case Type::Text:
                    m_tokenText.~TokenText();
                    break;
                case Type::Tag:
                    m_tokenTag.~TokenTag();
                    break;
                case Type::Entity:
                    m_tokenEntity.~TokenEntity();
                    break;
            }
        }
    };
    
    HtmlParser() {}
    
    void parse(const std::string& text);
    
    std::vector<Token>::const_iterator begin() const {return m_tokens.cbegin();}
    std::vector<Token>::const_iterator end() const   {return m_tokens.cend();}
    
    static std::string encodeEntities(const std::string& str);
    
private:
    std::vector<Token> m_tokens;
};
} // ncxmms2

#endif // HTMLPARSER_H

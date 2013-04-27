/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2013 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef SONGDISPLAYFORMATPARSER_H
#define SONGDISPLAYFORMATPARSER_H

#include <vector>
#include <assert.h>

#include "song.h"
#include "utils.h"

#include "lib/colors.h"
#include "lib/rectangle.h"

typedef struct _GRegex GRegex;

namespace ncxmms2 {

class Painter;

/*   SongDisplayFormatParser class paints song meta-data according to format string.
  Format string specification:
     Any playlist consists of columns, think of it as a container for displaying
  info. Each column has the following properties: alignment, size factor, size.
  Alignment defines how displayed info inside column will be aligned. Size factor
  determines how much space can column occupy compared to other column. If you
  want column to have fixed size, its size factor should be set to zero and size
  do desired size. Column is declared using the following syntax:
  [alignment:size factor:size],
  where alignment can be r (right), l (left), c (center)
  size factor - integer number, zero for fixed size column
  size - integer number, when column is not fixed this value doesn't matter
     After column specification display format specification should follow.
  It can contain variables, color settings, regular text and section braces.
   List of variables:
    $a - artist
    $t - title
    $b - album
    $p - performer
    $y - date
    $g - genre
    $f - file name
    $F - file path
    $i - id
    $n - track number
    $N - times played number
    $l - length
    $B - bitrate
    $S - samplerate
  Colors are specified as %Nc, where N is the color number.
   List of availiable colors:
    1 - Black
    2 - Red
    3 - Green
    4 - Yellow
    5 - Blue
    6 - Magenta
    7 - Cyan
    8 - White
  Any part of format specification string can be put in section braces {}.
  Section is printed only when all variables which it contains are not empty.
  Section can also be connected with or relation {}|{}, i.e first not empty
  section will be printed.
    Examples:
  Default format:
   [l:1:0]%4c{$a - $t}|{$t}|{$f}[r:0:10]{%3c($l)}
  Multiple column layout:
   [r:0:3]$n[l:3:0] {%2c$t}|{%2c$f}[c:2:0] %3c$a[c:2:0] %4c$b[c:2:0] %6c$g[c:0:10] $y[r:0:10]{%7c($l)}
 */

class SongDisplayFormatParser
{
public:
    SongDisplayFormatParser();

    bool setDisplayFormat(const std::string& formatString);

    const std::string& errorString() const {return m_errorString;}

    void paint(const Song& song, Painter *painter, const Rectangle& rect, bool ignoreColors = false);

    bool matchFormattedString(const Song& song, const GRegex *regex); // Used to select items in PlaylistView

private:
    class Variable
    {
    public:
        Variable() :
            m_type(Type::None) {}

        bool init(char key);
        bool isEmpty(const Song& song) const;
        int size(const Song& song) const;
        void print(Painter *painter, const Song& song, int maxLength) const;
        std::string toString(const Song& song) const;

    private:
        enum class Type
        {
            None,
            StringRef,
            String,
            Integer
        };

        Type m_type;
        union
        {
            const std::string& (Song::*m_songStrRefFuncPtr)() const;
            int (Song::*m_songIntFuncPtr)() const;
            std::string (*m_stringFuncPtr)(const Song&);
        };

        static std::string durationStringGenerator(const Song& song);
        static std::string bitrateStringGenerator(const Song& song);
        static std::string samplerateStringGenerator(const Song& song);
    };

    class FormatToken
    {
    public:
        enum class Type
        {
            None,
            Character,
            Variable,
            Color
        };

        typedef const std::string& (Song::*SongStrFuncPtr)() const;

        FormatToken() :
            m_type(Type::None) {}

        Type type() const {return m_type;}

        char character() const
        {
            assert(m_type == Type::Character);
            return m_character;
        }

        const Variable& variable() const
        {
            assert(m_type == Type::Variable);
            return m_variable;
        }

        Color color() const
        {
            assert(m_type == Type::Color);
            return m_color;
        }

        void setType(Type type) {m_type = type;}

        void setCharacter(char ch)
        {
            assert(m_type == Type::Character);
            m_character = ch;
        }

        bool setVariable(char key)
        {
            assert(m_type == Type::Variable);
            return m_variable.init(key);
        }

        void setColor(Color color)
        {
            assert(m_type == Type::Color);
            m_color = color;
        }

    private:
        Type m_type;
        union
        {
            char m_character;
            Variable m_variable;
            Color m_color;
        };
    };

    class Token
    {
    public:
        enum class Type
        {
            SectionOpenBrace,
            SectionCloseBrace,
            SectionOr,

            Format
        };

        Token(Type type) :
            m_type(type) {}

        Type type() const {return m_type;}

        FormatToken& formatToken()
        {
            assert(m_type == Type::Format);
            return m_formatToken;
        }

        const FormatToken& formatToken() const
        {
            assert(m_type == Type::Format);
            return m_formatToken;
        }

    private:
        Type m_type;
        FormatToken m_formatToken;
    };

    template <typename Iterator>
    class FormatTokenIterator
    {
    public:
        FormatTokenIterator(const Song& song, Iterator begin, Iterator end) :
            m_song(song),
            m_it(begin),
            m_itEnd(end)
        {
            checkIterator();
        }

        bool isValid() const {return m_it != m_itEnd;}

        void next()
        {
            ++m_it;
            checkIterator();
        }

        const FormatToken& get() const
        {
            return m_it->formatToken();
        }

    private:
        const Song& m_song;
        Iterator m_it;
        Iterator m_itEnd;

        void checkIterator()
        {
            if (m_it == m_itEnd)
                return;

            if (m_it->type() == Token::Type::SectionOpenBrace) {
                ++m_it;
                bool isSectionEmpty = false;
                auto it = m_it;
                for (; it->type() != Token::Type::SectionCloseBrace; ++it) {
                    const FormatToken& token = it->formatToken();
                    if (!isSectionEmpty && token.type() == FormatToken::Type::Variable) {
                        if (token.variable().isEmpty(m_song))
                            isSectionEmpty = true;
                    }
                }

                if (isSectionEmpty) {
                    m_it = it + 1;
                    if (m_it == m_itEnd)
                        return;
                    if (m_it->type() == Token::Type::SectionOr) {
                        ++m_it;
                        checkIterator();
                    }

                }
            } else if (m_it->type() == Token::Type::SectionCloseBrace) {
                ++m_it;
                if (m_it == m_itEnd)
                    return;
                if (m_it->type() == Token::Type::SectionOr)
                    for (; m_it->type() != Token::Type::SectionCloseBrace; ++m_it);

                checkIterator();
            }
        }
    };

    class Column
    {
    public:
        Column() :
            m_align(Alignment::Left),
            m_factor(0),
            m_size(0) {}

        enum class Alignment
        {
            Left,
            Center,
            Right
        };

        Alignment align() const        {return m_align;}
        int factor() const             {return m_factor;}
        int size() const               {return m_size;}
        int calculateContentSize(const Song& song) const;

        void setAlign(Alignment align) {m_align = align;}
        void setFactor(int factor)     {m_factor = factor;}
        void setSize(int size)         {m_size = size;}

        Token& addToken(Token::Type type)
        {
            m_tokens.emplace_back(type);
            return m_tokens.back();
        }

        FormatTokenIterator<std::vector<Token>::const_iterator>
        getFormatTokenIterator(const Song& song) const
        {
            return FormatTokenIterator<std::vector<Token>::const_iterator>(song,
                                                                           m_tokens.cbegin(),
                                                                           m_tokens.cend());
        }

    private:
        Alignment m_align;
        int m_factor;
        int m_size;
        std::vector<Token> m_tokens;

    };

    class ParsingError : public std::runtime_error
    {
    public:
        template <typename... Args>
        ParsingError(Args&&... args) :
            runtime_error(Utils::format(std::forward<Args>(args)...)) {}

    };

    std::string m_errorString;
    std::vector<Column> m_columns;

    static int getColorByKey(char key);
};
} // ncxmms2

#endif // SONGDISPLAYFORMATPARSER_H

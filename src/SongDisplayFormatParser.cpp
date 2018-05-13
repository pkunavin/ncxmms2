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

#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <glib.h>

#include "SongDisplayFormatParser.h"
#include "lib/Painter.h"

using namespace ncxmms2;

SongDisplayFormatParser::SongDisplayFormatParser()
{

}

bool SongDisplayFormatParser::setDisplayFormat(const std::string& formatString)
{
    m_columns.clear();

    auto ensureNotNullChar = [](const char *ptr)
    {
        if (!*ptr)
            throw ParsingError("Unexpected end of format string.");
    };

    auto readNumber = [&ensureNotNullChar](const char *str, const char **p, char delimiter) -> int
    {
        const char *numberEnd = *p;
        for (; *numberEnd != delimiter; ++numberEnd) {
            ensureNotNullChar(numberEnd);
            if (!g_ascii_isdigit(*numberEnd))
                throw ParsingError("Unexpected symbol '%c' at position %d, expected digit.",
                                   *numberEnd, numberEnd - str);
        }
        char *endPtr;
        const int result = g_ascii_strtoll(*p, &endPtr, 10);
        if (endPtr != numberEnd)
            throw ParsingError("Parsing int failed at position %d.", *p - str);
        *p = numberEnd;
        return result;
    };

    try
    {
        if (formatString.empty())
            return true;

        if (formatString[0] != '[')
            throw ParsingError("Unexpected symbol '%c' at position 0, expected '['.", formatString[0]);

        bool haveOpenedBrace = false;
        for (const char *p = formatString.c_str(); *p; ++p) {
            switch (*p) {
                case '[': // Column spec begin
                {
                    if (haveOpenedBrace)
                        throw ParsingError("Unbalanced braces.");
                    m_columns.emplace_back();
                    Column& column = m_columns.back();
                    ensureNotNullChar(++p);
                    switch (*p) {
                        case 'r': column.setAlign(Column::Alignment::Right);  break;
                        case 'l': column.setAlign(Column::Alignment::Left);   break;
                        case 'c': column.setAlign(Column::Alignment::Center); break;
                        default:
                            throw ParsingError("Unexpected symbol '%c' at position %d, expected 'r', 'l' or 'c'.",
                                               *p, p - formatString.c_str());
                    }
                    ensureNotNullChar(++p);
                    if (*p != ':')
                        throw ParsingError("Unexpected symbol '%c' at position %d, expected ':'.",
                                           *p, p - formatString.c_str());

                    ensureNotNullChar(++p);
                    column.setFactor(readNumber(formatString.c_str(), &p, ':'));

                    ensureNotNullChar(++p);
                    column.setSize(readNumber(formatString.c_str(), &p, ']'));
                    break;
                }

                case '$': // Variable
                {
                    ensureNotNullChar(++p);
                    Token& token = m_columns.back().addToken(Token::Type::Variable);
                    if (!token.setVariable(*p))
                        throw ParsingError("Unknown variable $%c at position %d.",
                                           *p, p - formatString.c_str() - 1);
                    break;
                }

                case '%': // Color
                {
                    const char *pStart = p;
                    ensureNotNullChar(++p);
                    Token& token = m_columns.back().addToken(Token::Type::Color);
                    const int value = readNumber(formatString.c_str(), &p, 'c');
                    const int color = getColorByKey(value);
                    if (color == -1)
                        throw ParsingError("Unknown color %d at position %d.",
                                           value, pStart - formatString.c_str());
                    token.setColor(static_cast<Color>(color));
                    break;
                }

                case '{': // Section begin
                    m_columns.back().addToken(Token::Type::SectionOpenBrace);
                    if (haveOpenedBrace)
                        throw ParsingError("Sections can't be nested.");
                    haveOpenedBrace = true;
                    break;

                case '}': // Section end
                    m_columns.back().addToken(Token::Type::SectionCloseBrace);
                    if (!haveOpenedBrace)
                        throw ParsingError("Unbalanced braces.");
                    haveOpenedBrace = false;
                    break;

                case '|': // Section OR
                    if (*(p - 1) != '}')
                        throw ParsingError("Unexpected symbol '|' at position %d.",
                                           p - formatString.c_str());
                    ensureNotNullChar(p + 1);
                    if (*(p + 1) != '{')
                        throw ParsingError("Unexpected symbol '%c' at position %d, expected '{'.",
                                           *(p + 1), p + 1 - formatString.c_str());
                    m_columns.back().addToken(Token::Type::SectionOr);
                    break;

                case '\\': // Escape
                    ensureNotNullChar(++p);
                    // Falls through
                default: // Character
                {
                    if (!g_ascii_isprint(*p))
                        throw ParsingError("Unexpected symbol '%c' at position %d.",
                                           *p, p - formatString.c_str());
                    Token& token = m_columns.back().addToken(Token::Type::Character);
                    token.setCharacter(*p);
                    break;
                }
            }
        }
        if (haveOpenedBrace)
            throw ParsingError("Unbalanced braces.");
    }
    catch (const ParsingError& error)
    {
        m_errorString = error.what();
        m_columns.clear();
        return false;
    }

    return true;
}

void SongDisplayFormatParser::calculateColumnsSize(const Rectangle& rect)
{
    int factorsSum = 0;
    int sizeLeft = rect.cols();
    int notFixedColumnCount = 0;
    for (auto& column : m_columns) {
        if (column.factor() != 0) {
            factorsSum += column.factor();
            ++notFixedColumnCount;
        } else {
            sizeLeft -= column.size();
        }
    }

    const int notFixedColumnMinSize = 4;
    if (sizeLeft < notFixedColumnCount * notFixedColumnMinSize)
        throw std::runtime_error("Terminal too small!");

    const int sizeStep = factorsSum ? sizeLeft / factorsSum : 0;

    int lastNotFixedColumn = m_columns.size() - 1;
    while (m_columns[lastNotFixedColumn].factor() == 0) {
        if (--lastNotFixedColumn < 0)
            break;
    }

    for (int i = 0; i < lastNotFixedColumn; ++i) {
        Column& column = m_columns[i];
        if (column.factor() != 0) {
            column.setSize(sizeStep * column.factor());
            sizeLeft -= column.size();
        }
    }
    if (lastNotFixedColumn >= 0)
        m_columns[lastNotFixedColumn].setSize(sizeLeft);
}

void SongDisplayFormatParser::paint(const Song& song, Painter *painter,
                                    const Rectangle& rect, bool ignoreColors)
{
    if (G_UNLIKELY(m_columns.empty()))
        return;
    
    calculateColumnsSize(rect);
    
    int xPos = rect.x();
    for (auto& column : m_columns) {
        int sizeLeft = 0;
        switch (column.align()) {
            case Column::Alignment::Left:
                painter->move(xPos, rect.y());
                sizeLeft = column.size();
                break;

            case Column::Alignment::Center:
            {
                const int contentSize = column.calculateContentSize(song);
                const int xShift = (column.size() - contentSize) / 2;
                if (xShift > 0) {
                    painter->move(xPos + xShift, rect.y());
                    sizeLeft = column.size() - xShift;
                } else {
                    painter->move(xPos, rect.y());
                    sizeLeft = column.size();
                }
                break;
            }

            case Column::Alignment::Right:
            {
                const int contentSize = column.calculateContentSize(song);
                const int xShift = column.size() - contentSize;
                if (xShift > 0) {
                    painter->move(xPos + xShift, rect.y());
                    sizeLeft = column.size() - xShift;
                } else {
                    painter->move(xPos, rect.y());
                    sizeLeft = column.size();
                }
                break;
            }

        }

        for (auto it = column.getFormatTokenIterator(song); it.isValid(); it.next()) {
            const auto& token = it.get();
            switch (token.type()) {
                case Token::Type::Variable:
                {
                    const int oldX = painter->x();
                    token.variable().print(painter, song, sizeLeft);
                    sizeLeft -= painter->x() - oldX;
                    break;
                }

                case Token::Type::Character:
                    painter->printChar(token.character());
                    --sizeLeft;
                    break;

                case Token::Type::Color:
                    if (!ignoreColors)
                        painter->setColor(token.color());
                    break;

                default:
                    assert(false);
                    break;
            }
            if (sizeLeft <= 0)
                break;
        }
        xPos += column.size();
    }
}

bool SongDisplayFormatParser::matchFormattedString(const Song& song, const GRegex *regex)
{
    for (auto& column : m_columns) {
        for (auto it = column.getFormatTokenIterator(song); it.isValid(); it.next()) {
            const auto& token = it.get();
            switch (token.type()) {
                case Token::Type::Variable:
                {
                    const bool match = g_regex_match(regex, token.variable().toString(song).c_str(),
                                                     (GRegexMatchFlags)0, nullptr);
                    if (match)
                        return true;
                    break;
                }

               // NOTE: Current implementation ignores single characters.
               //       Is it OK ?
                case Token::Type::Character:
                case Token::Type::Color:
                    break;

                default:
                    assert(false);
                    break;
            }
        }
    }
    return false;
}

std::string SongDisplayFormatParser::formattedString(const Song& song, int column) const
{
    assert((size_t)column < m_columns.size());
    std::string str;
    for (auto it = m_columns[column].getFormatTokenIterator(song); it.isValid(); it.next()) {
        const auto& token = it.get();
        switch (token.type()) {
            case Token::Type::Variable:
                str.append(token.variable().toString(song));
                break;
           
            case Token::Type::Character:
                str.push_back(token.character());
                break;

            case Token::Type::Color:
                break;

            default:
                assert(false);
                break;
        }
    }
    return str;
}

const char * SongDisplayFormatParser::getSongVariableName(char var)
{
    switch (var) {
        case 'a' : return "Artist";
        case 'A' : return "Album Artist";
        case 't' : return "Title";
        case 'b' : return "Album";
        case 'p' : return "Performer";
        case 'c' : return "Composer";
        case 'y' : return "Date";
        case 'g' : return "Genre";
        case 'f' : return "File name";
        case 'F' : return "File path";
        case 'i' : return "Id";
        case 'n' : return "Track number";
        case 'N' : return "Times played";
        case 'l' : return "Length";
        case 'B' : return "Bitrate";
        case 'S' : return "Samplerate";
        default  : return "Unknown variable";
    }
}

std::string SongDisplayFormatParser::getSongVariableValue(const Song& song, char var)
{
    Variable variable;
    return variable.init(var) ? variable.toString(song) : std::string();
}

int SongDisplayFormatParser::getColorByKey(char key)
{
    switch (key) {
        case 1: return ColorBlack;
        case 2: return ColorRed;
        case 3: return ColorGreen;
        case 4: return ColorYellow;
        case 5: return ColorBlue;
        case 6: return ColorMagenta;
        case 7: return ColorCyan;
        case 8: return ColorWhite;

        default: return -1;
    }
}


bool SongDisplayFormatParser::Variable::init(char key)
{
    m_songStrRefFuncPtr = nullptr;
    switch (key) {
        case 'a': m_songStrRefFuncPtr = &Song::artist;      break;
        case 'A': m_songStrRefFuncPtr = &Song::albumArtist; break;
        case 't': m_songStrRefFuncPtr = &Song::title;       break;
        case 'b': m_songStrRefFuncPtr = &Song::album;       break;
        case 'p': m_songStrRefFuncPtr = &Song::performer;   break;
        case 'c': m_songStrRefFuncPtr = &Song::composer;    break;
        case 'y': m_songStrRefFuncPtr = &Song::date;        break;
        case 'g': m_songStrRefFuncPtr = &Song::genre;       break;
        case 'f': m_songStrRefFuncPtr = &Song::fileName;    break;
        case 'F': m_songStrRefFuncPtr = &Song::url;         break;

        default: break;
    }
    if (m_songStrRefFuncPtr) {
        m_type = Type::StringRef;
        return true;
    }

    m_songIntFuncPtr = nullptr;
    switch (key) {
        case 'i': m_songIntFuncPtr = &Song::id;          break;
        case 'n': m_songIntFuncPtr = &Song::trackNumber; break;
        case 'N': m_songIntFuncPtr = &Song::timesPlayed; break;

        default: break;
    }
    if (m_songIntFuncPtr) {
        m_type = Type::Integer;
        return true;
    }

    m_stringFuncPtr = nullptr;
    switch (key) {
        case 'l': m_stringFuncPtr = &Variable::durationStringGenerator;   break;
        case 'B': m_stringFuncPtr = &Variable::bitrateStringGenerator;    break;
        case 'S': m_stringFuncPtr = &Variable::samplerateStringGenerator; break;

        default: break;
    }
    if (m_stringFuncPtr) {
        m_type = Type::String;
        return true;
    }

    m_type = Type::None;
    return false;
}

bool SongDisplayFormatParser::Variable::isEmpty(const Song& song) const
{
    switch (m_type) {
        case Type::StringRef: return (song.*m_songStrRefFuncPtr)().empty();
        case Type::String:    return (*m_stringFuncPtr)(song).empty();
        case Type::Integer:   return (song.*m_songIntFuncPtr)() == -1;

        case Type::None:
        default:
            assert(false);
            return true;
    }
}

int SongDisplayFormatParser::Variable::size(const Song& song) const
{
    switch (m_type) {
        case Type::StringRef:
        {
            const std::string& str = (song.*m_songStrRefFuncPtr)();
            return g_utf8_strlen(str.c_str(), str.size());
        }

        case Type::String:
        {
            std::string str = (*m_stringFuncPtr)(song);
            return g_utf8_strlen(str.c_str(), str.size());
        }

        case Type::Integer:
        {
            const int value = (song.*m_songIntFuncPtr)();
            if (value != -1) {
                const size_t maxLength = 16;
                char buffer[maxLength + 1];
                buffer[maxLength] = '\0';
                std::snprintf(buffer, maxLength, "%d", value);
                return std::strlen(buffer);
            }
            return 0;
        }

        case Type::None:
        default:
            assert(false);
            return 0;
    }
}

void SongDisplayFormatParser::Variable::print(Painter *painter, const Song& song, int maxLength) const
{
    switch (m_type) {
        case Type::StringRef:
            painter->squeezedPrint((song.*m_songStrRefFuncPtr)(), maxLength);
            break;

        case Type::String:
            painter->squeezedPrint((*m_stringFuncPtr)(song), maxLength);
            break;

        case Type::Integer:
        {
            const int value = (song.*m_songIntFuncPtr)();
            if (value != -1)
                painter->squeezedPrint(std::to_string(value), maxLength);

            break;
        }

        case Type::None:
            assert(false);
            break;
    }
}

std::string SongDisplayFormatParser::Variable::toString(const Song& song) const
{
    switch (m_type) {
        case Type::StringRef:
            return (song.*m_songStrRefFuncPtr)();

        case Type::String:
            return (*m_stringFuncPtr)(song);

        case Type::Integer:
        {
            const int value = (song.*m_songIntFuncPtr)();
            return value != -1 ? std::to_string(value) : std::string();
        }

        case Type::None:
            assert(false);
    }
    return std::string();
}

std::string SongDisplayFormatParser::Variable::durationStringGenerator(const Song& song)
{
    return song.duration() != -1 ? Utils::getTimeStringFromInt(song.duration()) : std::string();
}

std::string SongDisplayFormatParser::Variable::bitrateStringGenerator(const Song& song)
{
    if (song.bitrate() == -1)
        return std::string();

    return std::to_string(song.bitrate() / 1000).append("kbps");
}

std::string SongDisplayFormatParser::Variable::samplerateStringGenerator(const Song& song)
{
    if (song.samplerate() == -1)
        return std::string();

    return std::to_string(song.samplerate()).append("Hz");
}


int SongDisplayFormatParser::Column::calculateContentSize(const Song& song) const
{
    int size = 0;
    for (auto it = getFormatTokenIterator(song); it.isValid(); it.next()) {
        const auto& token = it.get();
        switch (token.type()) {
            case Token::Type::Variable:
                size += token.variable().size(song);
                break;

            case Token::Type::Character:
                ++size;
                break;

            case Token::Type::Color:
                break;

            default:
                assert(false);
                break;
        }
    }
    return size;
}

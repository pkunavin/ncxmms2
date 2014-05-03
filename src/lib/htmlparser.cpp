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

#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <glib.h>

#include "htmlparser.h"
#include "stringref.h"
#include "stringalgo.h"

namespace ncxmms2 {
namespace HtmlParserImpl {
typedef HtmlParser::TokenTag::Type Tags;

const folly::sorted_vector_map<StringRef, HtmlParser::TokenTag::Type> tagNamesMap
{
    {"b",          Tags::Bold            },
    {"u",          Tags::Underline       },
    {"blink",      Tags::Blink           },
    {"font",       Tags::Font            },
    {"script",     Tags::Script          },
    {"style",      Tags::Style           },
    {"h1",         Tags::Heading_1       },
    {"h2",         Tags::Heading_2       },
    {"h3",         Tags::Heading_3       },
    {"h4",         Tags::Heading_4       },
    {"h5",         Tags::Heading_5       },
    {"h6",         Tags::Heading_6       },
    {"br",         Tags::LineBreak       },
    {"p",          Tags::Paragraph       },
    {"div",        Tags::Division        },
    {"article",    Tags::Article         },
    {"blockquote", Tags::BlockQuote      },
    {"li",         Tags::ListItem        },
    {"pre",        Tags::PreformattedText},
    {"title",      Tags::Title           },
    {"q",          Tags::Quotation       }
};

const folly::sorted_vector_map<StringRef, char32_t> entitiesMap
{
    {"quot",      '"'},
    {"amp",       '&'},
    {"apos",     '\''},
    {"lt",        '<'},
    {"gt",        '>'},
    {"nbsp",      160},
    {"iexcl",     161},
    {"cent",      162},
    {"pound",     163},
    {"curren",    164},
    {"yen",       165},
    {"brvbar",    166},
    {"sect",      167},
    {"uml",       168},
    {"copy",      169},
    {"ordf",      170},
    {"laquo",     171},
    {"not",       172},
    {"shy",       173},
    {"reg",       174},
    {"macr",      175},
    {"deg",       176},
    {"plusmn",    177},
    {"sup2",      178},
    {"sup3",      179},
    {"acute",     180},
    {"micro",     181},
    {"para",      182},
    {"middot",    183},
    {"cedil",     184},
    {"sup1",      185},
    {"ordm",      186},
    {"raquo",     187},
    {"frac14",    188},
    {"frac12",    189},
    {"frac34",    190},
    {"iquest",    191},
    {"Agrave",    192},
    {"Aacute",    193},
    {"Acirc",     194},
    {"Atilde",    195},
    {"Auml",      196},
    {"Aring",     197},
    {"AElig",     198},
    {"Ccedil",    199},
    {"Egrave",    200},
    {"Eacute",    201},
    {"Ecirc",     202},
    {"Euml",      203},
    {"Igrave",    204},
    {"Iacute",    205},
    {"Icirc",     206},
    {"Iuml",      207},
    {"ETH",       208},
    {"Ntilde",    209},
    {"Ograve",    210},
    {"Oacute",    211},
    {"Ocirc",     212},
    {"Otilde",    213},
    {"Ouml",      214},
    {"times",     215},
    {"Oslash",    216},
    {"Ugrave",    217},
    {"Uacute",    218},
    {"Ucirc",     219},
    {"Uuml",      220},
    {"Yacute",    221},
    {"THORN",     222},
    {"szlig",     223},
    {"agrave",    224},
    {"aacute",    225},
    {"acirc",     226},
    {"atilde",    227},
    {"auml",      228},
    {"aring",     229},
    {"aring",     230},
    {"ccedil",    231},
    {"egrave",    232},
    {"eacute",    233},
    {"ecirc",     234},
    {"euml",      235},
    {"igrave",    236},
    {"iacute",    237},
    {"icirc",     238},
    {"iuml",      239},
    {"eth",       240},
    {"ntilde",    241},
    {"ograve",    242},
    {"oacute",    243},
    {"ocirc",     244},
    {"otilde",    245},
    {"ouml",      246},
    {"divide",    247},
    {"oslash",    248},
    {"ugrave",    249},
    {"uacute",    250},
    {"ucirc",     251},
    {"uuml",      252},
    {"yacute",    253},
    {"thorn",     254},
    {"yuml",      255},
    {"OElig",     338},
    {"oelig",     339},
    {"Scaron",    352},
    {"scaron",    353},
    {"Yuml",      376},
    {"fnof",      402},
    {"circ",      710},
    {"tilde",     732},
    {"Alpha",     913},
    {"Beta",      914},
    {"Gamma",     915},
    {"Delta",     916},
    {"Epsilon",   917},
    {"Zeta",      918},
    {"Eta",       919},
    {"Theta",     920},
    {"Iota",      921},
    {"Kappa",     922},
    {"Lambda",    923},
    {"Mu",        924},
    {"Nu",        925},
    {"Xi",        926},
    {"Omicron",   927},
    {"Pi",        928},
    {"Rho",       929},
    {"Sigma",     931},
    {"Tau",       932},
    {"Upsilon",   933},
    {"Phi",       934},
    {"Chi",       935},
    {"Psi",       936},
    {"Omega",     937},
    {"alpha",     945},
    {"beta",      946},
    {"gamma",     947},
    {"delta",     948},
    {"epsilon",   949},
    {"zeta",      950},
    {"eta",       951},
    {"theta",     952},
    {"iota",      953},
    {"kappa",     954},
    {"lambda",    955},
    {"mu",        956},
    {"nu",        957},
    {"xi",        958},
    {"omicron",   959},
    {"pi",        960},
    {"rho",       961},
    {"sigmaf",    962},
    {"sigma",     963},
    {"tau",       964},
    {"upsilon",   965},
    {"phi",       966},
    {"chi",       967},
    {"psi",       968},
    {"omega",     969},
    {"thetasym",  977},
    {"upsih",     978},
    {"piv",       982},
    {"ensp",     9194},
    {"emsp",     8195},
    {"thinsp",   8201},
    {"zwnj",     8204},
    {"zwj",      8205},
    {"lrm",      8206},
    {"rlm",      8207},
    {"ndash",    8211},
    {"mdash",    8212},
    {"lsquo",    8216},
    {"rsquo",    8217},
    {"sbquo",    8218},
    {"ldquo",    8220},
    {"rdquo",    8221},
    {"bdquo",    8222},
    {"dagger",   8224},
    {"Dagger",   8225},
    {"bull",     8226},
    {"hellip",   8230},
    {"permil",   8240},
    {"prime",    8242},
    {"Prime",    8243},
    {"lsaquo",   8249},
    {"rsaquo",   8250},
    {"oline",    8254},
    {"frasl",    8260},
    {"euro",     8364},
    {"image",    8465},
    {"weierp",   8472},
    {"real",     8476},
    {"trade",    8482},
    {"alefsym",  8501},
    {"larr",     8592},
    {"uarr",     8593},
    {"rarr",     8594},
    {"darr",     8595},
    {"harr",     8596},
    {"crarr",    8629},
    {"lArr",     8656},
    {"uArr",     8657},
    {"rArr",     8658},
    {"dArr",     8659},
    {"hArr",     8660},
    {"forall",   8704},
    {"part",     8706},
    {"exist",    8707},
    {"empty",    8709},
    {"nabla",    8711},
    {"isin",     8712},
    {"notin",    8713},
    {"ni",       8715},
    {"prod",     8719},
    {"sum",      8721},
    {"minus",    8722},
    {"lowast",   8727},
    {"radic",    8730},
    {"prop",     8733},
    {"infin",    8734},
    {"ang",      8736},
    {"and",      8743},
    {"or",       8744},
    {"cap",      8745},
    {"cup",      8746},
    {"int",      8747},
    {"there4",   8756},
    {"sim",      8764},
    {"cong",     8773},
    {"asymp",    8776},
    {"ne",       8800},
    {"equiv",    8801},
    {"le",       8804},
    {"ge",       8805},
    {"sub",      8834},
    {"sup",      8835},
    {"nsub",     8836},
    {"sube",     8838},
    {"supe",     8839},
    {"oplus",    8853},
    {"otimes",   8855},
    {"perp",     8869},
    {"sdot",     8901},
    {"vellip",   8942},
    {"lceil",    8968},
    {"rceil",    8969},
    {"lfloor",   8970},
    {"rfloor",   8971},
    {"lang",     9001},
    {"rang",     9002},
    {"loz",      9674},
    {"spades",   9824},
    {"clubs",    9827},
    {"hearts",   9829},
    {"diams",    9830}
};

template <typename Pred>
class IsNotImpl
{
    Pred m_pred;
public:
    IsNotImpl(Pred pred) : m_pred(pred) {}
    
    template <typename... Args>
    bool operator()(Args&&... args)
    {
        return !m_pred(std::forward<Args>(args)...);
    }
};

template <typename Pred>
IsNotImpl<Pred> isNot(Pred pred)
{
    return IsNotImpl<Pred>(pred);
}

auto isSpace = [](char ch){return g_ascii_isspace(ch);};

const char * readText(const char *p, const HtmlParser::Token *lastTextOrEntityToken, std::string *text)
{
    auto skipSpaces = [](const char *p){return readUntilIf<'<', '&'>(p, isNot(isSpace));};
    auto skipWord   = [](const char *p){return readUntilIf<'<', '&'>(p, isSpace);};
    
    if (lastTextOrEntityToken) {
        if (lastTextOrEntityToken->type() == HtmlParser::Token::Type::Text) {
            const HtmlParser::TokenText& lastTextToken = lastTextOrEntityToken->text();
            if (boost::ends_with(lastTextToken.text(), " "))
                 p = skipSpaces(p);
        }
    } else {
        p = skipSpaces(p);
    }
    
    while (*p && *p != '<' && *p != '&') {
        if (g_ascii_isspace(*p)) {
            p = skipSpaces(p);
            text->push_back(' ');
        } else {
            const char *wordEnd = skipWord(p);
            text->append(p, wordEnd);
            p = wordEnd;
        }
    }
    
    return p;
}

const char * readTextVerbatim(const char *p) {return readUntil<'<', '&'>(p);}
const char * readTag(const char *p)          {return readUntil<'>'>(p);}
const char * readEntity(const char *p)       {return readUntil<';'>(p);}

const char * readScript(const char *p)
{
    while (*p) {
        if (*p == '<') {
            const char *tagEnd = readTag(p + 1);
            if (!*tagEnd)
                return tagEnd;
            HtmlParser::TokenTag tag(p + 1, tagEnd);
            if (tag.type() == HtmlParser::TokenTag::Type::Script)
                break;
        }
        ++p;
    }
    return p;
}

const char * readStyle(const char *p)
{
    while (*p) {
        if (*p == '<') {
            const char *tagEnd = readTag(p + 1);
            if (!*tagEnd)
                return tagEnd;
            HtmlParser::TokenTag tag(p + 1, tagEnd);
            if (tag.type() == HtmlParser::TokenTag::Type::Style)
                break;
        }
        ++p;
    }
    return p;
}

enum class ParserState
{
    ReadingText,
    ReadingTextVerbatim,
    ReadingScript,
    ReadingStyle
};

ParserState switchParserState(ParserState state, const HtmlParser::TokenTag& tag)
{
    if (tag.isStartTag()) {
        switch (tag.type()) {
            case Tags::Script:
                state = ParserState::ReadingScript;
                break;
                
            case Tags::Style:
                state = ParserState::ReadingStyle;
                break;
                
            case Tags::PreformattedText:
                state = ParserState::ReadingTextVerbatim;
                break;
                
            default:
                break;
        }
    } else {
        switch (tag.type()) {
            case Tags::Script:
            case Tags::Style:
            case Tags::PreformattedText:
                state = ParserState::ReadingText;
                break;
                
            default:
                break;
        }
    }
    return state;
}

} // HtmlParserImpl
} // ncxmms2

using namespace ncxmms2;

HtmlParser::TokenTag::TokenTag(const char *p, const char *tagEnd) :
    m_type(Type::Unknown),
    m_isStartTag(true)
{
    using namespace HtmlParserImpl;
    
    assert(*tagEnd == '>');
    
    m_isStartTag = *p != '/';
    if (!m_isStartTag)
        ++p;
    
    auto readUntilSpace = [](const char *p, const char *tagEnd)
    {
        return readUntilIf(p, tagEnd, isSpace);
    };
    
    const char *tagNameEnd = readUntilSpace(p, tagEnd);
    
    const int maxTagNameLength = 32;
    char tagName[maxTagNameLength];
    if (tagNameEnd - p > maxTagNameLength - 1)
        return;
    boost::to_lower_copy(tagName, boost::make_iterator_range(p, tagNameEnd));
    tagName[tagNameEnd - p] = '\0';
    
    auto it = tagNamesMap.find(tagName);
    if (it != tagNamesMap.end()) {
        m_type = it->second;
    } else {
        return;
    }
    
    p = tagNameEnd;
    
    auto skipSpaces = [](const char *p, const char *tagEnd)
    {
        return readUntilIf(p, tagEnd, isNot(isSpace));
    };
    
    auto readAttributeName = [](const char *p, const char *tagEnd)
    {
        return readUntilIf<'='>(p, tagEnd, isSpace);
    };
    
    auto readUntilChar = [](const char *p, const char *tagEnd, char ch)
    {
        return readUntilIf(p, tagEnd, [ch](char c){return c == ch;});
    };
    
    while (p != tagEnd) {
        p = skipSpaces(p, tagEnd);
        if (p == tagEnd)
            break;
        
        const char *attrNameEnd = readAttributeName(p, tagEnd);
        std::string attrName(p, attrNameEnd);
        boost::to_lower(attrName);
        
        std::string attrValue;
        p = skipSpaces(attrNameEnd, tagEnd);
        if (*p == '=') {
            ++p;
            p = skipSpaces(p, tagEnd);
            
            if (*p == '"' || *p == '\'') {
                const char *attrValueEnd = readUntilChar(p + 1, tagEnd, *p);
                ++p;
                if (attrValueEnd != tagEnd) {
                    attrValue.assign(p, attrValueEnd);
                    p = attrValueEnd + 1;
                } else {
                    p = attrValueEnd;
                }
            } else {
                const char *attrValueEnd = readUntilSpace(p, tagEnd);
                attrValue.assign(p, attrValueEnd);
                p = attrValueEnd;
            }
            boost::to_lower(attrValue);
        }
        // FIXME: Decode entities inside attribute values
        m_attributes.insert(std::make_pair(std::move(attrName), std::move(attrValue)));
    }
}

bool HtmlParser::TokenTag::hasAttribute(const std::string& attr) const
{
    return m_attributes.find(attr) != m_attributes.end();
}

const std::string& HtmlParser::TokenTag::attribute(const std::string& attr) const
{
    static const std::string empty;
    auto it = m_attributes.find(attr);
    return it != m_attributes.end() ? it->second : empty;
}

HtmlParser::TokenEntity::TokenEntity(const std::string& entity) :
    m_character(0)
{
    using namespace HtmlParserImpl;
    
    if (boost::istarts_with(entity, "#x")) {
        char *end;
        m_character = strtol(entity.c_str() + 2, &end, 16);
        if (*end)
            m_character = 0;
    } else if (boost::istarts_with(entity, "#")) {
        char *end;
        m_character = strtol(entity.c_str() + 1, &end, 10);
        if (*end)
            m_character = 0;
    } else {
        auto it = entitiesMap.find(entity.c_str());
        if (it != entitiesMap.end())
            m_character = it->second;
    }
    
    if (m_character) {
        int n = g_unichar_to_utf8(m_character, m_utf8);
        m_utf8[n] = '\0';
    } else {
        m_utf8[0] = '\0';
    }
}

void HtmlParser::parse(const std::string& text)
{
    using namespace HtmlParserImpl;
    
    m_tokens.clear();
    
    ParserState parserState = ParserState::ReadingText;
    ptrdiff_t lastTextOrEntityTokenIndex = -1;
    for (const char *p = text.c_str(); *p;) {
        if (*p == '<') {
            ++p;
            const char *tagEnd = readTag(p);
            if (!*tagEnd)
                break;
            
            m_tokens.emplace_back(Token::TagTypeTag(), p, tagEnd);
            parserState = switchParserState(parserState, m_tokens.back().tag());
            p = tagEnd + 1;
        } else if (*p == '&') {
            ++p;
            const char *entityEnd = readEntity(p);
            if (!*entityEnd)
                break;
            
            lastTextOrEntityTokenIndex = m_tokens.size();
            m_tokens.emplace_back(Token::EntityTypeTag(), std::string(p, entityEnd));
            p = entityEnd + 1;
        }
         
        switch (parserState) {
            case ParserState::ReadingScript:
                p = readScript(p);
                break;
            
            case ParserState::ReadingStyle:
                p = readStyle(p);
                break;
                    
            case ParserState::ReadingText:
            {
                std::string text;
                const Token *lastTextOrEntityToken = lastTextOrEntityTokenIndex > 0 
                                                   ? &m_tokens[lastTextOrEntityTokenIndex]
                                                   : nullptr;
                p = readText(p, lastTextOrEntityToken, &text);
                if (!text.empty()) {
                    lastTextOrEntityTokenIndex = m_tokens.size();
                    m_tokens.emplace_back(Token::TextTypeTag(), std::move(text));
                }
                break;
            }
                
            case ParserState::ReadingTextVerbatim:
            {
                const char *textEnd = readTextVerbatim(p);
                if (textEnd != p) {
                    lastTextOrEntityTokenIndex = -1;
                    m_tokens.emplace_back(Token::TextTypeTag(), p, textEnd);
                }
                p = textEnd;
                break;
            }
        }
    }
}

std::string HtmlParser::encodeEntities(const std::string& str)
{
    std::string result;
    result.reserve(str.size());
    
    auto skipText = [](const char *p){return readUntil<'<', '>', '&'>(p);};
    
    const char *p = str.c_str();
    while (*p) {
        const char *textEnd = skipText(p);
        result.append(p, textEnd);
        if (!*textEnd)
            break;
        switch (*textEnd) {
            case '<': result.append("&lt;");  break;
            case '>': result.append("&gt;");  break;
            case '&': result.append("&amp;"); break;
            default: assert(false);
        }
        p = textEnd + 1;
    }
    return result;
}

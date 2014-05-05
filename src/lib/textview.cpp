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

#include <vector>
#include <algorithm>
#include <cstdint>
#include <stack>
#include <glib.h>

#include "textview.h"
#include "painter.h"
#include "keyevent.h"
#include "mouseevent.h"
#include "size.h"
#include "palette.h"
#include "htmlparser.h"
#include "stringptr.h"
#include "stringref.h"
#include "stringalgo.h"

#include "../../3rdparty/folly/sorted_vector_types.h"

namespace ncxmms2 {

class TextViewPrivate
{
public:
    
    enum TextAttributes
    {
        TextNormal    = 0x00,
        TextBold      = 0x01,
        TextUnderline = 0x02,
        TextBlink     = 0x04
    };
    
    struct HtmlCompilerState;
     
    struct TextChunk
    {
        uint8_t attrs;
        Color color;
        StringPtr text;
        
        std::unique_ptr<TextChunk> next;
        
        TextChunk() :
            attrs(TextNormal),
            color(ColorDefault) {}
        TextChunk(struct HtmlCompilerState *state);
        TextChunk(const TextChunk& other) = delete;
        TextChunk& operator=(const TextChunk& other) = delete;
        TextChunk(TextChunk&&) = default;
        TextChunk& operator=(TextChunk&&) = default;
    };
    
    TextViewPrivate(TextView *q_) :
        q(q_),
        viewportBeginLine(-1),
        mode(TextView::Mode::PlainText) {}

    TextView *q;
    int viewportBeginLine;
    TextView::Mode mode;
    HtmlParser htmlParser;
    std::vector<TextChunk> textLines;

    struct HtmlCompilerState
    {
        enum State
        {
            Text,
            TextVerbatim,
            NoDisplay
        };
        
        State state;
        uint8_t attrs;
        std::stack<Color> colorsStack;
        TextChunk *lastTextChunk;
        int lineSpaceLeft;
    };
    
    void compileHtml();
    void htmlCompilerProcessTag(HtmlCompilerState *state, const HtmlParser::TokenTag& tag);
    void htmlCompilerProcessTextToken(HtmlCompilerState *state, const HtmlParser::TokenText& tokenText);
    void htmlCompilerProcessText(HtmlCompilerState *state, const std::string& text);
    void htmlCompilerProcessTextVerbatim(HtmlCompilerState *state, const std::string& text);
    void htmlCompilerProcessEntity(HtmlCompilerState *state, const HtmlParser::TokenEntity &entity);
    
    Color htmlCompilerParseColorName(const std::string& colorName, Color fallback);
    
    void htmlCompilerAppendEmptyLine(HtmlCompilerState *state);
    void htmlCompilerAppendLine(HtmlCompilerState *state);
    void htmlCompilerAppendTextChunk(HtmlCompilerState *state);
    bool isLastLineEmpty() const;
    
    void compilePlainText(const std::string& text);
    
    void viewportInit();
    
    bool canScrollDown() const;
    int lineSizeBytes(int line) const;
    int lineLength(int line) const;
    
    void scrollUp();
    void scrollDown();
    void scrollHome();
    void scrollEnd();
    void scrollPageUp();
    void scrollPageDown();
};

TextViewPrivate::TextChunk::TextChunk(TextViewPrivate::HtmlCompilerState *state) :
    attrs(state->attrs),
    color(state->colorsStack.top())
{
    
}
} // ncxmms2

using namespace ncxmms2;

TextView::TextView(const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    d(new TextViewPrivate(this))
{
    loadPalette("TextView");
}

TextView::~TextView()
{
    
}

void TextView::setMode(TextView::Mode mode)
{
    d->mode = mode;
}

TextView::Mode TextView::mode() const
{
    return d->mode;
}

void TextView::setText(const std::string& text)
{
    switch (d->mode) {
        case Mode::PlainText:
            d->compilePlainText(text);
            break;
        
        case Mode::RichText:
            d->htmlParser.parse(text);
            d->compileHtml();
            break;
    }
    d->viewportInit();
}

void TextView::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:       d->scrollUp();       break;
        case KeyEvent::KeyDown:     d->scrollDown();     break;
        case KeyEvent::KeyHome:     d->scrollHome();     break;
        case KeyEvent::KeyEnd:      d->scrollEnd();      break;
        case KeyEvent::KeyPageUp:   d->scrollPageUp();   break;
        case KeyEvent::KeyPageDown: d->scrollPageDown(); break;
        default: break;
    }
}

void TextView::mouseEvent(const MouseEvent& ev)
{
    if (ev.type() != MouseEvent::Type::ButtonPress)
        return;

    switch (ev.button()) {
        case MouseEvent::WheelUp:   d->scrollUp();   break;
        case MouseEvent::WheelDown: d->scrollDown(); break;
        default: break;
    }
}

void TextView::resize(const Size& size)
{
    int linesCount = d->textLines.size();
    if (linesCount > 0 && size.lines() > lines()) {
        int extraSize = size.lines() - lines();
        
        if (linesCount > lines()) {
            extraSize -= linesCount - (d->viewportBeginLine + lines());
        }
        
        if (extraSize > 0 && d->viewportBeginLine > 0) {
            d->viewportBeginLine -= std::min(d->viewportBeginLine, extraSize);;
        }
    }

    Window::resize(size);
    
    if (d->mode == Mode::RichText && linesCount > 0) {
        d->compileHtml();
        linesCount = d->textLines.size();
        if (d->viewportBeginLine + lines() > linesCount) {
            d->viewportBeginLine = linesCount > lines()
                                 ? linesCount - lines()
                                 : 0;
        }
    }
}

void TextView::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearWindow();

    if (!d->textLines.empty()) {
        const int lastLine = std::min(d->viewportBeginLine + lines(), (int)d->textLines.size());
        int line = d->viewportBeginLine;
        for (int y = 0; y < lines() && line < lastLine; ++y) {
            painter.move(0, y);
            const TextViewPrivate::TextChunk *chunk = &d->textLines[line];
            do {
                painter.setNormal();
                if (chunk->attrs & TextViewPrivate::TextBold)
                    painter.setBold(true);
                if (chunk->attrs & TextViewPrivate::TextUnderline)
                    painter.setUnderline(true);
                if (chunk->attrs & TextViewPrivate::TextBlink)
                    painter.setBlink(true);
                
                painter.setColor(chunk->color);
                if (!chunk->text.isEmpty())
                    painter.printString(chunk->text.begin(), chunk->text.end());
                
                chunk = chunk->next.get();
            } while (chunk);
            
            ++line;
            if (painter.y() > y && painter.x() > 0)
                y += painter.y() - y;
        }
    }

    painter.flush();
}

void TextViewPrivate::compileHtml()
{
    textLines.clear();
    
    HtmlCompilerState state;
    state.state = HtmlCompilerState::Text;
    state.attrs = TextAttributes::TextNormal;
    state.colorsStack.push(q->palette().color(Palette::GroupActive, Palette::RoleText));
    state.lastTextChunk = nullptr;
    
    for (auto& token : htmlParser) {
            switch (token.type()) {
            case HtmlParser::Token::Type::Tag:
                htmlCompilerProcessTag(&state, token.tag());
                break;
                
            case HtmlParser::Token::Type::Text:
                htmlCompilerProcessTextToken(&state, token.text());
                break;
            
            case HtmlParser::Token::Type::Entity:
                htmlCompilerProcessEntity(&state, token.entity());
                break;
        }
    }
}

void TextViewPrivate::htmlCompilerProcessTag(TextViewPrivate::HtmlCompilerState *state,
                                             const HtmlParser::TokenTag& tag)
{
    auto setAttribute = [state](bool enable, uint8_t attr)
    {
        if (enable) {
            state->attrs |= attr;
        } else {
            state->attrs &= ~attr;
        }
    };

    switch (tag.type()) {
        case HtmlParser::TokenTag::Type::Bold:
            setAttribute(tag.isStartTag(), TextBold);
            break;
            
        case HtmlParser::TokenTag::Type::Blink:
            setAttribute(tag.isStartTag(), TextBlink);
            break;
            
        case HtmlParser::TokenTag::Type::Underline:
            setAttribute(tag.isStartTag(), TextUnderline);
            break;
            
        case HtmlParser::TokenTag::Type::Font:
        {
            if (tag.isStartTag()) {
                state->colorsStack.push(htmlCompilerParseColorName(tag.attribute("color"),
                                                                   state->colorsStack.top()));
            } else {
                if (state->colorsStack.size() > 1)
                    state->colorsStack.pop();
            }
            break;
        }
            
        case HtmlParser::TokenTag::Type::Heading_1:
        case HtmlParser::TokenTag::Type::Heading_2:
        case HtmlParser::TokenTag::Type::Heading_3:
        case HtmlParser::TokenTag::Type::Heading_4:
        case HtmlParser::TokenTag::Type::Heading_5:
        case HtmlParser::TokenTag::Type::Heading_6:
            if (tag.isStartTag()) {
                setAttribute(true, TextBold);
                if (!isLastLineEmpty())
                    htmlCompilerAppendEmptyLine(state);
                state->lastTextChunk = nullptr;
            } else {
                setAttribute(false, TextBold);
                htmlCompilerAppendEmptyLine(state);
            }
            break;
            
        case HtmlParser::TokenTag::Type::LineBreak:
            if (tag.isStartTag()) {
                state->lastTextChunk = nullptr;
            }
            break;
            
        case HtmlParser::TokenTag::Type::Paragraph:
            if (tag.isStartTag() && !isLastLineEmpty())
                htmlCompilerAppendEmptyLine(state);
            break;
            
        case HtmlParser::TokenTag::Type::Division:
        case HtmlParser::TokenTag::Type::Article:
        case HtmlParser::TokenTag::Type::BlockQuote:
            if (!isLastLineEmpty())
                htmlCompilerAppendEmptyLine(state);
            state->lastTextChunk = nullptr;
            break;
            
        case HtmlParser::TokenTag::Type::ListItem:
            // TODO: More sophisticated list support
            if (tag.isStartTag()) {
                htmlCompilerAppendLine(state);
                state->lastTextChunk->text.assign(StringPtr::Policy::OwnCopy, " * ");
                state->lineSpaceLeft -= state->lastTextChunk->text.size();
            }
            break;
            
        case HtmlParser::TokenTag::Type::PreformattedText:
            if (tag.isStartTag()) {
                state->state = HtmlCompilerState::TextVerbatim;
                if (!isLastLineEmpty())
                    htmlCompilerAppendEmptyLine(state);
            } else {
                state->state = HtmlCompilerState::Text;
            }
            state->lastTextChunk = nullptr;
            break;
            
        case HtmlParser::TokenTag::Type::Quotation:
            if (state->state == HtmlCompilerState::Text && state->lineSpaceLeft < 1) {
                htmlCompilerAppendLine(state);
            } else {
                htmlCompilerAppendTextChunk(state);
            }
            state->lastTextChunk->text.assign(StringPtr::Policy::OwnCopy, "\"");
            --state->lineSpaceLeft;
            break;
            
        case HtmlParser::TokenTag::Type::Title:
            state->state = tag.isStartTag() ? HtmlCompilerState::NoDisplay
                                            : HtmlCompilerState::Text;
            break;
            
        default:
            break;
    }
}

void TextViewPrivate::htmlCompilerProcessTextToken(TextViewPrivate::HtmlCompilerState *state,
                                                   const HtmlParser::TokenText& tokenText)
{
    switch (state->state) {
       case HtmlCompilerState::Text:
            htmlCompilerProcessText(state, tokenText.text());
            break;
            
        case HtmlCompilerState::TextVerbatim:
            htmlCompilerProcessTextVerbatim(state, tokenText.text());
            break;
            
        case HtmlCompilerState::NoDisplay:
            break;
    }
}

void TextViewPrivate::htmlCompilerProcessText(TextViewPrivate::HtmlCompilerState *state,
                                              const std::string& text)
{
    if (text.empty())
        return;
    
    auto skipWord = [](const char *wordBegin){return readUntil<' '>(wordBegin);};
    
    const char *wordBegin = text.c_str();
    
    if (state->lastTextChunk) {
        htmlCompilerAppendTextChunk(state);
    } else {
        if (*wordBegin == ' ')
            ++wordBegin;
        
        if (!*wordBegin)
            return;
        
        htmlCompilerAppendLine(state);
    }
    
    const char *textBegin = wordBegin;
    int lineLength = 0;
    
    while (*wordBegin) {
        const char *wordEnd = skipWord(wordBegin + 1);
        int wordLength = g_utf8_strlen(wordBegin, wordEnd - wordBegin);
        if (lineLength + wordLength > state->lineSpaceLeft) {
            assert(state->lastTextChunk);
            state->lastTextChunk->text.assign(StringPtr::Policy::NonOwn, textBegin, wordBegin);
            
            if (*wordBegin == ' ') {
                ++wordBegin;
                --wordLength;
            }
            if (!*wordBegin)
                return;
            
            htmlCompilerAppendLine(state);
            lineLength = 0;
            textBegin = wordBegin;
        }
        lineLength += wordLength;
        wordBegin = wordEnd;
    }
    if (*textBegin) {
        assert(state->lastTextChunk);
        state->lastTextChunk->text.assign(StringPtr::Policy::NonOwn, textBegin);
        state->lineSpaceLeft -= lineLength;
    }
}

void TextViewPrivate::htmlCompilerProcessTextVerbatim(TextViewPrivate::HtmlCompilerState *state,
                                                      const std::string& text)
{
    if (text.empty())
        return;
    
    htmlCompilerAppendTextChunk(state);
    assert(state->lastTextChunk);
    
    forEachLine(text.c_str(), [this, state](const char *lineBegin, const char *lineEnd) {
        assert(state->lastTextChunk);
        state->lastTextChunk->text.assign(StringPtr::Policy::NonOwn, lineBegin, lineEnd);
        if (*lineEnd)
            htmlCompilerAppendLine(state);
    });
}

void TextViewPrivate::htmlCompilerProcessEntity(TextViewPrivate::HtmlCompilerState *state,
                                                const HtmlParser::TokenEntity& entity)
{
    if (!entity.isKnown())
        return;
    
    if (state->lineSpaceLeft >= 1 || state->state != HtmlCompilerState::Text) {
        htmlCompilerAppendTextChunk(state);
    } else {
        htmlCompilerAppendLine(state);
    }
    assert(state->lastTextChunk);
    state->lastTextChunk->text.assign(StringPtr::Policy::NonOwn, entity.utf8());
    --state->lineSpaceLeft;
}

Color TextViewPrivate::htmlCompilerParseColorName(const std::string& colorName, Color fallback)
{
    Color color = fallback;
    
    static const folly::sorted_vector_map<StringRef, Color> colors
    {
        // Html standart colors
        {"black",   ColorBlack  },
        {"silver",  ColorBlack  },
        {"gray",    ColorBlack  },
        {"maroon",  ColorRed    },
        {"red",     ColorRed    },
        {"navy",    ColorBlue   },
        {"blue",    ColorBlue   },
        {"purple",  ColorMagenta},
        {"fuchsia", ColorMagenta},
        {"green",   ColorGreen  },
        {"lime",    ColorGreen  },
        {"olive",   ColorYellow },
        {"yellow",  ColorYellow },
        {"teal",    ColorCyan   },
        {"aqua",    ColorCyan   },
        {"white",   ColorWhite  },
        
        // Ncurses colors
        {"magenta", ColorMagenta},
        {"cyan",    ColorCyan   }
    };
    
    auto it = colors.find(colorName.c_str());
    if (it != colors.end()) {
        color = it->second;
    }
    
    return color;
}

void TextViewPrivate::htmlCompilerAppendEmptyLine(TextViewPrivate::HtmlCompilerState *state)
{
    textLines.emplace_back(state);
    state->lastTextChunk = nullptr;
    state->lineSpaceLeft = 0;
}

void TextViewPrivate::htmlCompilerAppendLine(TextViewPrivate::HtmlCompilerState *state)
{
    textLines.emplace_back(state);
    state->lastTextChunk = &textLines.back();
    state->lineSpaceLeft = q->cols();
}

void TextViewPrivate::htmlCompilerAppendTextChunk(TextViewPrivate::HtmlCompilerState *state)
{
    if (state->lastTextChunk) {
        state->lastTextChunk->next = make_unique<TextChunk>(state);
        state->lastTextChunk = state->lastTextChunk->next.get();
    } else {
        htmlCompilerAppendLine(state);
    }
}

bool TextViewPrivate::isLastLineEmpty() const
{
    if (textLines.empty())
        return true;
    
    const TextChunk *chunk = &textLines.back();
    do {
        if (!chunk->text.isEmpty())
            return false;
        chunk = chunk->next.get();
    } while (chunk);
    return true;
}

void TextViewPrivate::compilePlainText(const std::string& text)
{
    textLines.clear();
    forEachLine(text.c_str(), [this](const char *lineBegin, const char *lineEnd) {
        textLines.emplace_back();
        textLines.back().text.assign(StringPtr::Policy::OwnCopy, lineBegin, lineEnd);
    });
}

void TextViewPrivate::viewportInit()
{
    viewportBeginLine = !textLines.empty() ? 0 : -1;
    q->update();
}

bool TextViewPrivate::canScrollDown() const
{
    // We should take into account that a line of text may not
    // fit into one line, i.e. we can't assume that:
    // viewportEndLine = viewportBeginLine + q->lines() , in fact
    // it can be less.
    int y = 0;
    int item = viewportBeginLine + 1;
    while (item < (int)textLines.size() && y < q->lines()) {
        // For utf8 encoded string real length is less or
        // equal to its size in bytes, thus if size in bytes
        // is less than q->cols() it is also true for real length
        // and we don't have to calculate it.
        if (lineSizeBytes(item) > q->cols()) {
            int extraLength = lineLength(item) - q->cols();
            while (extraLength > 0) {
                ++y;
                extraLength -= q->cols();
            }
        }
        ++y;
        ++item;
    }
    // We basically check if any space left at the bottom of the window
    return y >= q->lines();
}

int TextViewPrivate::lineSizeBytes(int line) const
{
    assert((size_t)line < textLines.size());
    int result = 0;
    const TextChunk *chunk = &textLines[line];
    do {
        result += chunk->text.size();
        chunk = chunk->next.get();
    } while (chunk);
    return result;
}

int TextViewPrivate::lineLength(int line) const
{
    assert((size_t)line < textLines.size());
    int result = 0;
    const TextChunk *chunk = &textLines[line];
    do {
        result += g_utf8_strlen(chunk->text.begin(), chunk->text.size());
        chunk = chunk->next.get();
    } while (chunk);
    return result;
}

void TextViewPrivate::scrollUp()
{
    if (viewportBeginLine > 0) {
        --viewportBeginLine;
        q->update();
    }
}

void TextViewPrivate::scrollDown()
{
    if (textLines.empty())
        return;

    if (canScrollDown()) {
        ++viewportBeginLine;
        q->update();
    }
}

void TextViewPrivate::scrollHome()
{
    viewportInit();
}

void TextViewPrivate::scrollEnd()
{
    if (textLines.size() > (size_t)q->lines()) {
        viewportBeginLine = textLines.size() - q->lines();
        while (canScrollDown())
            ++viewportBeginLine;
        q->update();
    }
}

void TextViewPrivate::scrollPageUp()
{
    if (textLines.empty())
        return;
    
    viewportBeginLine -= q->lines();
    if (viewportBeginLine < 0)
        viewportBeginLine = 0;
    q->update();
}

void TextViewPrivate::scrollPageDown()
{
    if (textLines.empty())
        return;

    if (viewportBeginLine + 2 * q->lines() > (int)textLines.size()) {
        scrollEnd();
    } else {
        viewportBeginLine += q->lines();
        q->update();
    }
}

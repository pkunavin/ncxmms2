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

#include <glib.h>
#include "QuestionWindow.h"

#include "../lib/Painter.h"
#include "../lib/Size.h"
#include "../lib/Rectangle.h"

using namespace ncxmms2;

QuestionWindow::QuestionWindow(int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent)
{
    setMinumumLines(1);
    setMaximumLines(1);
    loadPalette("QuestionWindow");
    m_answerEdit = new LineEdit(0, 0, cols, this);
    m_answerEdit->loadPalette("QuestionWindow");
}

void QuestionWindow::askQuestion(const std::string& question,
                                 const LineEdit::ResultCallback& answerCallback,
                                 const std::string& initialAnswer)
{
    m_htmlParser.parse(question);
    m_answerEdit->edit(answerCallback, initialAnswer);
    adjustSize();
}

void QuestionWindow::resizeChildren(const Size& size)
{
    NCXMMS2_UNUSED(size);
    adjustSize();
}

void QuestionWindow::adjustSize()
{
    int questionTextLength = calculateQuestionTextLength();
    if (questionTextLength > cols())
        questionTextLength = cols();

    m_answerEdit->resize(Size(cols() - questionTextLength, lines()));
    m_answerEdit->move(questionTextLength, 0);
}

int QuestionWindow::calculateQuestionTextLength() const
{
    int length = 0;
    for (const auto & token : m_htmlParser) {
        switch (token.type()) {
            case HtmlParser::Token::Type::Text:
                length += g_utf8_strlen(token.text().text().c_str(), token.text().text().size());
                break;

            case HtmlParser::Token::Type::Entity:
                ++length;
                break;

            default:
                break;
        }
    }
    return length;
}

void QuestionWindow::applyHtmlTag(const HtmlParser::TokenTag& tag, Painter * painter)
{
    switch (tag.type()) {
        case HtmlParser::TokenTag::Type::Bold:
            painter->setBold(tag.isStartTag());
            break;

        case HtmlParser::TokenTag::Type::Underline:
            painter->setUnderline(tag.isStartTag());
            break;

        case HtmlParser::TokenTag::Type::Blink:
            painter->setBlink(tag.isStartTag());
            break;

        default:
            break;
    }
}

void QuestionWindow::paint(const Rectangle &rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    for (const auto & token : m_htmlParser) {
        switch (token.type()) {
            case HtmlParser::Token::Type::Text:
                painter.printString(token.text().text());
                break;

            case HtmlParser::Token::Type::Entity:
                painter.printChar(token.entity().character());
                break;

            case HtmlParser::Token::Type::Tag:
                applyHtmlTag(token.tag(), &painter);
                break;
        }
    }
    painter.flush();
}

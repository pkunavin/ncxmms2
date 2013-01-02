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

#include "questionwindow.h"

#include "../lib/painter.h"
#include "../lib/size.h"
#include "../lib/rectangle.h"

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

void QuestionWindow::askQuestion(const std::string &question,
                                 const LineEdit::ResultCallback& answerCallback,
                                 const std::string& initialAnswer)
{
    m_question = question;
    m_answerEdit->edit(answerCallback, initialAnswer);
    adjustSize();
}

void QuestionWindow::adjustSize()
{
    m_answerEdit->resize(Size(cols() - m_question.size(), lines()));
    m_answerEdit->move(m_question.size(), 0);
}

void QuestionWindow::resize(const Size &size)
{
    Window::resize(size);
    adjustSize();
}

void QuestionWindow::paint(const Rectangle &rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.setBold(true);
    painter.printString(m_question);
    painter.flush();
}

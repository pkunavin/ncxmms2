/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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
#include "questionwindow.h"
#include "lib/label.h"

using namespace ncxmms2;

QuestionWindow::QuestionWindow(int cols, int yPos, int xPos, Window* parent) :
	Window(1, cols, yPos, xPos, parent)
{
	const int initialQuestionLabelSize=1;
	m_questionLabel=new Label(initialQuestionLabelSize, cols, 0, 0, this);
	m_questionLabel->setText("Some text");
	
	m_answerEdit=new LineEdit(cols-initialQuestionLabelSize, 0, initialQuestionLabelSize, this);
}

void QuestionWindow::askQuestion(const std::string &question,
                                 const LineEdit::ResultCallback& answerCallback,
                                 const std::string &initialAnswer)
{
	m_questionLabel->setText(question);
	m_answerEdit->edit(answerCallback, initialAnswer);
	adjustSize();
}

void QuestionWindow::adjustSize()
{
	//FIXME: Handle empty string. Maybe hide m_questionLabel ?
	const auto textSize=!m_questionLabel->text().empty() ? m_questionLabel->text().size() : 1;
	if ((std::string::size_type)cols() < textSize)
		throw std::runtime_error("QuestionWindow: terminal too small!");
	
	m_questionLabel->resizeEvent(Size(lines(), textSize));
	m_answerEdit->resizeEvent(Size(lines(), cols()-textSize));
	m_answerEdit->move(0, textSize);
}

void QuestionWindow::resizeEvent(const Size &size)
{
	Window::resizeEvent(size);
	adjustSize();
}

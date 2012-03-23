/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef QUESTIONWINDOW_H
#define QUESTIONWINDOW_H

#include "lib/window.h"
#include "lib/lineedit.h"

namespace ncxmms2 {

class Label;

class QuestionWindow : public Window
{
public:
    QuestionWindow(int cols, int yPos, int xPos, Window *parent = nullptr);

    void askQuestion(const std::string& question,
                     const LineEdit::ResultCallback& answerCallback,
                     const std::string& initialAnswer=std::string());

    virtual void resizeEvent(const Size &size);

private:
    Label *m_questionLabel;
    LineEdit *m_answerEdit;

    void adjustSize();
};
} // ncxmms2

#endif // QUESTIONWINDOW_H

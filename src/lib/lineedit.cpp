/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <string>
#include <glib.h>

#include "lineedit.h"
#include "utf.h"
#include "painter.h"
#include "palette.h"
#include "application.h"
#include "keyevent.h"
#include "size.h"
#include "rectangle.h"

namespace ncxmms2 {

class LineEditPrivate
{
public:
    LineEditPrivate(LineEdit *q_) :
        q(q_),
        textSelected(false),
        cursorPosition(0),
        viewportBegin(0) {}

    const LineEdit *q;

    LineEdit::ResultCallback resultCallback;

    std::u32string text;
    bool textSelected;
    std::u32string::size_type cursorPosition;
    std::u32string::size_type viewportBegin;
    typedef std::u32string::size_type TextSizeType;

    Signals::Connection focusLostSlotConnection;

    void returnResult(LineEdit::Result result);
    void clearText();

    void keyLeft();
    void keyRight();
    void keyHome();
    void keyEnd();
    void keyBackspace();
    void keyDelete();

    void addChar(char32_t ch);
};
} // ncxmms2

using namespace ncxmms2;

void LineEditPrivate::returnResult(LineEdit::Result result)
{
    Signals::ScopedConnectionBlock block(focusLostSlotConnection);
    if (resultCallback)
        resultCallback(u32stringToUtf8(text), result);
    Application::releaseFocus();
}

void LineEditPrivate::clearText()
{
    text.clear();
    textSelected = false;
    cursorPosition = 0;
    viewportBegin = 0;
}

void LineEditPrivate::keyLeft()
{
    textSelected = false;
    if (cursorPosition > 0) {
        if (cursorPosition == viewportBegin)
            --viewportBegin;
        --cursorPosition;
    }
}

void LineEditPrivate::keyRight()
{
    textSelected = false;
    if (cursorPosition < text.size()) {
        if (cursorPosition - viewportBegin == (TextSizeType)q->cols() - 1)
            ++viewportBegin;
        ++cursorPosition;
    }
}

void LineEditPrivate::keyHome()
{
    textSelected = false;
    cursorPosition = 0;
    viewportBegin = 0;
}

void LineEditPrivate::keyEnd()
{
    textSelected = false;
    cursorPosition = text.size();
    if (text.size() >= (TextSizeType)q->cols())
        viewportBegin = text.size() - q->cols() + 1;
}

void LineEditPrivate::keyBackspace()
{
    if (textSelected) {
        clearText();
    } else {
        if (cursorPosition > viewportBegin) {
            if (viewportBegin + q->cols() >= text.size()) {
                if (viewportBegin > 0) {
                    --viewportBegin;
                }
            }
        } else {
            if (viewportBegin > 0) {
                --viewportBegin;
            } else {
                return;
            }
        }
        --cursorPosition;
        text.erase(cursorPosition, 1);
    }
}

void LineEditPrivate::keyDelete()
{
    if (textSelected) {
        clearText();
    } else {
        if (cursorPosition < text.size()) {
            ++cursorPosition;
            keyBackspace();
        }
    }
}

void LineEditPrivate::addChar(char32_t ch)
{
    if (textSelected) {
        clearText();
    }

    if (cursorPosition < text.size()) {
        text.insert(cursorPosition, 1, ch);
    } else {
        text.push_back(ch);
    }

    if (cursorPosition-viewportBegin == (TextSizeType)q->cols() - 1)
        ++viewportBegin;
    ++cursorPosition;
}

LineEdit::LineEdit(int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, 1), parent),
    d(new LineEditPrivate(this))
{
    loadPalette("LineEdit");
    d->focusLostSlotConnection = focusLost_Connect([this](){d->returnResult(Result::Rejected);});
}

void LineEdit::edit(const ResultCallback& resultCallback, const std::string& text)
{
    edit(resultCallback, utf8ToU32String(text));
}

void LineEdit::edit(const ResultCallback& resultCallback, const std::u32string& text)
{
    d->resultCallback = resultCallback;
    d->text = text;
    d->cursorPosition = text.size();
    d->textSelected = !text.empty();
    setFocus();
    Application::grabFocus(this);
    update();
}

void LineEdit::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.isFunctionKey()) {
        switch(keyEvent.key()) {
            case KeyEvent::KeyEscape:    d->returnResult(Result::Rejected); return;
            case KeyEvent::KeyEnter:     d->returnResult(Result::Accepted); return;
            case KeyEvent::KeyLeft:      d->keyLeft();                      break;
            case KeyEvent::KeyRight:     d->keyRight();                     break;
            case KeyEvent::KeyHome:      d->keyHome();                      break;
            case KeyEvent::KeyEnd:       d->keyEnd();                       break;
            case KeyEvent::KeyBackspace: d->keyBackspace();                 break;
            case KeyEvent::KeyDelete:    d->keyDelete();                    break;
        }
    } else if (g_unichar_isprint(keyEvent.key())) {
        d->addChar(keyEvent.key());
    }
    update();
}

void LineEdit::resize(const Size& size)
{
    if (size.cols() > cols()) {
        int extraSize = size.cols() - cols();
        if (d->viewportBegin + cols() < d->text.size())
            extraSize -= d->text.size() - (d->viewportBegin + cols());
        if (extraSize > 0) {
            if ((LineEditPrivate::TextSizeType)extraSize > d->viewportBegin) {
                d->viewportBegin = 0;
            } else {
                d->viewportBegin -= extraSize;
            }
        }
    } else {
        int sizeDiff = cols() - size.cols();
        sizeDiff -= cols() - (d->cursorPosition - d->viewportBegin + 1);
        if (sizeDiff > 0)
            d->viewportBegin += sizeDiff;
    }

    Window::resize(size);
}

void LineEdit::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearLine();

    const Palette::ColorGroup colorGroup = hasFocus()
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;
    if (d->textSelected) {
        painter.setBackgroundColor(palette().color(colorGroup, Palette::RoleHighlightedText));
        painter.setColor(palette().color(colorGroup, Palette::RoleHighlight));
        painter.setReverse(true);
    }
    painter.printString(d->text.c_str() + d->viewportBegin, cols());
    const auto cursorPosition = d->cursorPosition;
    painter.move(cursorPosition - d->viewportBegin, 0);
    painter.setColor(palette().color(colorGroup, Palette::RoleText));
    painter.setReverse(true);
    painter.printChar(cursorPosition < d->text.size() ? d->text[cursorPosition] : char32_t(' '));
    painter.flush();
}

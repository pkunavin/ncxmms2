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

#include <string>
#include <glib.h>

#include "lineedit.h"
#include "painter.h"
#include "application.h"
#include "keyevent.h"
#include "size.h"
#include "rectangle.h"

namespace ncxmms2 {

class LineEditPrivate
{
public:
    LineEditPrivate(LineEdit *q_) : q(q_), cursorPosition(0), viewportBegin(0) {}

    const LineEdit *q;

    LineEdit::ResultCallback resultCallback;

    std::wstring text;
    std::wstring::size_type cursorPosition;
    std::wstring::size_type viewportBegin;
    typedef std::wstring::size_type TextSizeType;

    void returnResult(LineEdit::ResultCode result);

    void keyLeft();
    void keyRight();
    void keyHome();
    void keyEnd();
    void keyBackspace();
    void keyDelete();

    void addChar(wchar_t ch);

    static std::wstring utf8ToWString(const std::string& str)
    {
        std::wstring result;
        if (!g_utf8_validate(str.c_str(), str.size(), NULL))
            return result;

        result.reserve(str.size());

        const char *c_str = str.c_str();
        while (*c_str) {
            result.push_back(g_utf8_get_char(c_str));
            c_str = g_utf8_next_char(c_str);
        }
        return result;
    }

    static std::string wStringToUtf8(const std::wstring& str)
    {
        std::string result;
        result.reserve(str.size() * 3);

        char buf[6];
        int n;
        for (wchar_t ch : str) {
            n = g_unichar_to_utf8(ch, buf);
            result.append(buf, n);
        }

        return result;
    }
};
} // ncxmms2

using namespace ncxmms2;

void LineEditPrivate::returnResult(LineEdit::ResultCode result)
{
    if (!resultCallback.empty())
        resultCallback(wStringToUtf8(text), result);
    Application::releaseFocus();
}

void LineEditPrivate::keyLeft()
{
    if (cursorPosition > 0) {
        if (cursorPosition == viewportBegin)
            --viewportBegin;
        --cursorPosition;
    }
}

void LineEditPrivate::keyRight()
{
    if (cursorPosition < text.size()) {
        if (cursorPosition - viewportBegin == (TextSizeType)q->cols() - 1)
            ++viewportBegin;
        ++cursorPosition;
    }
}

void LineEditPrivate::keyHome()
{
    cursorPosition = 0;
    viewportBegin = 0;
}

void LineEditPrivate::keyEnd()
{
    cursorPosition = text.size();
    if (text.size() >= (TextSizeType)q->cols())
        viewportBegin = text.size() - q->cols() + 1;
}

void LineEditPrivate::keyBackspace()
{
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

void LineEditPrivate::keyDelete()
{
    if (cursorPosition < text.size()) {
        ++cursorPosition;
        keyBackspace();
    }
}

void LineEditPrivate::addChar(wchar_t ch)
{
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

}

void LineEdit::edit(const ResultCallback& resultCallback, const std::string& text)
{
    edit(resultCallback, LineEditPrivate::utf8ToWString(text));
}

void LineEdit::edit(const ResultCallback& resultCallback, const std::wstring& text)
{
    d->resultCallback = resultCallback;
    d->text = text;
    d->cursorPosition = 0;
    Application::grabFocus(this);
    update();
}

void LineEdit::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.isFunctionKey()) {
        switch(keyEvent.key()) {
            case KeyEvent::KeyEscape:    d->returnResult(Rejected); return;
            case KeyEvent::KeyEnter:     d->returnResult(Accepted); return;
            case KeyEvent::KeyLeft:      d->keyLeft();              break;
            case KeyEvent::KeyRight:     d->keyRight();             break;
            case KeyEvent::KeyHome:      d->keyHome();              break;
            case KeyEvent::KeyEnd:       d->keyEnd();               break;
            case KeyEvent::KeyBackspace: d->keyBackspace();         break;
            case KeyEvent::KeyDelete:    d->keyDelete();            break;
        }
    } else if (g_unichar_isprint(keyEvent.key())) {
        d->addChar(keyEvent.key());
    }
    update();
}

void LineEdit::resize(const Size &size)
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
    painter.printString(d->text.c_str() + d->viewportBegin, cols());
    const auto cursorPosition = d->cursorPosition;
    painter.move(cursorPosition - d->viewportBegin, 0);
    painter.setReverse(true);
    painter.printChar(cursorPosition < d->text.size() ? d->text[cursorPosition] : wchar_t(' '));
    painter.flush();
}

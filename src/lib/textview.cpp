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

#include "textview.h"
#include "painter.h"
#include "keyevent.h"
#include "mouseevent.h"
#include "size.h"

namespace ncxmms2 {

class TextViewPrivate
{
public:
    TextViewPrivate(TextView *q_) :
        q(q_),
        viewportBeginLine(-1),
        viewportEndLine(-1) {}

    TextView *q;

    int viewportBeginLine;
    int viewportEndLine;

    std::vector<std::string> textLines;

    void splitTextIntoLines(const std::string& text);
    void viewportInit();
    void scrollUp();
    void scrollDown();
};
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

void TextView::setText(const std::string& text)
{
    d->splitTextIntoLines(text);
    d->viewportInit();
}

void TextView::appendLine(const std::string& line)
{
    d->textLines.push_back(line);

    if (d->textLines.size() <= (size_t)lines()) {
        d->viewportBeginLine = 0;
        d->viewportEndLine = d->textLines.size();
        update();
    }
}

void TextView::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:   d->scrollUp();   break;
        case KeyEvent::KeyDown: d->scrollDown(); break;
        default: break;
    }
}

void TextView::mouseEvent(const MouseEvent &ev)
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
    const int linesCount = d->textLines.size();

    if (linesCount > 0) {
        if (size.lines() > lines()) {
            int extraSize = size.lines() - lines();

            if (linesCount > d->viewportEndLine) {
                const int min = std::min(linesCount - d->viewportEndLine, extraSize);
                d->viewportEndLine += min;
                extraSize -= min;
            }

            if (extraSize > 0 && d->viewportBeginLine > 0) {
                const int min = std::min(d->viewportBeginLine, extraSize);
                d->viewportBeginLine -= min;
            }
        }

        if (size.lines() < lines()) {
            int sizeDiff = lines() - size.lines();

            if (lines() > linesCount)
                sizeDiff -= std::min(lines() - linesCount, sizeDiff);

            if (sizeDiff > 0)
                d->viewportEndLine -= sizeDiff;
           }
    }

    Window::resize(size);
}

void TextView::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);

    if (!d->textLines.empty()) {
        const int lastLine = std::min(d->viewportBeginLine + lines(), d->viewportEndLine);
        const int maxCols = cols();
        for (int line = d->viewportBeginLine; line < lastLine; ++line) {
            painter.clearLine(line - d->viewportBeginLine);
            // FIXME: For now text line is just truncated if it
            //        doesn't fit in one line
            painter.printString(d->textLines[line], maxCols);
        }

        for (int line = d->viewportEndLine; line < lines(); ++line) {
            painter.clearLine(line);
        }
    } else {
        painter.clearWindow();
    }

    painter.flush();
}

void TextViewPrivate::splitTextIntoLines(const std::string& text)
{
    textLines.clear();

    std::string::size_type lineBegin = 0;
    std::string::size_type lineEnd;

    for (;;) {
        lineEnd = text.find('\n', lineBegin);
        if (lineEnd == std::string::npos) {
            textLines.emplace_back(text, lineBegin);
            break;
        } else {
            textLines.emplace_back(text, lineBegin, lineEnd - lineBegin);
            if (lineEnd + 1 >= text.size())
                break;
            lineBegin = lineEnd + 1;
        }
    }
}

void TextViewPrivate::viewportInit()
{
    if (!textLines.empty()) {
        viewportBeginLine = 0;
        viewportEndLine = textLines.size() > (size_t)q->lines()
                          ? q->lines()
                          : textLines.size();
    } else {
        viewportBeginLine = -1;
        viewportEndLine = -1;
    }
    q->update();
}

void TextViewPrivate::scrollUp()
{
    if (viewportBeginLine > 0) {
        --viewportBeginLine;
        --viewportEndLine;
        q->update();
    }
}

void TextViewPrivate::scrollDown()
{
    if (textLines.empty())
        return;

    if ((size_t)viewportEndLine < textLines.size() ) {
        ++viewportBeginLine;
        ++viewportEndLine;
        q->update();
    }
}


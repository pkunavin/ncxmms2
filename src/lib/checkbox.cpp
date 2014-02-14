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

#include "checkbox.h"
#include "application.h"
#include "palette.h"
#include "painter.h"
#include "rectangle.h"
#include "keyevent.h"
#include "mouseevent.h"

namespace ncxmms2 {

class CheckBoxPrivate
{
public:
    CheckBoxPrivate() :
        isChecked(false) {}

    bool isChecked;
    std::string text;
};
} // ncxmms2

using namespace ncxmms2;

CheckBox::CheckBox(int x, int y, int cols, Window *parent) :
    Window(Rectangle(x, y, cols, 1), parent),
    d(new CheckBoxPrivate())
{
    loadPalette("CheckBox");
    setMinumumCols(5);
}

CheckBox::~CheckBox()
{

}

const std::string& CheckBox::text() const
{
    return d->text;
}

void CheckBox::setText(const std::string& text)
{
    d->text = text;
    update();
}

bool CheckBox::isChecked() const
{
    return d->isChecked;
}

void CheckBox::setChecked(bool checked)
{
    if (d->isChecked != checked) {
        d->isChecked = checked;
        update();
        checkStateChanged(checked);
    }
}

void CheckBox::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyEnter:
        case ' ':
            setChecked(!isChecked());
            activated(d->isChecked);
            break;

        default: Window::keyPressedEvent(keyEvent);
    }
}

void CheckBox::mouseEvent(const MouseEvent& ev)
{
    if (   ev.button() == MouseEvent::ButtonLeft
        && ev.type() == MouseEvent::Type::ButtonPress && ev.position().x() <= 2) {
        setChecked(!isChecked());
        activated(d->isChecked);
    }
}

void CheckBox::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearWindow();
    if (hasFocus()) {
        if (Application::useColors()) {
            painter.setColorPair(palette().color(Palette::GroupActive, Palette::RoleHighlightedText),
                                 palette().color(Palette::GroupActive, Palette::RoleHighlight));
        } else {
            painter.setReverse(true);
        }
    }
    painter.setBold(true);
    painter.printString(d->isChecked ? "[X] " : "[ ] ");
    painter.setBold(false);
    painter.squeezedPrint(d->text, cols() - painter.x());
    painter.flush();
}

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

#include <vector>

#include "radiobuttongroupbox.h"
#include "application.h"
#include "painter.h"
#include "palette.h"
#include "keyevent.h"
#include "mouseevent.h"

namespace ncxmms2 {

class RadioButtonGroupBoxPrivate
{
public:
    RadioButtonGroupBoxPrivate() :
        checkedRadioButton(-1),
        activeRadioButton(-1) {}

    std::string groupName;
    std::vector<std::string> radioButtons;
    int checkedRadioButton;
    int activeRadioButton;
};
} // ncxmms2

using namespace ncxmms2;

RadioButtonGroupBox::RadioButtonGroupBox(const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    d(new RadioButtonGroupBoxPrivate())
{
    loadPalette("RadioButtonGroupBox");
}

RadioButtonGroupBox::~RadioButtonGroupBox()
{

}

const std::string& RadioButtonGroupBox::groupName() const
{
    return d->groupName;
}

void RadioButtonGroupBox::setGroupName(const std::string& groupName)
{
    d->groupName = groupName;
    update();
}

void RadioButtonGroupBox::addRadioButton(const std::string& buttonText)
{
    d->radioButtons.push_back(buttonText);
    if (d->activeRadioButton == -1)
        d->activeRadioButton = 0;
    update();
}

int RadioButtonGroupBox::checkedRadioButton() const
{
    return d->checkedRadioButton;
}

void RadioButtonGroupBox::setCheckedRadioButton(int index)
{
    assert(index >= 0 && (size_t)index < d->radioButtons.size());
    if (d->checkedRadioButton != index) {
        d->checkedRadioButton = index;
        update();
        checkedRadioButtonChanged(d->checkedRadioButton);
    }
}

int RadioButtonGroupBox::activeRadioButton() const
{
    return d->activeRadioButton;
}

void RadioButtonGroupBox::setActiveRadioButton(int index)
{
    assert(index >= 0 && (size_t)index < d->radioButtons.size());
    d->activeRadioButton = index;
    update();
}

const std::string& RadioButtonGroupBox::radioButtonText(int index) const
{
    assert(index >= 0 && (size_t)index < d->radioButtons.size());
    return d->radioButtons[index];
}

const std::string &RadioButtonGroupBox::checkedRadioButtonText() const
{
    return radioButtonText(d->checkedRadioButton);
}

void RadioButtonGroupBox::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (d->radioButtons.empty())
        return;

    assert(d->activeRadioButton >= 0);

    switch (keyEvent.key()) {
        case KeyEvent::KeyEnter:
        case ' ':
            if (d->activeRadioButton != -1) {
                setCheckedRadioButton(d->activeRadioButton);
                activated(d->checkedRadioButton);
            }
            break;

        case KeyEvent::KeyUp:
            if (d->activeRadioButton > 0) {
                --d->activeRadioButton;
                update();
            }
            break;

        case KeyEvent::KeyDown:
            if ((size_t)d->activeRadioButton < d->radioButtons.size() - 1) {
                ++d->activeRadioButton;
                update();
            }
            break;

        default:
            break;
    }
}

void RadioButtonGroupBox::mouseEvent(const MouseEvent& ev)
{
    if (ev.button() != MouseEvent::ButtonLeft || ev.type() != MouseEvent::Type::ButtonPress)
        return;
    
    const Point& pos = ev.position();
    if (pos.x() < 1 || pos.x() >= cols() - 1 || pos.y() < 1 || pos.y() >= lines() - 1)
        return;
        
    d->activeRadioButton = pos.y() - 1;
    if (pos.x() >=1 && pos.x() <= 3) {
        setCheckedRadioButton(d->activeRadioButton);
        activated(d->checkedRadioButton);
    } else {
        update();
    }
}

void RadioButtonGroupBox::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearWindow();
    painter.drawBox();

    painter.move(2, 0);
    painter.squeezedPrint(d->groupName, cols() - 2 - 1);

    for (int i = 0; i < (int)d->radioButtons.size(); ++i) {
        if (i == d->activeRadioButton)
            continue;
        painter.move(1, i + 1);
        painter.setBold(true);
        painter.printString(i == d->checkedRadioButton ? "(*) " : "( ) ");
        painter.setBold(false);
        painter.squeezedPrint(d->radioButtons[i], cols() - 1 - 4 - 1);
    }

    if (d->activeRadioButton >= 0) {
        if (hasFocus()) {
            if (Application::useColors()) {
                painter.setColorPair(palette().color(Palette::GroupActive, Palette::RoleHighlightedText),
                                     palette().color(Palette::GroupActive, Palette::RoleHighlight));
            } else {
                painter.setReverse(true);
            }
        }
        painter.move(1, d->activeRadioButton + 1);
        painter.setBold(true);
        painter.printString(d->activeRadioButton == d->checkedRadioButton ? "(*) " : "( ) ");
        painter.setBold(false);
        painter.squeezedPrint(d->radioButtons[d->activeRadioButton], cols() - 1 - 4 - 1);
    }
    painter.flush();
}

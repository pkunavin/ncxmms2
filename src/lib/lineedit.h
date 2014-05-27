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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include "window.h"

namespace ncxmms2 {

class LineEditPrivate;

class LineEdit : public Window
{
public:
    LineEdit(int xPos, int yPos, int cols, Window *parent = nullptr);

    enum class Result
    {
        Rejected,
        Accepted
    };
    typedef std::function<void (const std::string&, Result)> ResultCallback;

    void edit(const ResultCallback& resultCallback, const std::string& text = std::string());
    void edit(const ResultCallback& resultCallback, const std::u32string& text = std::u32string());

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resize(const Size& size);

protected:
    virtual void paint(const Rectangle& rect);

private:
    std::unique_ptr<LineEditPrivate> d;
};
} // ncxmms2

#endif // LINEEDIT_H

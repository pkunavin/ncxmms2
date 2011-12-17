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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include "window.h"

namespace ncxmms2 {

class LineEditPrivate;

class LineEdit : public Window
{
public:
    LineEdit(int cols, int yPos, int xPos, Window *parent = 0);

    enum ResultCode
    {
        Rejected,
        Accepted
    };
    typedef boost::function<void (const std::string&, ResultCode)> ResultCallback;

    void edit(const ResultCallback& resultCallback, const std::string& text = std::string());
    void edit(const ResultCallback& resultCallback, const std::wstring& text = std::wstring());

    virtual void keyPressedEvent(const KeyEvent& keyEvent);
    virtual void resizeEvent(const Size& size);

protected:
    virtual void showEvent();

private:
    std::unique_ptr<LineEditPrivate> d;
};
} // ncxmms2

#endif // LINEEDIT_H

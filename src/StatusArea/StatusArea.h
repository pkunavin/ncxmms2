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

#ifndef STATUSAREA_H
#define STATUSAREA_H

#include "../Utils.h"
#include "../XmmsUtils/Result.h"

#include "../lib/Window.h"
#include "../lib/LineEdit.h"
#include "../lib/Timer.h"

namespace ncxmms2 {

namespace xmms2 {
class Client;
}

class PlaybackProgressBar;
class StackedWindow;

class StatusArea : public Window
{
public:
    StatusArea(xmms2::Client *client, int xPos, int yPos, int cols, Window *parent = nullptr);
    ~StatusArea();

    static void showMessage(const std::string& message);

    template <typename Arg, typename... Args>
    static void showMessage(const char *fmt, Arg&& arg, Args&&... args)
    {
        showMessage(Utils::format(fmt, std::forward<Arg>(arg), std::forward<Args>(args)...));
    }

    static void askQuestion(const std::string& question,
                            const LineEdit::ResultCallback& answerCallback,
                            const std::string& initialAnswer = std::string());

    xmms2::PlaybackStatus playbackStatus() const;

    enum
    {
        PlaybackProgressLine,
        InformationLine,

        LinesNumber
    };

protected:
    virtual void resizeChildren(const Size& size);

private:
    static StatusArea *inst;

    PlaybackProgressBar *m_playbackProgressBar;

    enum StackedWindows
    {
        StackedPlaybackStatusWindow,
        StackedMessageWindow,
        StackedQuestionWindow
    };

    StackedWindow *m_stackedWindow;
    Timer m_timer;

    void _showMessage(const std::string& message);
    void _askQuestion(const std::string& question,
                      const LineEdit::ResultCallback& answerCallback,
                      const std::string& initialAnswer = std::string());
    
    void getPlaytime(const xmms2::Expected<int>& playtime);
};
} // ncxmms2

#endif // STATUSAREA_H

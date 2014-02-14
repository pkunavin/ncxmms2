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

#ifndef STATUSAREA_H
#define STATUSAREA_H

#include <xmmsclient/xmmsclient++/playback.h>

#include "../song.h"
#include "../utils.h"

#include "../lib/window.h"
#include "../lib/lineedit.h"
#include "../lib/timer.h"

namespace ncxmms2 {

class PlaybackProgressBar;
class StackedWindow;

class StatusArea : public Window
{
public:
    StatusArea(Xmms::Client *client, int xPos, int yPos, int cols, Window *parent = nullptr);
    ~StatusArea();

    static void showMessage(const std::string& message);

    template <typename T, typename Arg, typename... Args>
    static void showMessage(T&& str, Arg&& arg, Args&&... args)
    {
        showMessage(Utils::format(std::forward<T>(str), std::forward<Arg>(arg), std::forward<Args>(args)...));
    }

    static void askQuestion(const std::string& question,
                            const LineEdit::ResultCallback& answerCallback,
                            const std::string& initialAnswer = std::string());

    Xmms::Playback::Status playbackStatus() const;

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
};
} // ncxmms2

#endif // STATUSAREA_H

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

#include <map>
#include <algorithm>
#include <stdexcept>

#include "statusarea.h"
#include "playbackprogressbar.h"
#include "playbackstatuswindow.h"
#include "questionwindow.h"

#include "lib/label.h"
#include "lib/stackedwindow.h"

using namespace ncxmms2;

StatusArea *StatusArea::inst = 0;

StatusArea::StatusArea(Xmms::Client *client, int lines, int cols, int yPos, int xPos, Window *parent) :
    Window(lines, cols, yPos, xPos, parent)
{
    if (inst) {
        throw std::logic_error(std::string(__PRETTY_FUNCTION__).append("; instance of StatusArea already exists!"));
    } else {
        inst = this;
    }

    const int stackedWindowLines = 1;
    m_stackedWindow = new StackedWindow(stackedWindowLines, cols, 1, 0, this);

    const std::map <StackedWindows, Window*> stackedWins =
    {
        {StackedPlaybackStatusWindow, new PlaybackStatusWindow(client, stackedWindowLines, cols, 0, 0, m_stackedWindow)},
        {StackedMessageWindow,        new Label(stackedWindowLines, cols, 0, 0, m_stackedWindow)},
        {StackedQuestionWindow,       new QuestionWindow(cols, 0, 0, m_stackedWindow)}
    };

    std::for_each(stackedWins.begin(), stackedWins.end(), [this](const std::pair<StackedWindows, Window*>& value)
    {
        m_stackedWindow->addWindow(value.second);
    });
    m_stackedWindow->setFocus();
    m_stackedWindow->setCurrentIndex(StackedPlaybackStatusWindow);

    m_playbackProgressBar = new PlaybackProgressBar(stackedWindowLines, cols, 0, 0, this);
    auto *playbackStatusWin = static_cast<PlaybackStatusWindow*>(m_stackedWindow->window(StackedPlaybackStatusWindow));
    playbackStatusWin->setPlaytimeChangedCallback(boost::bind(&PlaybackProgressBar::setValue, m_playbackProgressBar, _1));
    playbackStatusWin->setCurrentSongChangedCallback([this](const Song& song)
    {
        m_playbackProgressBar->setMaxValue(song.duration());
    });

    m_timer.connectTimeoutSignal([this]()
    {
        m_timer.stop();
        m_stackedWindow->setCurrentIndex(StackedPlaybackStatusWindow);
    });
}

StatusArea::~StatusArea()
{
    inst = 0;
}

void StatusArea::showMessage(const std::string& message)
{
    if (inst) {
        inst->_showMessage(message);
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__).append(" : there is no instance of StatusArea!"));
    }
}

void StatusArea::_showMessage(const std::string& message)
{
    static_cast<Label*>(m_stackedWindow->window(StackedMessageWindow))->setText(message);
    m_stackedWindow->setCurrentIndex(StackedMessageWindow);
    m_timer.start(5);
}

void StatusArea::askQuestion(const std::string& question,
                             const LineEdit::ResultCallback& answerCallback,
                             const std::string& initialAnswer)
{
    if (inst) {
        inst->_askQuestion(question, answerCallback, initialAnswer);
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__).append(" : there is no instance of StatusArea!"));
    }
}

void StatusArea::_askQuestion(const std::string& question,
                              const LineEdit::ResultCallback& answerCallback,
                              const std::string& initialAnswer)
{
    auto resultCallback = [answerCallback, this](const std::string& answer, LineEdit::ResultCode result)
    {
        m_stackedWindow->setCurrentIndex(StackedPlaybackStatusWindow);
        if (!answerCallback.empty())
            answerCallback(answer, result);
    };

    QuestionWindow *questionWin = static_cast<QuestionWindow*>(m_stackedWindow->window(StackedQuestionWindow));
    questionWin->askQuestion(question, resultCallback, initialAnswer);
    m_stackedWindow->setCurrentIndex(StackedQuestionWindow);
    m_timer.stop();
}

void StatusArea::resizeEvent(const Size &size)
{
    Window::resizeEvent(size);
    m_playbackProgressBar->resizeEvent(Size(m_playbackProgressBar->lines(), size.cols()));
    m_stackedWindow->resizeEvent(Size(m_stackedWindow->lines(), size.cols()));
}

Xmms::Playback::Status StatusArea::playbackStatus() const
{
    return static_cast<PlaybackStatusWindow*>(m_stackedWindow->window(StackedPlaybackStatusWindow))->playbackStatus();
}

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

#include <algorithm>
#include <stdexcept>

#include "statusarea.h"
#include "playbackprogressbar.h"
#include "playbackstatuswindow.h"
#include "questionwindow.h"

#include "../lib/label.h"
#include "../lib/stackedwindow.h"
#include "../lib/size.h"
#include "../lib/rectangle.h"

#include "../../3rdparty/folly/sorted_vector_types.h"

using namespace ncxmms2;

StatusArea *StatusArea::inst = nullptr;

StatusArea::StatusArea(Xmms::Client *client, int xPos, int yPos, int cols, Window *parent) :
    Window(Rectangle(xPos, yPos, cols, LinesNumber), parent)
{
    if (inst) {
        throw std::logic_error(std::string(__PRETTY_FUNCTION__).append("; instance of StatusArea already exists!"));
    } else {
        inst = this;
    }

    setMaximumLines(LinesNumber);
    setMaximumLines(LinesNumber);

    m_stackedWindow = new StackedWindow(Rectangle(0, InformationLine, cols, 1), this);

    const folly::sorted_vector_map<StackedWindows, Window*> stackedWins
    {
        {StackedPlaybackStatusWindow, new PlaybackStatusWindow(client, 0, 0, cols, m_stackedWindow)},
        {StackedMessageWindow,        new Label(0, 0, cols, m_stackedWindow)                       },
        {StackedQuestionWindow,       new QuestionWindow(0, 0, cols, m_stackedWindow)              }
    };
    for (const auto& pair : stackedWins) {
        m_stackedWindow->addWindow(pair.second);
    }
    
    m_stackedWindow->setFocus();
    m_stackedWindow->setCurrentIndex(StackedPlaybackStatusWindow);
    m_stackedWindow->window(StackedMessageWindow)->loadPalette("StatusMessageWindow");

    m_playbackProgressBar = new PlaybackProgressBar(0, 0, cols, this);
    m_playbackProgressBar->progressChangeRequested_Connect([client](int value){
        client->playback.seekMs(value);
    });
    auto *playbackStatusWin = static_cast<PlaybackStatusWindow*>(m_stackedWindow->window(StackedPlaybackStatusWindow));
    playbackStatusWin->playtimeChanged_Connect(&PlaybackProgressBar::setValue, m_playbackProgressBar);
    playbackStatusWin->currentSongChanged_Connect([this](const Song& song)
    {
        m_playbackProgressBar->setMaxValue(song.duration());
    });

    m_timer.setSingleShot(true);
    m_timer.timeout_Connect([this](){
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
    auto resultCallback = [answerCallback, this](const std::string& answer, LineEdit::Result result)
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

Xmms::Playback::Status StatusArea::playbackStatus() const
{
    return static_cast<PlaybackStatusWindow*>(m_stackedWindow->window(StackedPlaybackStatusWindow))->playbackStatus();
}

void StatusArea::resizeChildren(const Size& size)
{
    m_playbackProgressBar->resize(Size(size.cols(), m_playbackProgressBar->lines()));
    m_stackedWindow->resize(Size(size.cols(), m_stackedWindow->lines()));
}

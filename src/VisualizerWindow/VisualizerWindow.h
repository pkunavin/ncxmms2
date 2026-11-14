/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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

#ifndef VISUALIZERWINDOW_H
#define VISUALIZERWINDOW_H

#include <memory>
#include <string>
#include <vector>

#include "../lib/Colors.h"
#include "../lib/Window.h"

namespace ncxmms2 {

namespace xmms2 {
class VisualizationClient;
}

class Painter;
class Timer;

class VisualizerWindow : public Window
{
public:
    VisualizerWindow(const std::string& ipcPath, const Rectangle& rect, Window *parent = nullptr);
    ~VisualizerWindow();

protected:
    virtual void paint(const Rectangle& rect);
    virtual void showEvent();

private:
    enum
    {
        BandsNumber = 16,
        HistoryFrames = 4096,
        SpectrumFftSize = HistoryFrames,
        SpectrumBins = SpectrumFftSize / 2,
        PcmSamples = 1024,
        MaxPendingChunks = 512
    };

    std::string m_ipcPath;
    std::unique_ptr<xmms2::VisualizationClient> m_visualizationClient;
    Timer *m_refreshTimer;
    int m_fps;
    int m_idleTicks;
    int m_noDataTicks;
    bool m_gotData;
    bool m_showHint;
    std::string m_errorMessage;

    std::vector<short> m_leftHistory;
    std::vector<short> m_rightHistory;
    std::vector<float> m_window;
    std::vector<float> m_leftBarHeights;
    std::vector<float> m_rightBarHeights;
    std::vector<int> m_bandFirstBins;
    std::vector<int> m_bandLastBins;

    void initializeVisualization();
    void updateSpectrum();
    void appendPcm(const short *pcm, int samples);
    void updateBarHeights(bool newData);
    void computeBandLevels(const short *samples, float *levels);
    void drawBar(Painter& painter, int x, int width, float level, int height);
    void stopWithError(const std::string& message);
    Color barColor(int row, int height) const;
};
} // ncxmms2

#endif // VISUALIZERWINDOW_H

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

#include <algorithm>
#include <cmath>
#include <cstring>

#include "VisualizerWindow.h"
#include "../Settings.h"
#include "../XmmsUtils/VisualizationClient.h"

#include "../lib/Painter.h"
#include "../lib/Palette.h"
#include "../lib/Rectangle.h"
#include "../lib/Timer.h"

using namespace ncxmms2;

namespace {

// Custom palette roles used to draw a color gradient from the bottom of
// the bars to their tops.
enum
{
    RoleBarLow = Palette::RoleUser,
    RoleBarMid,
    RoleBarHigh
};

const float Pi = 3.14159265358979323846f;
const int SpectrumSampleRate = 44100;

// Center frequencies of the 16 bands (2/3-octave ISO series).
const double BandFrequencies[] =
{
    20.0, 31.5, 50.0, 80.0, 125.0, 200.0, 315.0, 500.0,
    800.0, 1250.0, 2000.0, 3150.0, 5000.0, 8000.0, 12500.0, 20000.0
};
const int BandFrequenciesNumber = sizeof(BandFrequencies) / sizeof(BandFrequencies[0]);

const float SpectrumDynamicRangeDb = 60.0f;

const char32_t FullBlock = 0x2588; // █
const char32_t PartialBlocks[] =
{
    0,
    0x2581, // ▁
    0x2582, // ▂
    0x2583, // ▃
    0x2584, // ▄
    0x2585, // ▅
    0x2586, // ▆
    0x2587  // ▇
};

// In-place radix-2 Cooley-Tukey FFT.
void fft(float *real, float *imag, int n)
{
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;

        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }

    for (int length = 2; length <= n; length <<= 1) {
        const float angle = -2.0f * Pi / length;
        const float wLengthReal = std::cos(angle);
        const float wLengthImag = std::sin(angle);

        for (int i = 0; i < n; i += length) {
            float wReal = 1.0f;
            float wImag = 0.0f;
            for (int k = 0; k < length / 2; ++k) {
                const float evenReal = real[i + k];
                const float evenImag = imag[i + k];
                const float oddReal = real[i + k + length / 2] * wReal
                                    - imag[i + k + length / 2] * wImag;
                const float oddImag = real[i + k + length / 2] * wImag
                                    + imag[i + k + length / 2] * wReal;

                real[i + k] = evenReal + oddReal;
                imag[i + k] = evenImag + oddImag;
                real[i + k + length / 2] = evenReal - oddReal;
                imag[i + k + length / 2] = evenImag - oddImag;

                const float nextWReal = wReal * wLengthReal - wImag * wLengthImag;
                wImag = wReal * wLengthImag + wImag * wLengthReal;
                wReal = nextWReal;
            }
        }
    }
}

} // namespace

VisualizerWindow::VisualizerWindow(const std::string& ipcPath, const Rectangle& rect,
                                   Window *parent) :
    Window(rect, parent),
    m_ipcPath(ipcPath),
    m_visualizationClient(new xmms2::VisualizationClient()),
    m_refreshTimer(new Timer(this)),
    m_fps(Settings::value("VisualizerScreen", "fps", 40)),
    m_idleTicks(0),
    m_noDataTicks(0),
    m_gotData(false),
    m_showHint(false)
{
    setName("Visualizer");

    loadPalette("VisualizerWindow",
                {
                    {"BarLow",  RoleBarLow },
                    {"BarMid",  RoleBarMid },
                    {"BarHigh", RoleBarHigh}
                });

    if (m_fps < 1)
        m_fps = 1;
    else if (m_fps > 120)
        m_fps = 120;

    m_leftHistory.assign(HistoryFrames, short(0));
    m_rightHistory.assign(HistoryFrames, short(0));
    m_leftBarHeights.assign(BandsNumber, 0.0f);
    m_rightBarHeights.assign(BandsNumber, 0.0f);

    m_window.resize(SpectrumFftSize);
    for (int i = 0; i < SpectrumFftSize; ++i) {
        m_window[i] = 0.5f - 0.5f * std::cos(2.0f * Pi * i / SpectrumFftSize);
    }

    // Precalculate FFT bin ranges for the bands.
    m_bandFirstBins.resize(BandsNumber);
    m_bandLastBins.resize(BandsNumber);
    const double nyquist = SpectrumSampleRate / 2.0;
    for (int band = 0; band < BandsNumber; ++band) {
        const double center = BandFrequencies[band];
        const double low = band == 0
                           ? center * center / BandFrequencies[1]
                           : std::sqrt(BandFrequencies[band - 1] * center);
        const double high = band + 1 == BandFrequenciesNumber
                            ? nyquist
                            : std::sqrt(center * BandFrequencies[band + 1]);

        int firstBin = int(std::ceil(low * SpectrumFftSize / SpectrumSampleRate));
        int lastBin = int(std::ceil(high * SpectrumFftSize / SpectrumSampleRate)) - 1;

        // Skip the DC bin
        firstBin = std::max(1, firstBin);
        lastBin = std::min(SpectrumBins - 1, lastBin);

        if (lastBin < firstBin) {
            // The band is narrower than the FFT resolution: use the nearest bin.
            int nearestBin = int(std::round(center * SpectrumFftSize / SpectrumSampleRate));
            nearestBin = std::max(1, std::min(SpectrumBins - 1, nearestBin));
            firstBin = nearestBin;
            lastBin = nearestBin;
        }

        m_bandFirstBins[band] = firstBin;
        m_bandLastBins[band] = lastBin;
    }

    m_refreshTimer->timeout_Connect(&VisualizerWindow::updateSpectrum, this);
}

VisualizerWindow::~VisualizerWindow()
{
    m_refreshTimer->stop();
}

void VisualizerWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);

    Painter painter(this);
    painter.clearWindow();

    if (!m_errorMessage.empty()) {
        painter.move(0, lines() / 2);
        painter.squeezedPrint(m_errorMessage, cols());
        painter.flush();
        return;
    }

    const int height = lines();
    if (height <= 0 || cols() <= 0) {
        painter.flush();
        return;
    }

    // Left channel is drawn in the left half of the screen, right channel in
    // the right half. A small gap in the middle separates the two analyzers.
    const int centerGap = cols() >= 2 * BandsNumber + 2 ? 2 : 0;
    const int groupWidth = (cols() - centerGap) / 2;
    if (groupWidth <= 0) {
        painter.flush();
        return;
    }

    const int rightStart = cols() - groupWidth;
    for (int band = 0; band < BandsNumber; ++band) {
        const int xBegin = band * groupWidth / BandsNumber;
        const int xEnd = (band + 1) * groupWidth / BandsNumber;
        const int slotWidth = xEnd - xBegin;
        if (slotWidth <= 0)
            continue;

        const int barWidth = slotWidth >= 3 ? slotWidth - 1 : slotWidth;
        drawBar(painter, xBegin, barWidth, m_leftBarHeights[band], height);
        drawBar(painter, rightStart + xBegin, barWidth, m_rightBarHeights[band], height);
    }

    if (m_showHint) {
        static const char hint1[] = "No visualization data.";
        static const char hint2[] =
            "Add 'effect.order.N = visualization' to the XMMS2 config and restart playback.";

        const int hint1X = std::max(0, (cols() - int(sizeof(hint1) - 1)) / 2);
        const int hint2X = std::max(0, (cols() - int(sizeof(hint2) - 1)) / 2);
        const int hintY = height / 2;

        painter.setColor(palette().color(hasFocus() ? Palette::GroupActive
                                                    : Palette::GroupInactive,
                                         Palette::RoleText));
        painter.move(hint1X, hintY);
        painter.printString(hint1);
        painter.move(hint2X, std::min(hintY + 1, height - 1));
        painter.squeezedPrint(hint2, cols());
    }

    painter.flush();
}

void VisualizerWindow::showEvent()
{
    if (!m_visualizationClient->isStarted())
        initializeVisualization();

    if (m_visualizationClient->isStarted()) {
        std::fill(m_leftHistory.begin(), m_leftHistory.end(), short(0));
        std::fill(m_rightHistory.begin(), m_rightHistory.end(), short(0));
        std::fill(m_leftBarHeights.begin(), m_leftBarHeights.end(), 0.0f);
        std::fill(m_rightBarHeights.begin(), m_rightBarHeights.end(), 0.0f);
        m_idleTicks = 0;
        m_noDataTicks = 0;
        m_gotData = false;
        m_showHint = false;

        m_refreshTimer->startMs(1000 / m_fps);
        updateSpectrum();
    } else {
        update();
    }
}

void VisualizerWindow::initializeVisualization()
{
    m_errorMessage.clear();

    if (!m_visualizationClient->isConnected()
        && !m_visualizationClient->connect(m_ipcPath)) {
        stopWithError(m_visualizationClient->lastError());
        return;
    }

    if (!m_visualizationClient->start()) {
        stopWithError(m_visualizationClient->lastError());
        return;
    }

    m_idleTicks = 0;
}

void VisualizerWindow::updateSpectrum()
{
    if (isHidden()) {
        m_refreshTimer->stop();
        m_visualizationClient->stop();
        return;
    }

    if (!m_visualizationClient->isStarted())
        return;

    // Discard chunks which were queued while the screen was not visible:
    // visualization data is put into a ring buffer, so only the most recent
    // chunks are needed for the FFT window.
    short chunk[PcmSamples];
    bool newData = false;
    for (int i = 0; i < MaxPendingChunks; ++i) {
        const int read = m_visualizationClient->chunkGet(chunk, 0, 0);
        if (read < 0) {
            stopWithError(m_visualizationClient->lastError());
            return;
        }
        if (read == 0)
            break;

        appendPcm(chunk, std::min(read, int(PcmSamples)));
        newData = true;
    }

    if (newData) {
        m_idleTicks = 0;
        m_noDataTicks = 0;
        m_gotData = true;
        m_showHint = false;
        updateBarHeights(true);
        update();
        return;
    }

    ++m_idleTicks;
    ++m_noDataTicks;

    if (!m_gotData && !m_showHint && m_noDataTicks >= 2 * m_fps) {
        m_showHint = true;
        update();
        return;
    }

    // When playback is paused (or stopped), no new chunks are delivered.
    // Let the bars fall down instead of freezing them.
    const int idleTicksLimit = std::max(2, m_fps / 4);
    if (m_idleTicks < idleTicksLimit)
        return;

    const bool haveBars =
           std::any_of(m_leftBarHeights.begin(), m_leftBarHeights.end(),
                       [](float level) {return level > 0.0f;})
        || std::any_of(m_rightBarHeights.begin(), m_rightBarHeights.end(),
                       [](float level) {return level > 0.0f;});
    if (!haveBars)
        return;

    updateBarHeights(false);
    update();
}

void VisualizerWindow::appendPcm(const short *pcm, int samples)
{
    const int frames = samples / 2;
    if (frames <= 0)
        return;

    if (frames >= HistoryFrames) {
        const int offset = frames - HistoryFrames;
        std::copy(pcm + offset, pcm + offset + HistoryFrames, m_leftHistory.begin());
        std::copy(pcm + frames + offset, pcm + frames + offset + HistoryFrames,
                  m_rightHistory.begin());
        return;
    }

    const int keep = HistoryFrames - frames;
    std::memmove(m_leftHistory.data(), m_leftHistory.data() + frames,
                 keep * sizeof(short));
    std::memmove(m_rightHistory.data(), m_rightHistory.data() + frames,
                 keep * sizeof(short));
    std::copy(pcm, pcm + frames, m_leftHistory.begin() + keep);
    std::copy(pcm + frames, pcm + 2 * frames, m_rightHistory.begin() + keep);
}

void VisualizerWindow::updateBarHeights(bool newData)
{
    float leftTargets[BandsNumber] = {0.0f};
    float rightTargets[BandsNumber] = {0.0f};

    if (newData) {
        computeBandLevels(m_leftHistory.data(), leftTargets);
        computeBandLevels(m_rightHistory.data(), rightTargets);
    }

    const float falloff = std::max(0.02f, 2.5f / m_fps);
    for (int band = 0; band < BandsNumber; ++band) {
        if (leftTargets[band] >= m_leftBarHeights[band])
            m_leftBarHeights[band] = leftTargets[band];
        else
            m_leftBarHeights[band] = std::max(leftTargets[band],
                                              m_leftBarHeights[band] - falloff);

        if (rightTargets[band] >= m_rightBarHeights[band])
            m_rightBarHeights[band] = rightTargets[band];
        else
            m_rightBarHeights[band] = std::max(rightTargets[band],
                                               m_rightBarHeights[band] - falloff);
    }
}

void VisualizerWindow::computeBandLevels(const short *samples, float *levels)
{
    float fftReal[SpectrumFftSize];
    float fftImag[SpectrumFftSize];

    for (int i = 0; i < SpectrumFftSize; ++i) {
        fftReal[i] = samples[i] / 32768.0f * m_window[i];
        fftImag[i] = 0.0f;
    }

    fft(fftReal, fftImag, SpectrumFftSize);

    float magnitudes[SpectrumBins];
    for (int bin = 0; bin < SpectrumBins; ++bin) {
        magnitudes[bin] = 2.0f * std::sqrt(fftReal[bin] * fftReal[bin]
                                         + fftImag[bin] * fftImag[bin])
                          / SpectrumFftSize;
    }

    for (int band = 0; band < BandsNumber; ++band) {
        // Band energy: a narrow tone is not lost when the band contains
        // many FFT bins.
        float energy = 0.0f;
        for (int bin = m_bandFirstBins[band]; bin <= m_bandLastBins[band]; ++bin)
            energy += magnitudes[bin] * magnitudes[bin];

        const float amplitude = std::sqrt(energy);
        if (amplitude <= 0.0f) {
            levels[band] = 0.0f;
        } else {
            const float db = 20.0f * std::log10(amplitude);
            levels[band] = std::min(1.0f, std::max(0.0f,
                                                   (db + SpectrumDynamicRangeDb)
                                                   / SpectrumDynamicRangeDb));
        }
    }
}

void VisualizerWindow::drawBar(Painter& painter, int x, int width, float level, int height)
{
    if (width <= 0 || level <= 0.0f)
        return;

    int eighths = int(level * height * 8.0f + 0.5f);
    if (eighths <= 0)
        return;

    int fullCells = eighths / 8;
    int partialCell = eighths % 8;
    if (fullCells >= height) {
        fullCells = height;
        partialCell = 0;
    }

    for (int row = 0; row < fullCells; ++row) {
        painter.setColor(barColor(row, height));
        for (int dx = 0; dx < width; ++dx) {
            painter.move(x + dx, height - 1 - row);
            painter.printChar(FullBlock);
        }
    }

    if (partialCell > 0) {
        painter.setColor(barColor(fullCells, height));
        for (int dx = 0; dx < width; ++dx) {
            painter.move(x + dx, height - 1 - fullCells);
            painter.printChar(PartialBlocks[partialCell]);
        }
    }
}

void VisualizerWindow::stopWithError(const std::string& message)
{
    m_errorMessage = message.empty() ? "visualization is not available" : message;
    m_visualizationClient->disconnect();
    m_refreshTimer->stop();
    std::fill(m_leftBarHeights.begin(), m_leftBarHeights.end(), 0.0f);
    std::fill(m_rightBarHeights.begin(), m_rightBarHeights.end(), 0.0f);
    update();
}

Color VisualizerWindow::barColor(int row, int height) const
{
    const Palette& palette = this->palette();
    const Palette::ColorGroup group = hasFocus() ? Palette::GroupActive
                                                  : Palette::GroupInactive;

    const float position = height > 1 ? float(row) / (height - 1) : 0.0f;
    if (position < 0.5f)
        return palette.color(group, RoleBarLow, ColorGreen);
    if (position < 0.8f)
        return palette.color(group, RoleBarMid, ColorYellow);
    return palette.color(group, RoleBarHigh, ColorRed);
}

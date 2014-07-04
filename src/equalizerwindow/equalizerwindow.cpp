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
#include <iterator>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "equalizerwindow.h"
#include "equalizerbandswindow.h"
#include "equalizerpreampwindow.h"
#include "../xmmsutils/client.h"
#include "../utils.h"

#include "../lib/painter.h"
#include "../lib/rectangle.h"
#include "../lib/checkbox.h"
#include "../lib/radiobuttongroupbox.h"
#include "../lib/keyevent.h"

using namespace ncxmms2;

EqualizerWindow::EqualizerWindow(xmms2::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    Window(rect, parent),
    m_xmmsClient(xmmsClient),
    m_equalizerPluginEnabled(false)
{
    setName("Equalizer");

    const int preampWindowXShift = 19;
    const int bandsNumberGroupBoxLines = 7;
    setMinumumLines(1 + 1 + bandsNumberGroupBoxLines);
    setMinumumCols(preampWindowXShift + 6 + 10);
    
    m_enabledCheckBox = new CheckBox(0, 0, preampWindowXShift, this);
    m_enabledCheckBox->setText("Enabled");
    m_enabledCheckBox->activated_Connect(&EqualizerWindow::setEqualizerEnabled, this);
    m_enabledCheckBox->setFocus();

    m_extraFilteringCheckBox = new CheckBox(0, 1, preampWindowXShift, this);
    m_extraFilteringCheckBox->setText("Extra filtering");
    m_extraFilteringCheckBox->activated_Connect(&EqualizerWindow::setEqualizerExtraFiltering, this);

    m_bandsNumberGroupBox = new RadioButtonGroupBox(Rectangle(0, 2, preampWindowXShift, bandsNumberGroupBoxLines), this);
    m_bandsNumberGroupBox->setGroupName("Bands number");
    m_bandsNumberGroupBox->addRadioButton("10 (legacy)");
    m_bandsNumberGroupBox->addRadioButton("10");
    m_bandsNumberGroupBox->addRadioButton("15");
    m_bandsNumberGroupBox->addRadioButton("25");
    m_bandsNumberGroupBox->addRadioButton("31");
    m_bandsNumberGroupBox->activated_Connect(&EqualizerWindow::setEqualizerBandsNumber, this);
    
    m_preampWindow = new EqualizerPreampWindow(preampWindowXShift, 0, rect.lines(), this);
    m_preampWindow->preampChangeRequested_Connect(&EqualizerWindow::setEqualizerPreamp, this);
    
    Rectangle eqBandsWinRect(preampWindowXShift + m_preampWindow->cols(), 0,
                             rect.cols() - preampWindowXShift - m_preampWindow->cols(), rect.lines());
    m_bandsWindow = new EqualizerBandsWindow(eqBandsWinRect, this);
    m_bandsWindow->bandGainChangeRequested_Connect(&EqualizerWindow::setEqualizerBandGain, this);

    m_xmmsClient->configLoaded_Connect(&EqualizerWindow::loadEqualizerConfig, this);
    m_xmmsClient->configValueChanged_Connect(&EqualizerWindow::handleEqualizerConfigChanged, this);
}

void EqualizerWindow::keyPressedEvent(const KeyEvent& keyEvent)
{
    if (!m_equalizerPluginEnabled)
        return;
    
    Window *const verticalSwithOrder[] =
    {
        m_enabledCheckBox,
        m_extraFilteringCheckBox,
        m_bandsNumberGroupBox
    };
    
    auto vIt = std::find(std::begin(verticalSwithOrder), std::end(verticalSwithOrder),
                        focusedWindow());
    
    Window *const horizontalSwithOrder[] =
    {
        vIt != std::end(verticalSwithOrder) ? *vIt : m_enabledCheckBox,
        m_preampWindow,
        m_bandsWindow
    };
    
    auto hIt = std::find(std::begin(horizontalSwithOrder), std::end(horizontalSwithOrder),
                        focusedWindow());
    assert(hIt != std::end(horizontalSwithOrder));
    
    switch (keyEvent.key()) {
        case KeyEvent::KeyUp:
            if (vIt != std::end(verticalSwithOrder) && vIt != std::begin(verticalSwithOrder)) {
                if (*vIt == m_bandsNumberGroupBox && m_bandsNumberGroupBox->activeRadioButton() > 0)
                    break;
                (*--vIt)->setFocus();
                return;
            }
            break;

        case KeyEvent::KeyDown:
            if (vIt != std::end(verticalSwithOrder) && ++vIt != std::end(verticalSwithOrder)) {
                if (*vIt == m_bandsNumberGroupBox)
                    m_bandsNumberGroupBox->setActiveRadioButton(0);
                (*vIt)->setFocus();
                return;
            } 
            break;

        case KeyEvent::KeyRight:
            if (++hIt != std::end(horizontalSwithOrder)) {
                (*hIt)->setFocus();
                return;
            }
            break;
            
        case KeyEvent::KeyLeft:
            if (*hIt == m_bandsWindow && m_bandsWindow->selectedBand() > 0)
                break;
            if (hIt != std::begin(horizontalSwithOrder)) {
                (*--hIt)->setFocus();
                return;
            }
            break;
            
        default: break;
    }
    Window::keyPressedEvent(keyEvent);
}

void EqualizerWindow::mouseEvent(const MouseEvent& ev)
{
    if (m_equalizerPluginEnabled)
        Window::mouseEvent(ev);
}

void EqualizerWindow::paint(const Rectangle& rect)
{
    NCXMMS2_UNUSED(rect);
    
    Painter painter(this);
    painter.clearWindow();
    if (!m_equalizerPluginEnabled) {
        const char *message = "Equalizer plugin is not enabled in the xmms2 config!";
        painter.move((cols() - std::strlen(message)) / 2, lines() / 2);
        painter.printString(message);
    }
    painter.flush();
}

void EqualizerWindow::resizeChildren(const Size& size)
{
    m_preampWindow->resize(Size(m_preampWindow->cols(), size.lines()));
    m_bandsWindow->resize(Size(size.cols() - (m_preampWindow->x() + m_preampWindow->cols()),
                               size.lines()));
}

void EqualizerWindow::showEvent()
{
    if (!m_xmmsClient->isConfigLoaded())
        m_xmmsClient->configLoad();
    
    if (m_equalizerPluginEnabled)
        Window::showEvent();
}

void EqualizerWindow::loadEqualizerConfig()
{
    std::string effect;
    int i = 0;
    do {
        effect = m_xmmsClient->configValue(Utils::format("effect.order.%d", i));
        if (effect == "equalizer") {
            m_equalizerPluginEnabled = true;
            break;
        }
    } while (!effect.empty());
    
    const std::string optionsToLoad[] = {"equalizer.enabled",
                                         "equalizer.extra_filtering",
                                         "equalizer.preamp",
                                         "equalizer.use_legacy"};
    
    for (const auto& option : optionsToLoad) {
        handleEqualizerConfigChanged(option, m_xmmsClient->configValue(option));
    }
    
    if (!isHidden()) {
        update();
        showEvent();
    }
}

void EqualizerWindow::handleEqualizerConfigChanged(const std::string& key, const std::string& value)
{
    if (!boost::starts_with(key, "equalizer."))
        return;
    
    const std::string eqKey = key.substr(std::strlen("equalizer."));
    if (eqKey == "enabled") {
        m_enabledCheckBox->setChecked(value == "1");
    } else if (eqKey == "extra_filtering") {
        m_extraFilteringCheckBox->setChecked(value == "1");
    } else if (eqKey == "preamp") {
        m_preampWindow->setPreamp(boost::lexical_cast<double>(value));
    } else if (boost::starts_with(eqKey, "legacy")) {
        m_bandsWindow->setBandGain(boost::lexical_cast<int>(eqKey.back()),
                                   boost::lexical_cast<double>(value));
    } else if (boost::starts_with(eqKey, "gain")) {
        if (m_bandsWindow->legacyModeEnabled())
            return;
        const int band = boost::lexical_cast<int>(eqKey.substr(std::strlen("gain")));
        if (band < m_bandsWindow->bandsNumber()) {
            m_bandsWindow->setBandGain(band, boost::lexical_cast<double>(value));
        }
    } else if (eqKey == "use_legacy" || eqKey == "bands") {
        const bool useLegacy = m_xmmsClient->configValue("equalizer.use_legacy") == "1";
        if (useLegacy) {
             m_bandsNumberGroupBox->setCheckedRadioButton(BandsNumberGroupBoxLegacy);
        } else {
            const std::string bandsNumberStr = m_xmmsClient->configValue("equalizer.bands");
            for (int i = BandsNumberGroupBox10; i < BandsNumberGroupBoxItemsCount; ++i) {
                if (m_bandsNumberGroupBox->radioButtonText(i) == bandsNumberStr) {
                    m_bandsNumberGroupBox->setCheckedRadioButton(i);
                    break;
                }
            }
        }
        
        const int bandsNumber = !useLegacy 
                                ? boost::lexical_cast<int>(m_bandsNumberGroupBox->checkedRadioButtonText())
                                : EqualizerBandsWindow::LegacyBandsNumber;
        m_bandsWindow->setBandsNumber(bandsNumber);
        if (useLegacy)
            m_bandsWindow->setLegacyModeEnabled();
        const char *fmt= useLegacy ? "equalizer.legacy%d" : "equalizer.gain%02d";
        for (int i = 0; i < bandsNumber; ++i) {
            std::string band = Utils::format(fmt, i);
            std::string gain = m_xmmsClient->configValue(band);
            m_bandsWindow->setBandGain(i, boost::lexical_cast<double>(gain));
            
            //FIXME: Workaround of xmms2 bug 2581
            m_xmmsClient->configSetValue(band, "0");
            m_xmmsClient->configSetValue(band, gain);
        }
    }
}

void EqualizerWindow::setEqualizerEnabled(bool enable)
{
    m_xmmsClient->configSetValue("equalizer.enabled", enable ? "1" : "0");
}

void EqualizerWindow::setEqualizerExtraFiltering(bool enable)
{
    m_xmmsClient->configSetValue("equalizer.extra_filtering", enable ? "1" : "0");
}

void EqualizerWindow::setEqualizerBandsNumber(int index)
{
    if (index == BandsNumberGroupBoxLegacy) {
        m_xmmsClient->configSetValue("equalizer.use_legacy", "1");
    } else {
        m_xmmsClient->configSetValue("equalizer.use_legacy", "0");
        m_xmmsClient->configSetValue("equalizer.bands", m_bandsNumberGroupBox->radioButtonText(index));
    }
}

void EqualizerWindow::setEqualizerPreamp(int preamp)
{
    m_xmmsClient->configSetValue("equalizer.preamp", boost::lexical_cast<std::string>(preamp));
}

void EqualizerWindow::setEqualizerBandGain(int band, int gain)
{
    const char *keyFmt = m_bandsWindow->legacyModeEnabled() ? "equalizer.legacy%d" : "equalizer.gain%02d";
    m_xmmsClient->configSetValue(Utils::format(keyFmt, band), boost::lexical_cast<std::string>(gain));
}

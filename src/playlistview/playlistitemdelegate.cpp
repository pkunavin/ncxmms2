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

#include <stdexcept>

#include "playlistitemdelegate.h"
#include "playlistmodel.h"

#include "../lib/painter.h"
#include "../lib/listitempaintoptions.h"
#include "../lib/application.h"

using namespace ncxmms2;

PlaylistItemDelegate::PlaylistItemDelegate(const PlaylistModel *model) :
    ListModelItemDelegate(model)
{

}

void PlaylistItemDelegate::setDisplayFormat(const std::string& format)
{
    if (!m_songDisplayFormatter.setDisplayFormat(format)) {
        throw std::runtime_error(
            std::string("Parsing format string failed: ").append(m_songDisplayFormatter.errorString())
        );
    }
}

void PlaylistItemDelegate::paint(Painter *painter, const ListItemPaintOptions& options, int item)
{
    const Palette *palette = options.palette;
    const Palette::ColorGroup colorGroup = options.hasFocus
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;

    bool displayFormatterIgnoreColors = false;

    if (options.state & ListItemStateCurrent) {
        if (Application::useColors()) {
            painter->setColorPair(palette->color(colorGroup, Palette::RoleHighlightedText),
                                  palette->color(colorGroup, Palette::RoleHighlight));
            painter->clearLine(options.rect.y());
            displayFormatterIgnoreColors = true;
        } else {
            painter->fillLine(options.rect.y(), ColorBlack);
            painter->setColor(ColorBlack);
            painter->setReverse(true);
        }
    } else if (options.state & ListItemStateSelected)  {
        painter->setColor(palette->color(colorGroup, Palette::RoleSelection));
        painter->clearLine(options.rect.y());
        painter->setBold(true);
        displayFormatterIgnoreColors = true;
    } else {
        painter->clearLine(options.rect.y());
    }

    const PlaylistModel *plsModel = static_cast<const PlaylistModel*>(model());
    const Song& song = plsModel->song(item);

    if (song.id() > 0) {
        if (item == plsModel->currentSongItem() && !(options.state & ListItemStateCurrent))
            painter->setBold(true);

        m_songDisplayFormatter.paint(song, painter, options.rect, displayFormatterIgnoreColors);
    } else {
        painter->setColor(palette->color(colorGroup, Palette::RoleText));
        painter->printString("Loading...");
    }
}

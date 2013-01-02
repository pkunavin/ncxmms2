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

void PlaylistItemDelegate::paint(Painter *painter, const ListItemPaintOptions& options, int item)
{
    const Palette *palette = options.palette;
    const Palette::ColorGroup colorGroup = options.hasFocus
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;

    Color artistColor   = ColorBlack;
    Color titleColor    = ColorBlack;
    Color durationColor = ColorBlack;
    Color textColor     = ColorBlack;
    if (options.state & ListItemStateCurrent) {
        if (Application::useColors()) {
            artistColor   =
            titleColor    =
            durationColor =
            textColor     = palette->color(colorGroup, Palette::RoleHighlightedText);
            painter->setColorPair(palette->color(colorGroup, Palette::RoleHighlightedText),
                                  palette->color(colorGroup, Palette::RoleHighlight));
            painter->clearLine(options.rect.y());
        } else {
            painter->fillLine(options.rect.y(), ColorBlack);
            painter->setColor(ColorBlack);
            painter->setReverse(true);
        }
    } else if (options.state & ListItemStateSelected)  {
        artistColor   =
        titleColor    =
        durationColor =
        textColor     = palette->color(colorGroup, Palette::RoleSelection);
        painter->clearLine(options.rect.y());
        painter->setBold(true);
    } else {
        artistColor   = palette->color(colorGroup, RoleArtist,        ColorYellow);
        titleColor    = palette->color(colorGroup, RoleTitle,         ColorYellow);
        durationColor = palette->color(colorGroup, RoleDuration,      ColorGreen);
        textColor     = palette->color(colorGroup, Palette::RoleText, ColorYellow);
        painter->clearLine(options.rect.y());
    }

    const PlaylistModel *plsModel = static_cast<const PlaylistModel*>(model());
    const Song& song = plsModel->song(item);

    if (song.id()) {
        if (item == plsModel->currentSongItem() && !(options.state & ListItemStateCurrent))
            painter->setBold(true);

        int durationStringSize = 0;
        if (song.duration()) {
            durationStringSize = 1 + song.durationString().size() + 1;
            painter->move(options.rect.cols() - durationStringSize, options.rect.y());
            painter->setColor(durationColor);
            painter->printChar('(');
            painter->printString(song.durationString());
            painter->printChar(')');
        }
        int sizeLeft = options.rect.cols() - durationStringSize - 1;
        painter->move(0, options.rect.y());
        if (!song.artist().empty()) {
            painter->setColor(artistColor);
            painter->squeezedPrint(song.artist(), sizeLeft);
            if (painter->x() < sizeLeft - 6) {
                 sizeLeft -= painter->x();
                 painter->setColor(textColor);
                 painter->printString(" - ");
                 sizeLeft -= 3;
                 painter->setColor(titleColor);
                 painter->squeezedPrint(song.title(), sizeLeft);
            }
        } else {
            painter->setColor(titleColor);
            painter->squeezedPrint(song.title(), sizeLeft);
        }
    } else {
        painter->setColor(palette->color(colorGroup, Palette::RoleText));
        painter->printString("Loading...");
    }
}

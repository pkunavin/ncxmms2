/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
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

#include "filesystemitemdelegate.h"
#include "filesystemmodel.h"

#include "../lib/painter.h"
#include "../lib/palette.h"
#include "../lib/listitempaintoptions.h"
#include "../lib/application.h"

using namespace ncxmms2;

FileSystemItemDelegate::FileSystemItemDelegate(const FileSystemModel *fsModel) :
    ListModelItemDelegate(fsModel)
{
}

void FileSystemItemDelegate::paint(Painter *painter, const ListItemPaintOptions &options, int item)
{
    const Palette *palette = options.palette;
    const Palette::ColorGroup colorGroup = options.hasFocus
                                           ? Palette::GroupActive
                                           : Palette::GroupInactive;

    if (options.state == ListItemStateCurrent) {
        if (Application::useColors()) {
            painter->setColorPair(palette->color(colorGroup, Palette::RoleSelectedText),
                                  palette->color(colorGroup, Palette::RoleSelection));
            painter->clearLine(options.rect.y());
        } else {
            painter->fillLine(options.rect.y(), ColorBlack);
            painter->setColor(ColorBlack);
            painter->setReverse(true);
        }
    } else {
        painter->setColor(palette->color(colorGroup, Palette::RoleText));
        painter->clearLine(options.rect.y());
    }

    const FileSystemModel *fsModel = static_cast<const FileSystemModel*>(model());
    if (fsModel->isDirectory(item) && options.state != ListItemStateCurrent) {
        painter->setBold(true);
    }

    if (fsModel->isDirectory(item)) {
        painter->printChar('/');
        painter->squeezedPrint(fsModel->fileName(item), options.rect.cols() - 1);
    } else {
        painter->squeezedPrint(fsModel->fileName(item), options.rect.cols());
    }
}


#include <boost/format.hpp>

#include "playlistitemdelegate.h"
#include "playlistmodel.h"

#include "../lib/painter.h"
#include "../lib/listitempaintoptions.h"

using namespace ncxmms2;

PlaylistItemDelegate::PlaylistItemDelegate(const PlaylistModel *model) :
    ListModelItemDelegate(model)
{
}

void PlaylistItemDelegate::paint(Painter *painter, const ListItemPaintOptions& options, int item)
{
    if (options.state == ListItemStateCurrent) {
            painter->fillLine(options.rect.y(), ColorYellow);
            painter->setReverse(true);
        } else {
            painter->clearLine(options.rect.y());
        }

        const PlaylistModel *plsModel = static_cast<const PlaylistModel*>(model());
        const Song& song = plsModel->song(item);

        if (song.id()) {
            if (item == plsModel->currentSongItem() && options.state != ListItemStateCurrent)
                painter->setBold(true);

            int durationStringSize = 0;
            if (song.duration()) {
                durationStringSize = 1 + song.durationString().size() + 1;
                painter->move(options.rect.cols() - durationStringSize, options.rect.y());
                painter->setColor(ColorGreen);
                painter->printString((boost::format("(%1%)") % song.durationString()).str());
            }

            painter->move(0, options.rect.y());
            painter->setColor(ColorYellow);
            if (!song.artist().empty()) {
                painter->squeezedPrint((boost::format("%1% - %2%") % song.artist() % song.title()).str(),
                                      options.rect.cols() - durationStringSize - 1);
            } else {
                painter->squeezedPrint(song.title(), options.rect.cols() - durationStringSize - 1);
            }
        } else {
            painter->setColor(ColorYellow);
            painter->printString("Loading...");
        }
}

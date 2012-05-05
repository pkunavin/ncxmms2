#ifndef PLAYLISTITEMDELEGATE_H
#define PLAYLISTITEMDELEGATE_H

#include "../lib/listmodelitemdelegate.h"

namespace ncxmms2 {

class PlaylistModel;

class PlaylistItemDelegate : public ListModelItemDelegate
{
public:
    PlaylistItemDelegate(const PlaylistModel *model);

    virtual void paint(Painter *painter, const ListItemPaintOptions& options, int item);
};
}

#endif // PLAYLISTITEMDELEGATE_H

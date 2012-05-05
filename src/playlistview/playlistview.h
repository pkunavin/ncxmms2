#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <xmmsclient/xmmsclient++.h>

#include "../lib/listview.h"

namespace ncxmms2 {

class PlaylistModel;

class PlaylistView : public ListView
{
public:
    PlaylistView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent = nullptr);

    void setPlaylist(const std::string& playlist);

    virtual void keyPressedEvent(const KeyEvent& keyEvent);

private:
    Xmms::Client *m_xmmsClient;
    PlaylistModel *m_model;
    std::string m_activePlaylist;
    Xmms::Playback::Status m_playbackStatus;


    bool getActivePlaylist(const std::string& playlist);
    bool getPlaybackStatus(const Xmms::Playback::Status& status);
    void onItemEntered(int item);

};

} // ncxmms2

#endif // PLAYLISTVIEW_H

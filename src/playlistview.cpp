#include "playlistview.h"
#include "playlistmodel.h"
#include "playlistitemdelegate.h"

#include "lib/keyevent.h"

using namespace ncxmms2;

PlaylistView::PlaylistView(Xmms::Client *xmmsClient, const Rectangle& rect, Window *parent) :
    ListView(rect, parent),
    m_xmmsClient(xmmsClient),
    m_playbackStatus(Xmms::Playback::STOPPED)
{
    m_model = new PlaylistModel(m_xmmsClient, this);
    setModel(m_model);
    setItemDelegate(new PlaylistItemDelegate(m_model));
    setHideCurrentItemInterval(10);

    itemEntered_Connect(&PlaylistView::onItemEntered, this);

    m_xmmsClient->playlist.currentActive()(Xmms::bind(&PlaylistView::getActivePlaylist, this));
    m_xmmsClient->playlist.broadcastLoaded()(Xmms::bind(&PlaylistView::getActivePlaylist, this));

    m_xmmsClient->playback.getStatus()(Xmms::bind(&PlaylistView::getPlaybackStatus, this));
    m_xmmsClient->playback.broadcastStatus()(Xmms::bind(&PlaylistView::getPlaybackStatus, this));
}
    \
void PlaylistView::setPlaylist(const std::string& playlist)
{
    m_model->setPlaylist(playlist);
}

void PlaylistView::keyPressedEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.key()) {
        case KeyEvent::KeyDelete:
            if (m_model->itemsCount() && !isCurrentItemHidden()) {
                m_xmmsClient->playlist.removeEntry(currentItem(), m_model->playlist());
                showCurrentItem();
            }
            break;

        case 'c':
            m_xmmsClient->playlist.clear(m_model->playlist());
            break;

        case 'o':
            setCurrentItem(m_model->currentSongItem());
            break;

        default: ListView::keyPressedEvent(keyEvent);
    }
}

bool PlaylistView::getActivePlaylist(const std::string& playlist)
{
    m_activePlaylist = playlist;
    return true;
}

bool PlaylistView::getPlaybackStatus(const Xmms::Playback::Status& status)
{
    m_playbackStatus = status;
    return true;
}

void PlaylistView::onItemEntered(int item)
{
    if (m_model->playlist() != m_activePlaylist)
            m_xmmsClient->playlist.load(m_model->playlist());

        m_xmmsClient->playlist.setNext(item);
        m_xmmsClient->playback.tickle();
        switch (m_playbackStatus) {
            case Xmms::Playback::STOPPED:
                m_xmmsClient->playback.start();
                break;
            case Xmms::Playback::PLAYING:
                m_xmmsClient->playback.tickle();
                break;
            case Xmms::Playback::PAUSED:
                m_xmmsClient->playback.start();
                m_xmmsClient->playback.tickle();
        }
}

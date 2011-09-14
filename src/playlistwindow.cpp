/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011 Pavel Kunavin <tusk.kun@gmail.com>
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

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "playlistwindow.h"
#include "utils.h"

#include "lib/painter.h"
#include "lib/colors.h"

using namespace ncxmms2;

PlaylistWindow::PlaylistWindow(Xmms::Client *xmmsClient, int lines, int cols, int yPos, int xPos, Window *parent) :
	AbstractItemView(lines, cols, yPos, xPos, parent),
	m_xmmsClient(xmmsClient),
	m_currentPosition(-1),
	m_playbackStatus(Xmms::Playback::STOPPED),
	m_totalDuration(0)
{
	m_xmmsClient->playlist.currentActive()(Xmms::bind(&PlaylistWindow::getActivePlaylist, this));
	m_xmmsClient->playlist.broadcastLoaded()(Xmms::bind(&PlaylistWindow::getActivePlaylist, this));
	
	m_xmmsClient->playlist.broadcastChanged()(Xmms::bind(&PlaylistWindow::processPlaylistChange, this));
	m_xmmsClient->playlist.broadcastCurrentPos()(Xmms::bind(&PlaylistWindow::getCurrentPosition, this));
	m_xmmsClient->collection.broadcastCollectionChanged()(Xmms::bind(&PlaylistWindow::handlePlaylistRename, this));
	m_xmmsClient->medialib.broadcastEntryChanged()(Xmms::bind(&PlaylistWindow::handleSongInfoUpdate, this));
	
	m_xmmsClient->playback.getStatus()(Xmms::bind(&PlaylistWindow::getPlaybackStatus, this));
	m_xmmsClient->playback.broadcastStatus()(Xmms::bind(&PlaylistWindow::getPlaybackStatus, this));
	
	setTitle("Playlist");
	setHideCurrentItemInterval(10);
}

void PlaylistWindow::updateWindowTitle()
{
	std::string titleString("Playlist: ");
	titleString.append(m_activePlaylist);
	
	if (m_idList.size()) {
		titleString.append(" (");
		titleString.append(boost::lexical_cast<std::string>(m_idList.size()));
		titleString.append(" tracks");
		if (m_totalDuration>0) {
			titleString.append(", ");
			titleString.append(Utils::getTimeStringFromInt(m_totalDuration));
			titleString.append(" total playtime");
		}
		titleString.push_back(')');
	}
	setTitle(titleString);
}

bool PlaylistWindow::getActivePlaylist(const std::string& playlist)
{
	m_activePlaylist=playlist;
	m_xmmsClient->playlist.listEntries(m_activePlaylist)(Xmms::bind(&PlaylistWindow::getEntries, this));
	m_xmmsClient->playlist.currentPos(m_activePlaylist)(Xmms::bind(&PlaylistWindow::getCurrentPosition, this));
	updateWindowTitle();
	return true;
}

bool PlaylistWindow::getEntries(const Xmms::List<int>& list)
{
	m_totalDuration=0;
	m_idList.clear();
	m_songInfos.clear();
	m_idList.reserve(200);
	m_songInfos.rehash(200/m_songInfos.max_load_factor()+1);
	
	int pos=0;
	for(auto i(list.begin()), i_end(list.end()); i != i_end; ++i,++pos) {
		const int id=*i;
		m_idList.push_back(id);
		m_songInfos[id];
		m_xmmsClient->medialib.getInfo(id)(boost::bind(&PlaylistWindow::getSongInfo, this, pos, _1));
	}
	reset();
	updateWindowTitle();
	return true;
}

bool PlaylistWindow::getSongInfo(int position, const Xmms::PropDict& info)
{
	const int id=info.get<int>("id");
	auto it=m_songInfos.find(id);
	if (it==m_songInfos.end())
		return true;
	
	Song *song=&(*it).second;
	int durationDiff=-song->duration();
	song->loadInfo(info);
		
	if (position==-1 || (std::vector<int>::size_type)position>=m_idList.size() || m_idList[position]!=id) {
		redrawAll();
	} else {
		redrawItem(position);
	}
	
	durationDiff+=song->duration();
	if (durationDiff) {
		m_totalDuration+=durationDiff;
		updateWindowTitle();
	}
	
	return true;
}

bool PlaylistWindow::processPlaylistChange(const Xmms::Dict& change)
{
	if (change.get<std::string>("name")!=m_activePlaylist)
		return true;
	
	switch (change.get<int>("type")) {
		case XMMS_PLAYLIST_CHANGED_ADD:
		{
			const int id=change.get<int>("id");
			
			m_idList.push_back(id);
			m_songInfos[id];
			m_xmmsClient->medialib.getInfo(id)(boost::bind(&PlaylistWindow::getSongInfo, this, m_idList.size()-1, _1));
			itemAdded();
			break;
		}
	
		case XMMS_PLAYLIST_CHANGED_INSERT:
		{
			const int id=change.get<int>("id");
			const int position=change.get<int>("position");
			
			m_idList.insert(m_idList.begin()+position, id);
			m_songInfos[id];
			m_xmmsClient->medialib.getInfo(id)(boost::bind(&PlaylistWindow::getSongInfo, this, position, _1));
			itemInserted(position);			
			break;
		}
		
		case XMMS_PLAYLIST_CHANGED_REMOVE:
		{
			const int position=change.get<int>("position");
			const int id=m_idList[position];
			
			m_idList.erase(m_idList.begin()+position);
			m_totalDuration-=m_songInfos[id].duration();
			m_songInfos.erase(id);
			
			updateWindowTitle();
			itemRemoved(position);
			break;
		}
		
		case XMMS_PLAYLIST_CHANGED_MOVE:
		{
			const int position=change.get<int>("position");
			const int newPosition=change.get<int>("newposition");
			
			const int id=m_idList[position];
			m_idList.erase(m_idList.begin()+position);
			m_idList.insert(m_idList.begin()+newPosition, id);
			itemMoved(position, newPosition);
			break;
		}
		
		case XMMS_PLAYLIST_CHANGED_CLEAR:
			m_currentPosition=-1;
			m_totalDuration=0;
			m_idList.clear();
			m_songInfos.clear();
			updateWindowTitle();
			reset();
			break;
			
		case XMMS_PLAYLIST_CHANGED_SHUFFLE:
		case XMMS_PLAYLIST_CHANGED_SORT:
		case XMMS_PLAYLIST_CHANGED_UPDATE:
			m_xmmsClient->playlist.listEntries(m_activePlaylist)(Xmms::bind(&PlaylistWindow::getEntries, this));
			break;
	}
	
	return true;
}

bool PlaylistWindow::getCurrentPosition(const Xmms::Dict& position)
{	
	if (!position.contains("name") || position.get<std::string>("name")!=m_activePlaylist)
		return true;
	
	if (position.contains("position")) {
		const int oldPosition=m_currentPosition;
		m_currentPosition=position.get<int>("position");
		redrawItem(oldPosition);
		redrawItem(m_currentPosition);
	}
	return true;
}

bool PlaylistWindow::handlePlaylistRename(const Xmms::Dict& change)
{
	if (change.get<int>("type")==XMMS_COLLECTION_CHANGED_RENAME
		&& change.get<std::string>("namespace")=="Playlists"
		&& change.get<std::string>("name")==m_activePlaylist) {
			m_activePlaylist=change.get<std::string>("newname");
	}
		
	return true;
}

bool PlaylistWindow::handleSongInfoUpdate(const int& id)
{
	if (m_songInfos.find(id)!=m_songInfos.end())
		m_xmmsClient->medialib.getInfo(id)(boost::bind(&PlaylistWindow::getSongInfo, this, -1, _1));
	
	return true;
}

bool PlaylistWindow::getPlaybackStatus(const Xmms::Playback::Status& status)
{
	m_playbackStatus=status;
	return true;
}

void PlaylistWindow::drawItem(int item)
{
	Painter painter(this);

	if (item==currentItem() && !isCurrentItemHidden()) {
		painter.fillLine(itemLine(item), ColorYellow);
		painter.setReverse(true);
	} else {
		painter.clearLine(itemLine(item));
	}
		
	Song *song=0;
	const int id=m_idList[item];
	auto it=m_songInfos.find(id);
	if (it==m_songInfos.end()) {
		song=&m_songInfos[id];
		m_xmmsClient->medialib.getInfo(id)(boost::bind(&PlaylistWindow::getSongInfo, this, item, _1));
	} else {
		song=&(*it).second;
	}

	if (song->id()) {
		if (item==m_currentPosition && (item!=currentItem() || isCurrentItemHidden()))
			painter.setBold(true);
		
		int durationStringSize=0;
		if (song->duration()) {
			durationStringSize=1+song->durationString().size()+1;
			painter.move(cols()-durationStringSize, itemLine(item));
			painter.setColor(ColorGreen);
			painter.printString((boost::format("(%1%)") % song->durationString()).str());
		}
		
		painter.move(0, itemLine(item));
		painter.setColor(ColorYellow);
		if (!song->artist().empty()) {
			painter.squeezedPrint((boost::format("%1% - %2%") % song->artist() % song->title()).str(),
								  cols()-durationStringSize-1);
		} else {
			painter.squeezedPrint(song->title(), cols()-durationStringSize-1);
		}
	} else {
		painter.setColor(ColorYellow);
		painter.printString("Loading...");
	}
}

int PlaylistWindow::itemsCount() const
{
	return m_idList.size();
}

void PlaylistWindow::keyPressedEvent(const KeyEvent& keyEvent)
{	
	if (keyEvent.key()==KeyEvent::KeyDelete) {
		if (m_idList.size() && !isCurrentItemHidden()) {
			m_xmmsClient->playlist.removeEntry(currentItem());
			showCurrentItem();
		}
	} else {
		AbstractItemView::keyPressedEvent(keyEvent);
	}
}

void PlaylistWindow::itemEntered(int item)
{
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

PlaylistWindow::~PlaylistWindow()
{

}


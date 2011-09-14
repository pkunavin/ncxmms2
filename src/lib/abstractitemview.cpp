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

#include <boost/bind.hpp>
#include "abstractitemview.h"
#include "painter.h"

using namespace ncxmms2;

AbstractItemView::AbstractItemView(int lines, int cols, int yPos, int xPos, Window *parent) :
	Window(lines, cols, yPos, xPos, parent),
	m_currentItem(-1),
	m_viewportFirstItem(-1),
	m_viewportLastItem(-1),
	m_currentItemHidden(false),
	m_hideCurrentItemSelectionInterval(0)
{
	m_hideSelectionTimer.connectTimeoutSignal(boost::bind(&AbstractItemView::hideCurrentItemSelection, this));
}

void AbstractItemView::reset()
{
	const int itemsNum=itemsCount();
	
	if (itemsNum>0) {
		m_currentItem=0;
		m_viewportFirstItem=0;
		m_viewportLastItem=itemsNum>lines() ? lines() : itemsNum;
		if (m_hideCurrentItemSelectionInterval)
			m_hideSelectionTimer.start(m_hideCurrentItemSelectionInterval);
	} else {
		m_currentItem=-1;
		m_viewportFirstItem=-1;
		m_viewportLastItem=-1;
	}
		
	update();
}

void AbstractItemView::redrawItem(int item)
{
	if (isHidden())
		return;

	if (item>=m_viewportFirstItem && item<m_viewportLastItem)
		drawItem(item);
	Painter(this).flush();
}

void AbstractItemView::redrawItems(int first, int last)
{
	if (isHidden())
		return;

	const int firstItem=std::max(first, m_viewportFirstItem);
	const int lastItem=std::min(last, m_viewportLastItem);
	
	for (int item=firstItem;item<lastItem;++item) {
		drawItem(item);
	}
	Painter(this).flush();
}

void AbstractItemView::redrawAll()
{
	if (isHidden())
		return;

	for (int i=m_viewportFirstItem;i<m_viewportLastItem;++i) {
		drawItem(i);
	}

	Painter painter(this);
	for (int i=m_viewportLastItem;i<lines();++i) {
		painter.clearLine(i);
	}

	painter.flush();
}

void AbstractItemView::itemAdded()
{
	if (m_currentItem==-1)
		reset();
	
	const int itemsNum=itemsCount();
	if (lines()>=itemsNum) {
		m_viewportLastItem=itemsNum;
		redrawItem(itemsNum-1);
	}
}

void AbstractItemView::itemInserted(int item)
{
	if (m_currentItem==-1)
		reset();
	
	const int itemsNum=itemsCount();
	if (lines()>=itemsNum) 
		m_viewportLastItem=itemsNum;
	
	redrawItems(item, itemsNum);		
}

void AbstractItemView::itemRemoved(int item)
{
	const int itemsNum=itemsCount();
	
	if (m_currentItem>item || m_currentItem==itemsNum)
		--m_currentItem;
	
	if (lines()>itemsNum) {
		m_viewportLastItem=itemsNum;
		update();
	} else {
		if (item<m_viewportFirstItem) {
			--m_viewportFirstItem;
			--m_viewportLastItem;
		} else if (item<m_viewportLastItem) {
			if (itemsNum>=m_viewportLastItem) {
				redrawItems(item, m_viewportLastItem);
			} else {
				--m_viewportFirstItem;
				--m_viewportLastItem;
				redrawAll();
			}
		}
	}
}

void AbstractItemView::itemMoved(int from, int to)
{
	redrawItems(std::min(from, to), std::max(from, to)+1);
}

void AbstractItemView::itemEntered(int item)
{

}

int AbstractItemView::currentItem() const
{
	return m_currentItem;
}

void AbstractItemView::setCurrentItem(int item)
{
	if (item<0 || item>=itemsCount())
		return;
	
	if (!(item>=m_viewportFirstItem && item<m_viewportLastItem))
		setViewportFirstItem(item);
	
	m_currentItem=item;
	redrawAll();
}

bool AbstractItemView::isCurrentItemHidden() const
{
	return m_currentItemHidden;
}

void AbstractItemView::setHideCurrentItemInterval(unsigned int sec)
{
	m_hideCurrentItemSelectionInterval=sec;
	if (m_hideCurrentItemSelectionInterval) {
		m_hideSelectionTimer.start(m_hideCurrentItemSelectionInterval);
	} else {
		m_hideSelectionTimer.stop();
		m_currentItemHidden=false;
		redrawItem(m_currentItem);
	}
}

void AbstractItemView::showCurrentItem()
{
	m_currentItemHidden=false;
	redrawItem(m_currentItem);
	if (m_hideCurrentItemSelectionInterval)
		m_hideSelectionTimer.start(m_hideCurrentItemSelectionInterval);
}

int AbstractItemView::itemLine(int item) const
{
	return item-m_viewportFirstItem;
}

int AbstractItemView::viewportFirstItem() const
{
	return m_viewportFirstItem;
}

void AbstractItemView::setViewportFirstItem(int item)
{
	if (itemsCount()<=lines())
		return;
	
	if (item+lines()<itemsCount()) {
		m_viewportFirstItem=item;
		m_currentItem=item;
		m_viewportLastItem=m_viewportFirstItem+lines();
	} else {
		m_viewportLastItem=itemsCount();
		m_viewportFirstItem=m_viewportLastItem-lines();
		m_currentItem=m_viewportFirstItem;
	}
	redrawAll();
}

void AbstractItemView::showEvent()
{
	redrawAll();
}

void AbstractItemView::keyPressedEvent(const KeyEvent& keyEvent)
{
	switch (keyEvent.key()) {
		case KeyEvent::KeyUp:
			if (m_currentItemHidden) {
				m_currentItemHidden=false;
				redrawItem(m_currentItem);
			} else {
				scrollUp();
			}
			if (m_hideCurrentItemSelectionInterval)
				m_hideSelectionTimer.start(m_hideCurrentItemSelectionInterval);
			break;
			
		case KeyEvent::KeyDown:
			if (m_currentItemHidden) {
				m_currentItemHidden=false;
				redrawItem(m_currentItem);
			} else {
				scrollDown();
			}
			if (m_hideCurrentItemSelectionInterval)
				m_hideSelectionTimer.start(m_hideCurrentItemSelectionInterval);
			break;
			
		case KeyEvent::KeyEnter:
			if (m_currentItem!=-1 && !m_currentItemHidden)
				itemEntered(m_currentItem);
			break;
			
		default:
			break;
	}
}

void AbstractItemView::scrollUp()
{
	if (m_currentItem==-1)
		return;

	if (m_currentItem>0) {
		drawItem(m_currentItem--);
		if (m_currentItem<m_viewportFirstItem) {
			--m_viewportFirstItem;
			--m_viewportLastItem;
			redrawAll();
		} else {
			redrawItem(m_currentItem);
		}
	}
}

void AbstractItemView::scrollDown()
{
	if (m_currentItem==-1)
		return;

	if (m_currentItem<itemsCount()-1) {
		drawItem(m_currentItem++);
		if (m_currentItem>=m_viewportLastItem) {
			++m_viewportFirstItem;
			++m_viewportLastItem;
			redrawAll();
		} else {
			redrawItem(m_currentItem);
		}
	}
}

void AbstractItemView::resizeEvent(const Size& size)
{
	if (size.lines()>lines()) {
		int extraSize=size.lines()-lines();
		
		if (itemsCount()>m_viewportLastItem) {
			const int min=std::min(itemsCount()-m_viewportLastItem, extraSize);
			m_viewportLastItem+=min;
			extraSize-=min;
		}
		
		if (extraSize>0 && m_viewportFirstItem>0) {
			const int min=std::min(m_viewportFirstItem, extraSize);
			m_viewportFirstItem-=min;
			extraSize-=min;
		}
	}
	
	if (size.lines()<lines()) {
		int sizeDiff=lines()-size.lines();
		
		if (lines()>itemsCount())
			sizeDiff-=std::min(lines()-itemsCount(), sizeDiff);
		
		if (sizeDiff>0 && m_currentItem<m_viewportLastItem-1) {
			const int min=std::min(m_viewportLastItem-1-m_currentItem, sizeDiff);
			m_viewportLastItem-=min;
			sizeDiff-=min;
		}
		
		if (sizeDiff>0)
			m_viewportFirstItem+=sizeDiff;
	}
	
	Window::resizeEvent(size);
}

void AbstractItemView::hideCurrentItemSelection()
{
	m_hideSelectionTimer.stop();
	m_currentItemHidden=true;
	redrawItem(m_currentItem);
}

AbstractItemView::~AbstractItemView() 
{

}


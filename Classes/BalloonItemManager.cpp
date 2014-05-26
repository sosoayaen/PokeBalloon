/*
 * =====================================================================================
 *
 *       Filename:  BalloonItemManager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2014 23:00:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonItemManager.h"

USING_NS_CC;

BalloonItemManager::BalloonItemManager():
	m_bInited(false),
	m_pLayerItemContainer(NULL),
	m_pDelegate(NULL)
{

}

BalloonItemManager::~BalloonItemManager()
{

}

bool BalloonItemManager::init(BalloonItemDelegate* pDelegate, cocos2d::CCLayer* pLayerItemContainer)
{
	if (!m_bInited)
	{
		m_pDelegate = pDelegate;
		m_pLayerItemContainer = pLayerItemContainer;

		m_bInited = true;
	}

	return false;
}

void BalloonItemManager::updatePosition(float dt, unsigned long ulFrame)
{
	if (!m_bInited || !m_pLayerItemContainer) return;
	// 循环调用
	CCObject* pObj = NULL;
	CCARRAY_FOREACH(m_pLayerItemContainer->getChildren(), pObj)
	{
		BalloonItem* pBalloonItem = dynamic_cast<BalloonItem*>(pObj);
		if (pBalloonItem)
		{
			// 先更新各自的状态
			pBalloonItem->updateItemStatus(dt, ulFrame);
		}
	}

	// 最终更新一次所有的位置
	alignItems();
}

bool BalloonItemManager::appendBalloonItem(BalloonItem* pBalloonItem)
{
	if (!pBalloonItem || !m_bInited || !m_pDelegate || !m_pLayerItemContainer) return false;

	if (!pBalloonItem->getBalloonItemDelegate())
	{
		pBalloonItem->setBalloonItemDelegate(m_pDelegate);
	}
	// 把道具添加到道具层
	m_pLayerItemContainer->addChild(pBalloonItem);

	alignItems();

	return true;
}

bool BalloonItemManager::appendBalloonItemWithItemId(BalloonItemId eID, const char* pszTextureFile, unsigned long nCnts)
{
	BalloonItem* pBalloonItem = NULL;
	switch(eID)
	{
		case kBalloonItemId_Pumps:
			pBalloonItem = BalloonItemClick::create(m_pDelegate, pszTextureFile, nCnts);
			if (pBalloonItem)
			{
				pBalloonItem->setItemId(eID);
				return appendBalloonItem(pBalloonItem);
			}
			break;
		case kBalloonItemId_AddMore:
			break;
	}

	return false;
}

void BalloonItemManager::alignItems()
{
	CCPoint posItem = CCPointZero;
	posItem.y = CCDirector::sharedDirector()->getWinSize().height*0.2f;

	CCObject* pObj = NULL;
	CCARRAY_FOREACH(m_pLayerItemContainer->getChildren(), pObj)
	{
		BalloonItem* pBalloonItem = dynamic_cast<BalloonItem*>(pObj);
		if (pBalloonItem)
		{
			static float fOffsetX = pBalloonItem->getContentSize().width;

			posItem.x += fOffsetX;

			// 排序，从左下角开始排序
			pBalloonItem->setPosition(posItem);
		}
	}
}

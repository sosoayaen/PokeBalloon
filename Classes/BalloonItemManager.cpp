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

    // 添加道具后重新排布下位置
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
        default:
            break;
	}

	return false;
}

bool BalloonItemManager::appendBalloonItemWithItemId(BalloonItemId eID, cocos2d::CCSpriteFrame* pSpriteFrame, unsigned long nCnts)
{
    
	BalloonItem* pBalloonItem = NULL;
	switch(eID)
	{
		case kBalloonItemId_Pumps:
			pBalloonItem = BalloonItemClick::create(m_pDelegate, pSpriteFrame, nCnts);
			if (pBalloonItem)
			{
				pBalloonItem->setItemId(eID);
				return appendBalloonItem(pBalloonItem);
			}
			break;
		case kBalloonItemId_AddMore:
			break;
        default:
            break;
	}

	return false;
}

void BalloonItemManager::alignItems()
{
    unsigned long nLoopCnts = 0;
	CCObject* pObj = NULL;
	CCARRAY_FOREACH(m_pLayerItemContainer->getChildren(), pObj)
	{
		BalloonItem* pBalloonItem = dynamic_cast<BalloonItem*>(pObj);
		if (pBalloonItem)
		{
            
            CCPoint posItem = CCPointZero;
            CCSize size = CCDirector::sharedDirector()->getWinSize();
            posItem.y = size.height*0.2f;
            
            int nModeResult = nLoopCnts%3;
            int nTail = ceil(nLoopCnts/3);
            
            float fOffsetX = 0;
			float fOffsetY = nTail*pBalloonItem->getContentSize().height*1.1f;

            switch(nModeResult)
            {
                case 2:     // 右边
                    fOffsetX = size.width*5/6;
                    break;
                case 0:     // 左边
                    fOffsetX = size.width*1/6;
                    break;
                case 1:     // 中间
                    fOffsetX = size.width*0.5f;
                    break;
            }
			posItem.x += fOffsetX;
            posItem.y += fOffsetY;

			// 排序，从左下角开始排序
			pBalloonItem->setPosition(posItem);
            
            nLoopCnts++;
		}
	}
}

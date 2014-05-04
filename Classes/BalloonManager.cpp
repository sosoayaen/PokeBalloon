/*
 * =====================================================================================
 *
 *       Filename:  BalloonManager.cpp
 *
 *    Description:  气球管理类的实现文件
 *
 *        Version:  1.0
 *        Created:  2014/04/20 11时32分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonManager.h"

USING_NS_CC;

BalloonManager::BalloonManager():
	m_bInited(false),
	m_pLayerBalloonContainer(NULL),
	m_pBalloonDelegate(NULL)
{
	// 初始化程序界面大小
	m_sizeScreen = CCDirector::sharedDirector()->getWinSize();
}

BalloonManager::~BalloonManager()
{
}

void BalloonManager::init(BalloonDelegate* pDelegate, cocos2d::CCLayer* pLayerBalloonContainer)
{
	m_pBalloonDelegate = pDelegate;
	m_pLayerBalloonContainer = pLayerBalloonContainer;
    
    m_bInited = pDelegate && pLayerBalloonContainer;
}

bool BalloonManager::getInitFlag()
{
    return m_bInited;
}

void BalloonManager::updatePosition()
{
	// 迭代气球数组，更新位置
	// for (BalloonObjectList::iterator iter = m_BalloonObjectList.begin();
	//	iter != m_BalloonObjectList.end();)
	// {
    CCObject* pObj = NULL;
    CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
    {
		// Balloon* pBalloon = *iter;
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
        
        if (!pBalloon || !pBalloon->isAlive()) continue;
        
		pBalloon->updatePosition();

		// 判断如果气球移出屏幕择直接消失
		if (pBalloon->getPositionY() > m_sizeScreen.height) 
		{
			// 从层中移除对象
			if (pBalloon->getParent())
			{
				pBalloon->removeFromParent();
			}
			// 从数组中移除对象
			// iter = m_BalloonObjectList.erase(iter);
			// continue;
		}
		// ++iter;
	}
}

bool BalloonManager::hitTest(const cocos2d::CCRect& rect, cocos2d::CCSprite* pSprite /* = NULL */) 
{
	bool bRet = false;
	// 迭代气球数组
	// for (BalloonObjectList::reverse_iterator iter = m_BalloonObjectList.rbegin();
	// 	iter != m_BalloonObjectList.rend();
	// 	++iter)
	// {
	// 	Balloon* pBalloon = *iter;
	CCObject* pObj = NULL;
	CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
	{
		Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
		if (pBalloon && pBalloon->isAlive() && pBalloon->hitTest(rect))
		{
			// 回调下通知外层碰撞成功了
			if (m_pBalloonDelegate)
				m_pBalloonDelegate->balloonHitTestSuccess(pBalloon, pSprite);

			bRet = true;
			break;	
		}
	}

	return bRet;
}

bool BalloonManager::touchTest(const cocos2d::CCPoint point, cocos2d::CCSprite* pSprite /* = NULL */)
{
	bool bRet = false;

	// 迭代气球数组
	// for (BalloonObjectList::reverse_iterator iter = m_BalloonObjectList.rbegin();
	// 	iter != m_BalloonObjectList.rend();
	// 	++iter)
	// {
	// 	Balloon* pBalloon = *iter;
	CCObject* pObj = NULL;
	CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
	{
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
		if (pBalloon && pBalloon->isAlive() && pBalloon->touchTest(point))
		{
		
			if (m_pBalloonDelegate)
				m_pBalloonDelegate->balloonTouchTestSuccess(pBalloon, pSprite);

			// 气球爆炸回调
			pBalloon->explorsive();

			bRet = true;
			break;
		}
	}

	return bRet;
}

void BalloonManager::addRandomBalloon()
{
	if (m_pLayerBalloonContainer)
	{
		// 随机生成一个气球
        int nBalloonIndex = rand()%7+1;
		CCString* pStrBalloonName = CCString::createWithFormat("balloon/balloon_1_%d.png", nBalloonIndex);
        int nValue = rand()%3 + 1;
        BalloonType nType = kBalloonTypeNormal;

        int nRate = rand()%100;
        if (nRate > 90)
        {
            pStrBalloonName = CCString::createWithFormat("balloon/balloon_4_%d.png", nBalloonIndex);
            nValue = 2;
            nType = kBalloonTypeMulti;
        }
        else if (nRate > 80)
        {
            pStrBalloonName = CCString::createWithFormat("balloon/balloon_3_%d.png", nBalloonIndex);
            nValue = 2;
            nType = kBalloonTypeDiv;
        }
        else if (nRate > 70)
        {
            pStrBalloonName = CCString::createWithFormat("balloon/balloon_2_%d.png", nBalloonIndex);
            nValue *= -1;
        }
        
		// 设定随机缩放
		float fScale = 1.5f;

		// 随机生成X坐标，起始值为0加气球的半个宽度，终止值为屏幕宽减去半个气球宽度
		float fStartPosX = fmod(rand(), CCDirector::sharedDirector()->getWinSize().width);
		float fStartPosY = 0;

		// 增加一个气球到屏幕
		Balloon* pBalloon = Balloon::create(pStrBalloonName->getCString(), nValue, nType);
        if (!pBalloon) return;
		pBalloon->setPosition(ccp(fStartPosX, fStartPosY));
		pBalloon->setSpeedY(rand()%50/5.0f + 0.5f);

		// m_BalloonObjectList.push_back(pBalloon);

		// 添加到层
		m_pLayerBalloonContainer->addChild(pBalloon);

	}
}

void BalloonManager::multipleBalloonScore(long nValue)
{
	CCObject* pObj = NULL;
	CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
	{
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
		if (pBalloon && pBalloon->isAlive() && pBalloon->getBalloonType() == kBalloonTypeNormal)
		{
			// if (m_pBalloonDelegate)
			//	m_pBalloonDelegate->balloonTouchTestSuccess(pBalloon, pSprite);
            pBalloon->setBalloonScore(pBalloon->getBalloonScore()*nValue);
            // 更新显示文字
            pBalloon->updateDisplayDesc();
		}
	}
}
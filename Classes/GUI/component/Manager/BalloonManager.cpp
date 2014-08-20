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

BalloonTypeRate::BalloonTypeRate()
{
    // 初始化每个气球的概率
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeMulti, 50)); // 5%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypePump, 50)); // 5%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeAddTime, 50)); // 5%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeFrozen, 20)); // 2%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeGiant, 20));  // 2%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeReverse, 30)); // 3%
    m_vRateArray.push_back(BalloonTypeRateElement(kBalloonTypeBoom, 30)); // 3%
}

BalloonType BalloonTypeRate::getRandomBalloonTypeByRandom() const
{
    BalloonType eRet = kBalloonTypeNormal;
    
    int rate = rand()%1000;
    
    int nSum = 0;
    std::vector<BalloonTypeRateElement>::const_iterator iter = m_vRateArray.begin();
    for (; iter != m_vRateArray.end(); iter++)
    {
        const BalloonTypeRateElement& element = *iter;
        nSum += element.nRate;
        if (rate < nSum)
        {
            eRet = element.eType;
            break;
        }
    }
    
    return eRet;
}

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
    CCObject* pObj = NULL;
    CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
    {
		// Balloon* pBalloon = *iter;
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
        
        if (!pBalloon || !pBalloon->isAlive()) continue;
        
		pBalloon->updatePosition();

		// 判断如果气球移出屏幕择直接消失
		if (pBalloon->getPositionY() > m_sizeScreen.height + pBalloon->getContentSize().height)
		{
            // 通知外层，这个气球移出屏幕边界了
            m_pBalloonDelegate->balloonMoveOutOfScreen(pBalloon);
            
			// 从层中移除对象
			if (pBalloon->getParent())
			{
				pBalloon->removeFromParent();
			}
		}
	}
}

bool BalloonManager::hitTest(const cocos2d::CCRect& rect, cocos2d::CCSprite* pSprite /* = NULL */) 
{
	bool bRet = false;
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

	CCObject* pObj = NULL;
	CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
	{
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
		if (pBalloon && pBalloon->isAlive() && pBalloon->touchTest(point))
		{
            // 点击次数加1
            pBalloon->setBalloonClickCnt(pBalloon->getBalloonClickCnt() + 1);
            
            switch (pBalloon->getBalloonType())
            {
                case kBalloonTypeGiant:
                    // 更新下气球的描述
                    pBalloon->updateDisplayDesc();
                    break;
                    
                default:
                    break;
            }
		
			if (m_pBalloonDelegate)
				m_pBalloonDelegate->balloonTouchTestSuccess(pBalloon, pSprite);

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
        int nBalloonIndex = rand()%8 + 1;
        // 这里是气球点击爆炸后的粒子效果的颜色
        BalloonColor eColor = (BalloonColor)nBalloonIndex;
        // 普通气球是1至3分
        int nValue = rand()%3 + 1;
        BalloonType nType = kBalloonTypeNormal;
        // 目前只有4个不同种类的气球
        int nBalloonStyle = rand()%4 + 1;
        
        // 默认可点击1下
        unsigned int nClickableCnt = 1;

        // CCString* pStrBalloonName = CCString::createWithFormat("texture/balloon/balloon_%d_%d.png", nBalloonStyle, nBalloonIndex);
        CCString* pStrBalloonName = CCString::createWithFormat("balloon_%d_%d.png", nBalloonStyle, nBalloonIndex);
        
        /*
        int nRate = rand()%100;
        if (nRate > 95)
        {
            // 两倍出现的概率为5%
            nValue = 2;
            nType = kBalloonTypeMulti;
        }
        else if (nRate > 90)
        {
            // 道具气球出现概率为10%
            nType = kBalloonTypePump;
            nValue = rand()%5 + 1; // 每按一次加1分
        }
        else if (nRate > 87)
        {
            // 除二出现的概率为3%
            nValue = 2;
            nType = kBalloonTypeBoom;
        }
        else if (nRate > 82)
        {
            // 时钟出现的概率为5%
            nValue = 3; // 3秒
            nType = kBalloonTypeAddTime;
        }
        else if (nRate > 80)
        {
            nValue = 0; // 冰冻是一次性效果
            nType = kBalloonTypeFrozen;
        }
        else if (nRate > 77)
        {
            nValue = -1; // 默认正负反向，数值为对应的倍数
            nType = kBalloonTypeReverse;
        }
        else if (nRate > 75)
        {
            nClickableCnt = rand()%9 + 3; // 最多需要击打12下
            nType = kBalloonTypeGiant;
            // 得分为按下的2倍
            nValue = 1*nClickableCnt;
        }
        else if (nRate > 67)
        {
            // 10%概率出现负分
            nValue *= -1;
        }
        */
        
        nType = m_BalloonTypeRate.getRandomBalloonTypeByRandom();
        switch (nType)
        {
            case kBalloonTypeMulti:
                // 两倍
                nValue = 2;
                break;
            case kBalloonTypePump:
                // 可点击次数为1到5
                nValue = rand()%5 + 1;
                break;
            case kBalloonTypeBoom:
                // 炸弹的效果是除二
                nValue = 2;
                break;
            case kBalloonTypeAddTime:
                // 增加3秒钟时间
                nValue = 3;
                break;
            case kBalloonTypeFrozen:
                // 冰冻是一次性效果
                nValue = 0;
                break;
            case kBalloonTypeReverse:
                // 默认正负反向，数值为对应的倍数
                nValue = -1;
                break;
            case kBalloonTypeGiant:
                nClickableCnt = rand()%9 + 3; // 最多打12下
                // 得分为按下的x倍
                nValue = 1*nClickableCnt;
                break;
            default:
                // 10%的概率出现负分
                if (rand()%100 <= 12)
                    nValue *= -1;
                break;
        }
        
		// 设定随机缩放，90%到120%
		float fScale = (rand()%3 + 9)*0.1f;

		// 增加一个气球到屏幕
		// Balloon* pBalloon = Balloon::create(pStrBalloonName->getCString(), nValue, nType);
		Balloon* pBalloon = Balloon::create(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pStrBalloonName->getCString()), nValue, nType);
        
        if (!pBalloon) return;
        
        CCSize balloonSize = pBalloon->getContentSize();
        
		// 随机生成X坐标，起始值为0加气球的半个宽度，终止值为屏幕宽减去半个气球宽度
		float fStartPosX = fmod(rand(), CCDirector::sharedDirector()->getWinSize().width - balloonSize.width);
		float fStartPosY = - balloonSize.height;
		pBalloon->setPosition(ccp(fStartPosX + balloonSize.width/2, fStartPosY));
		pBalloon->setSpeedY(rand()%50/5.0f + 1.0f);
        // pBalloon->setBalloonColor4F(color);
        pBalloon->setBalloonColor(eColor);
        
        pBalloon->setScale(fScale);
        
        // 每个气球加一个随机的晃动效果
        float fRandomMoveDuration = (rand()%20+1)/10.0f;
        float fRandomMoveDistance = rand()%int(pBalloon->boundingBox().size.width) * (rand()%2 ? 1 : -1);
        CCSequence* pActionShake = CCSequence::create(CCMoveBy::create(fRandomMoveDuration, ccp(fRandomMoveDistance, 0)), CCDelayTime::create(rand()%10/10.0f), CCMoveBy::create(fRandomMoveDuration, ccp(-fRandomMoveDistance, 0)), NULL);
        
        // 自动摇摆
        float fRandomRotateDuration = (rand()%15+1)/10.0f;
        float fRandomRotateAngle = (rand()%30+1) * (rand()%2 ? 1 : -1);
        CCSequence* pActionRotate = CCSequence::create(CCRotateBy::create(fRandomRotateDuration, fRandomRotateAngle), CCDelayTime::create(fRandomRotateDuration), CCRotateBy::create(fRandomRotateDuration, -fRandomRotateAngle),NULL);

        pBalloon->runAction(CCSpawn::create(pActionShake, pActionRotate, NULL));
        
        // 给气球一个随机角度
        pBalloon->setRotation(rand()%30 - 15);
        
        // 设置可点击次数
        pBalloon->setBalloonClickableCnt(nClickableCnt);
        
        // 设置气球的套ID
        pBalloon->setBalloonSets(nBalloonStyle);

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
            pBalloon->setBalloonScore(pBalloon->getBalloonScore()*nValue);
            // 更新显示文字
            pBalloon->updateDisplayDesc();
		}
	}
}

bool BalloonManager::isBalloonInScreen()
{
    CCObject* pObj = NULL;
    
	CCARRAY_FOREACH_REVERSE(m_pLayerBalloonContainer->getChildren(), pObj)
	{
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
		if (pBalloon && pBalloon->isAlive())
		{
            return true;
		}
	}
    return false;
}

void BalloonManager::addBalloonScoreWithValue(long nValue)
{
    CCObject* pObj = NULL;
    
    CCARRAY_FOREACH(m_pLayerBalloonContainer->getChildren(), pObj)
    {
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
        // 只有气球，并且是普通分值的气球，再并且是活着的普通气球
        if (pBalloon && pBalloon->getBalloonType() == kBalloonTypeNormal && pBalloon->isAlive())
        {
            pBalloon->setBalloonScore(pBalloon->getBalloonScore() + nValue);
            pBalloon->updateDisplayDesc();
        }
    }
}

void BalloonManager::setAllBalloonSpeedY(float fSpeedY)
{
    CCObject* pObj = NULL;
    
    CCARRAY_FOREACH(m_pLayerBalloonContainer->getChildren(), pObj)
    {
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
        if (pBalloon)
        {
            pBalloon->setSpeedY(fSpeedY);
        }
    }
}

void BalloonManager::setAllBalloonEffect(BalloonEffect effect)
{
    CCObject* pObj = NULL;
    
    CCARRAY_FOREACH(m_pLayerBalloonContainer->getChildren(), pObj)
    {
        Balloon* pBalloon = dynamic_cast<Balloon*>(pObj);
        if (pBalloon)
        {
            pBalloon->setBalloonEffect(effect);
        }
    }
}
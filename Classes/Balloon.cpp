//
//  Balloon.cpp
//  PokeBalloon
//

#include "Balloon.h"
#include "BalloonSoundManager.h"

USING_NS_CC;

Balloon::Balloon():
    m_eType(kBalloonTypeNormal),
	m_lScore(1),
	m_bAlive(true),
    m_ulFrame(0),
    m_fSpeedX(0),
    m_fSpeedY(0),
    m_fAcceleratedSpeedX(0),
    m_fAcceleratedSpeedY(0)
{
    m_pLabelDesc = NULL;
}

Balloon::~Balloon()
{
}

Balloon* Balloon::create(const char* pszFileName)
{
	Balloon* pobBalloon = new Balloon();
	if (pobBalloon && pobBalloon->initWithFile(pszFileName))
	{
		pobBalloon->autorelease();
		return pobBalloon;
	}
	CC_SAFE_DELETE(pobBalloon);
	
	return NULL;
}

Balloon* Balloon::create(const char* pszFileName, long lScore, BalloonType nType /* = kBalloonTypeNormal */)
{
	Balloon* pobBalloon = new Balloon();
	if (pobBalloon && pobBalloon->initWithFile(pszFileName))
	{
		pobBalloon->autorelease();
        pobBalloon->setBalloonScore(lScore);
        pobBalloon->setBalloonType(nType);
		return pobBalloon;
	}
	CC_SAFE_DELETE(pobBalloon);
	
	return NULL;
}

Balloon* Balloon::create(const char* pszFileName, const CCRect& rect)
{
	Balloon* pobBalloon = new Balloon();
	if (pobBalloon && pobBalloon->initWithFile(pszFileName, rect))
	{
		pobBalloon->autorelease();
		return pobBalloon;
	}
	CC_SAFE_DELETE(pobBalloon);
	
	return NULL;
}

Balloon* Balloon::createWithSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame)
{
	Balloon* pobBalloon = new Balloon();
	if (pSpriteFrame && pobBalloon && pobBalloon->initWithSpriteFrame(pSpriteFrame))
	{
		pobBalloon->autorelease();
		return pobBalloon;
	}
	CC_SAFE_DELETE(pobBalloon);
	
	return NULL;
}

Balloon* Balloon::createWithSpriteFrameName(const char* pszSpriteFrameName)
{
	CCSpriteFrame* pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);

#if COCOS2D_DEBUG > 0
	char msg[256] = {0};
	sprintf(msg, "Invalid spriteFrameName: %s", pszSpriteFrameName);
	CCAssert(pFrame != NULL, msg);
#endif	

	return createWithSpriteFrame(pFrame);
}

void Balloon::updatePosition()
{
    // 帧自增
    m_ulFrame++;
    
    // 更新速度
    m_fSpeedX += m_fAcceleratedSpeedX;
    m_fSpeedY += m_fAcceleratedSpeedY;
    
    // 根据帧数调整位置
    float fPosX = m_fSpeedX + getPositionX();
    float fPosY = m_fSpeedY + getPositionY();
    setPosition(ccp(fPosX, fPosY));
    
}

void Balloon::explorsive()
{
	// 播放气球爆炸音效
	BalloonSoundManager::sharedBalloonSoundManager()->playEffectExplosive();

	// 气球死亡
	setAliveEnable(false);
	
	// 这里用淡出模拟下
	runAction(CCSequence::create(CCFadeOut::create(0.2f), CCRemoveSelf::create(), NULL));
}

bool Balloon::hitTest(const cocos2d::CCRect& rect)
{
	CCRect rectBoundingBox = boundingBox();
	// 转到世界坐标
	rectBoundingBox.origin = convertToWorldSpace(rectBoundingBox.origin);

	return rectBoundingBox.intersectsRect(rect);
}

bool Balloon::touchTest(const cocos2d::CCPoint& point)
{
	CCRect rect = boundingBox();

	return rect.containsPoint(point);
}

void Balloon::onEnter()
{
    CCSprite::onEnter();
    
    updateDisplayDesc();
}

void Balloon::updateDisplayDesc()
{
    CCString* pStrDesc = NULL;
    switch (m_eType)
    {
        case kBalloonTypeNormal:
            pStrDesc = CCString::createWithFormat("%+ld", getBalloonScore());
            break;
        case kBalloonTypeMulti:
            pStrDesc = CCString::createWithFormat("x%ld", getBalloonScore());
            break;
        case kBalloonTypeDiv:
            pStrDesc = CCString::createWithFormat("%%%ld", getBalloonScore());
            break;
        default:
            break;
    }
    
    // 更新显示文字
    m_strDisplayDesc = pStrDesc->getCString();
    if (!m_pLabelDesc)
    {
        m_pLabelDesc = CCLabelTTF::create("", "", 30);
        m_pLabelDesc->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
        addChild(m_pLabelDesc);
    }
    m_pLabelDesc->setString(m_strDisplayDesc.c_str());
}

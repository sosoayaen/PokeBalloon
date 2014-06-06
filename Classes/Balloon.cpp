//
//  Balloon.cpp
//  PokeBalloon
//

#include "Balloon.h"
#include "BalloonSoundManager.h"

USING_NS_CC;

Balloon::Balloon():
    m_eType(kBalloonTypeNormal),
    m_nBalloonSets(0),
    m_lLastTime(0),
	m_lScore(1),
	m_bAlive(true),
    m_ulFrame(0),
    m_fSpeedX(0),
    m_fSpeedY(0),
    m_nClickableCnt(1),
    m_nClickCnt(0),
    m_fAcceleratedSpeedX(0),
    m_fAcceleratedSpeedY(0)
{
    // m_pLabelDesc = NULL;
    m_pLabelBMFontDesc = NULL;
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

Balloon* Balloon::create(cocos2d::CCSpriteFrame *pSpriteFrame, long lScore, BalloonType nType /* = kBalloonTypeNormal */)
{
    Balloon* pobBalloon = new Balloon();
    if (pSpriteFrame && pobBalloon && pobBalloon->initWithSpriteFrame(pSpriteFrame))
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

void Balloon::explosive()
{
	// 播放气球爆炸音效
	BalloonSoundManager::sharedBalloonSoundManager()->playEffectExplosive();

	// 气球死亡
	setAliveEnable(false);
	
	// 这里用淡出模拟下
	runAction(CCSequence::create(CCFadeOut::create(0.2f), CCRemoveSelf::create(), NULL));
    
    if (getParent())
    {
        // 增加粒子特效
        CCParticleSystemQuad* pParticleExplosive = CCParticleSystemQuad::create("particles/explosive.plist");
        pParticleExplosive->setAutoRemoveOnFinish(true);
        pParticleExplosive->setPosition(getPosition());
        pParticleExplosive->setEndColor(getBalloonColor4F());
        getParent()->addChild(pParticleExplosive);
    }
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
    // 数字的放大倍率，现在数字太小
    float fScaleRate = 2.0f;
    CCString* pStrDesc = NULL;
    switch (m_eType)
    {
        case kBalloonTypeNormal:
            pStrDesc = CCString::createWithFormat("%+ld", getBalloonScore());
            break;
        case kBalloonTypeMulti:
            pStrDesc = CCString::createWithFormat("×%ld", getBalloonScore());
            break;
        case kBalloonTypeBoom:
            {
                // pStrDesc = CCString::createWithFormat("÷%ld", getBalloonScore());
                CCSprite* pSprite = CCSprite::createWithSpriteFrameName("item_boom.png");
                pSprite->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
                addChild(pSprite);
            }
            break;
        case kBalloonTypeAddTime:
            {
                // 添加时钟标志，不需要绘制字体描述，所以直接返回
                // CCSprite* pSpriteClock = CCSprite::create("texture/balloon/balloon_time.png");
                CCSprite* pSpriteClock = CCSprite::createWithSpriteFrameName("item_time.png");
                pSpriteClock->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
                addChild(pSpriteClock);
            }
            break;
        case kBalloonTypeAddBalloonScore:
            // pStrDesc = ccs("?");
            // fScaleRate = 2.6f;
            {
                CCSprite* pSprite = CCSprite::createWithSpriteFrameName("item_pump.png");
                pSprite->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
                addChild(pSprite);
            }
            break;
        case kBalloonTypeFrozen:
            {
                CCSprite* pSpriteFrozen = CCSprite::createWithSpriteFrameName("item_frozen.png");
                pSpriteFrozen->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
                addChild(pSpriteFrozen);
            }
            break;
        case kBalloonTypeReverse:
            {
                CCSprite* pSpriteReverse = CCSprite::createWithSpriteFrameName("item_reverse.png");
                pSpriteReverse->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
                addChild(pSpriteReverse);
            }
            break;
        case kBalloonTypeGiant:
            pStrDesc = CCString::createWithFormat("%u/%u", getBalloonClickCnt(), getBalloonClickableCnt());
            break;
        default:
            // 默认是不显示文字
            return;
    }
    
    // 更新显示文字
    if (pStrDesc && !pStrDesc->m_sString.empty())
    {
        m_strDisplayDesc = pStrDesc->getCString();
        if (!m_pLabelBMFontDesc)
        {
            m_pLabelBMFontDesc = CCLabelBMFont::create(m_strDisplayDesc.c_str(), "texture/fonts/font.fnt");
            m_pLabelBMFontDesc->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
            m_pLabelBMFontDesc->setScale(fScaleRate);
            addChild(m_pLabelBMFontDesc);
        }
        m_pLabelBMFontDesc->setCString(m_strDisplayDesc.c_str());
    }
}

void Balloon::setBalloonEffect(BalloonEffect effect)
{
    CCSprite* pSpriteEffect = NULL;
    
    switch (effect)
    {
        case kBalloonEffectFrozen:
            pSpriteEffect = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("balloon_effect_frozen_%d.png", getBalloonSets())->getCString());
            break;
            
        default:
            break;
    }
    
    if (pSpriteEffect)
    {
        pSpriteEffect->setAnchorPoint(ccp(0.5f, 0));
        pSpriteEffect->setPosition(ccp(getContentSize().width*0.5f, 0));
        // 放到最下层
        // pSpriteEffect->setZOrder(-100);
        addChild(pSpriteEffect);
    }
    
}
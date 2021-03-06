/*
 * =====================================================================================
 *
 *       Filename:  BalloonItem.cpp
 *
 *    Description:  道具状态实现类
 *
 *        Version:  1.0
 *        Created:  05/23/2014 01:04:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonItem.h"
#include "bailinUtil.h"
#include "BalloonSoundManager.h"

USING_NS_CC;
USING_NS_BAILIN_UTIL;

BalloonItem::BalloonItem():
    m_pBalloonItemDelegate(NULL),
    m_nItemValue(0),
    m_eItemId(kBalloonItemId_Unknow),
    m_eType(kBalloonItemType_Unknow)
{
    // 默认居中对齐
    setAnchorPoint(ccp(0.5f, 0.5f));
}

void BalloonItem::onEnter()
{
    CCNode::onEnter();
    
    // 缩小到10%
    this->setScale(0.1f);
    // 弹出来
    this->runAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
}

/**
 * 持续性道具类实现
 */
BalloonItemDurative::BalloonItemDurative():
	m_fItemDuration(0)
{
    m_eType = kBalloonItemType_Durative;
}

void BalloonItemDurative::updateItemStatus(float dt, unsigned long ulFrame)
{
	// 更新当前帧
	m_ulCurrentFrame = ulFrame;

}

void BalloonItemDurative::pause()
{

}

void BalloonItemDurative::resume()
{

}

void BalloonItemDurative::extendEffect(BalloonItem *pBalloonItem)
{
    
}

/**
 * 点击型道具类实现
 */
BalloonItemClick::BalloonItemClick():
	m_ulClickCounts(1),
    m_pSpriteIcon(NULL),
    m_pLabelBMFontCnts(NULL),
    m_pMenuClick(NULL)
{
    m_eType = kBalloonItemType_Click;
}

BalloonItemClick::~BalloonItemClick()
{
	CC_SAFE_RELEASE_NULL(m_pMenuClick);
	CC_SAFE_RELEASE_NULL(m_pSpriteIcon);
}

void BalloonItemClick::onPressMenuClick(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
	if (m_pBalloonItemDelegate && m_ulClickCounts > 0)
	{
		// 按下后，首先回调外围
		m_pBalloonItemDelegate->onBalloonItemEffectTrigger(this);
        
        m_ulClickCounts--;
        // 更新界面
        updateLeftCntsLabel();
        
		// 点击次数递减
		if (m_ulClickCounts == 0)
		{
			// 通知外围，道具移除
			m_pBalloonItemDelegate->onBalloonItemBeforeDisappear(this);
			// 移除道具
			removeFromParent();
            // 道具移除后
			m_pBalloonItemDelegate->onBalloonItemAfterDisappear(this);
		}
        
	}
}

BalloonItemClick* BalloonItemClick::create(BalloonItemDelegate* pDelegate, const char* pszTextureFile, unsigned long ulCnts)
{
    if (pszTextureFile)
    {
        CCSpriteFrame* pSpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszTextureFile);
        return BalloonItemClick::create(pDelegate, pSpriteFrame, ulCnts);
    }
    
    return NULL;
}

BalloonItemClick* BalloonItemClick::create(BalloonItemDelegate *pDelegate, cocos2d::CCSpriteFrame *pSpriteFrame, unsigned long ulCnts)
{
	BalloonItemClick* pRet = NULL;

	if (pSpriteFrame)
	{
		CCSprite* pSpriteMenuItem = CCSprite::createWithSpriteFrame(pSpriteFrame);
		if (pSpriteMenuItem)
		{
			// 创建对象
			pRet = new BalloonItemClick();
			if (pRet && pRet->init())
			{
				// 初始化按钮
                pRet->initClickMenu(pSpriteMenuItem);
                // 设定外部回调实例
                pRet->setBalloonItemDelegate(pDelegate);
                // 设定点击次数
                pRet->setClickCounts(ulCnts);
                
                pRet->autorelease();
			}
			else
			{
				CC_SAFE_DELETE(pRet);
			}
		}
	}
	
	return pRet;
}

void BalloonItemClick::updateItemStatus(float dt, unsigned long ulFrame)
{
    // 暂时没用
}

void BalloonItemClick::pause()
{
	
}

void BalloonItemClick::resume()
{

}

void BalloonItemClick::onEnter()
{
    BalloonItem::onEnter();
    
    // 显示剩余次数
    updateLeftCntsLabel();
}

void BalloonItemClick::initClickMenu(CCSprite* pSpriteIcon)
{
    // 加载按钮图形
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/items/items.plist");
    // 创建按钮
    CCMenuItemImage* pMenuItemImage = CCMenuItemImage::create();
    // 设置按钮的图片
    // pMenuItemImage->setNormalImage(CCSprite::create("texture/items/item_btn_normal.png"));
    pMenuItemImage->setNormalSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("item_btn_normal.png"));
    // pMenuItemImage->setSelectedSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("item_btn_normal.png"));
    
    // 顺便这里更新下控件的大小
    setContentSize(pMenuItemImage->getContentSize());
    
    // 设置按下后的回调函数
    pMenuItemImage->setTarget(this, menu_selector(BalloonItemClick::onPressMenuClick));
    // 设置按钮
    m_pMenuClick = CCMenu::create();
    m_pMenuClick->addChild(pMenuItemImage);
    CC_SAFE_RETAIN(m_pMenuClick);
    // 设置按钮居中对齐
    m_pMenuClick->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
    
    // 设置按下后的图片放大效果
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuClick, 1.2f);
    // 添加到界面上
    addChild(m_pMenuClick);
    
    m_pSpriteIcon = pSpriteIcon;
    m_pSpriteIcon->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
    CC_SAFE_RETAIN(m_pSpriteIcon);
    addChild(m_pSpriteIcon);
    
}

void BalloonItemClick::updateLeftCntsLabel()
{
    if (!m_pLabelBMFontCnts)
    {
        m_pLabelBMFontCnts = CCLabelBMFont::create("", "texture/fonts/font.fnt");
        m_pLabelBMFontCnts->setPosition(ccpFromSize(m_pSpriteIcon->getContentSize()));
        m_pSpriteIcon->addChild(m_pLabelBMFontCnts);
    }
    m_pLabelBMFontCnts->setCString(CCString::createWithFormat("%lu", m_ulClickCounts)->getCString());
}

void BalloonItemClick::extendEffect(BalloonItem *pBalloonItem)
{
    BalloonItemClick* pItem = dynamic_cast<BalloonItemClick*>(pBalloonItem);
    if (pItem)
    {
        setClickCounts(getClickCounts() + pItem->getClickCounts());
        // 更新次数列表
        updateLeftCntsLabel();
        
        // 放大一下右上角数字
        m_pLabelBMFontCnts->runAction(CCSequence::create(CCScaleTo::create(0.2f, 2.0f), CCScaleTo::create(0.2f, 1.0f), NULL));
    }
}
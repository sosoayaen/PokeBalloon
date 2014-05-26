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

USING_NS_CC;
USING_NS_BAILIN_UTIL;


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

/**
 * 点击型道具类实现
 */
BalloonItemClick::BalloonItemClick():
	m_ulClickCounts(1)
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
	if (m_pBalloonItemDelegate && m_ulClickCounts > 0)
	{
		// 按下后，首先回调外围
		m_pBalloonItemDelegate->onBalloonItemEffectTrigger(this);

		// 点击次数递减
		if (--m_ulClickCounts == 0)
		{
			// 通知外围，道具移除
			m_pBalloonItemDelegate->onBalloonItemDisappear(this);
			// 移除道具
			removeFromParent();
		}
	}
}

BalloonItemClick* BalloonItemClick::create(BalloonItemDelegate* pDelegate, const char* pszTextureFile, unsigned long ulCnts)
{
	BalloonItemClick* pRet = NULL;

	if (pszTextureFile)
	{
		CCSprite* pSpriteMenuItem = CCSprite::create(pszTextureFile);
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
	// 绘制剩余次数
	return;
}

void BalloonItemClick::pause()
{
	
}

void BalloonItemClick::resume()
{

}

void BalloonItemClick::initClickMenu(CCSprite* pSpriteIcon)
{
    // 创建按钮
    CCMenuItemImage* pMenuItemImage = CCMenuItemImage::create();
    // 设置按钮的图片
    pMenuItemImage->setNormalImage(CCSprite::create("items/item_btn_normal.png"));
    // pMenuItemImage->setSelectedImage(CCSprite::create("items/item_btn_selected.png"));
    
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

#include "LoadingSprit.h"
// #include "waitinglayer.h"
#include "ControlUtil.h"
#include "bailinUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

NS_BAILIN_UTIL_BEGIN

static struct cc_timeval g_sTimeVal;

ControlUtil::ControlUtil()
{
	m_sizeScreen = CCDirector::sharedDirector()->getWinSize();
//	m_fScaleFactor = CCDirector::sharedDirector()->getContentScaleFactor();
    memset(&g_sTimeVal, 0, sizeof(g_sTimeVal));
}

ControlUtil::~ControlUtil()
{

}

static ControlUtil* g_sharedControlUtil = NULL;

ControlUtil* ControlUtil::sharedControlUtil()
{
	if (!g_sharedControlUtil)
	{
		g_sharedControlUtil = new ControlUtil();
	}

	return g_sharedControlUtil;
}

void ControlUtil::purgeSharedControlUtil()
{
	CC_SAFE_DELETE(g_sharedControlUtil);
}

void ControlUtil::FitLabelTTFFontSize( cocos2d::CCLabelTTF* pLabel )
{
	if (pLabel)
	{
		pLabel->setFontSize(pLabel->getFontSize()/CCDirector::sharedDirector()->getContentScaleFactor());
	}
}

void ControlUtil::SetMenuItemSelectedImageWithNormalImage( cocos2d::CCMenu* pMenu, float fRate /*= 1.1f */ )
{
	CCObject* pObj = NULL;
    
	CCARRAY_FOREACH(pMenu->getChildren(), pObj)
	{
		CCMenuItemImage* pMenuItem = dynamic_cast<CCMenuItemImage* >(pObj);
		SetMenuItemSelectedImageWithNormalImage(pMenuItem, fRate);
	}
}

void ControlUtil::SetMenuItemSelectedImageWithNormalImage( cocos2d::CCMenuItemImage* pMenuItemImage, float fRate /*= 1.1f */ )
{
	if (pMenuItemImage)
	{
		CCSprite* pSprite = dynamic_cast<CCSprite*>(pMenuItemImage->getNormalImage());
		if (pSprite)
		{
			CCSprite* pSpriteSelected = new CCSprite;
			pSpriteSelected->autorelease();
			pSpriteSelected->initWithTexture(pSprite->getTexture(), pSprite->getTextureRect(), pSprite->isTextureRectRotated());

			// 这里设置锚点没用。setSelectedImage后会归零
			// pSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
			// 根据放大比例来计算中心点对齐的位移
			// 得到往左下角偏移的距离
			float x = (fRate - 1.0f)*pSpriteSelected->getContentSize().width*0.5f;
			float y = (fRate - 1.0f)*pSpriteSelected->getContentSize().height*0.5f;
			pSpriteSelected->setPosition(ccpSub(CCPointZero, ccp(x, y)));

			pSpriteSelected->setScale(pSpriteSelected->getScale()*fRate);
			pMenuItemImage->setSelectedImage(pSpriteSelected);
		}
	}
}

void ControlUtil::ShowCocos2dxMessageBox( const char* pszContent, const char* pszTitle /*= NULL*/ )
{
	if (!pszContent)
	{
		return;
	}

	if (!pszTitle)
	{
		pszTitle = DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("SystemText", "dialog_title");
	}

#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)

#endif
	CCMessageBox(pszContent, pszTitle);

}

void ControlUtil::SetNodeVisible( cocos2d::CCNode* pNode, bool bVisible )
{
	if (pNode)
	{
		pNode->setVisible(bVisible);
	}
}

void ControlUtil::SetMenuItemSelectedImageWithNormalImageAndLabel( cocos2d::CCMenuItemImage* pMenuItemImage, float fRate /*= 1.1f */ )
{
	if (pMenuItemImage)
	{
		CCSprite* pSprite = dynamic_cast<CCSprite*>(pMenuItemImage->getNormalImage());
		if (pSprite)
		{
			CCSprite* pSpriteSelected = new CCSprite;
			pSpriteSelected->autorelease();
			pSpriteSelected->initWithTexture(pSprite->getTexture(), pSprite->getTextureRect(), pSprite->isTextureRectRotated());

			// 这里设置锚点没用。setSelectedImage后会归零
			// pSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
			// 根据放大比例来计算中心点对齐的位移
			// 得到往左下角偏移的距离
			float x = (fRate - 1.0f)*pSpriteSelected->getContentSize().width*0.5f;
			float y = (fRate - 1.0f)*pSpriteSelected->getContentSize().height*0.5f;
			pSpriteSelected->setPosition(ccpSub(CCPointZero, ccp(x, y)));

			// 把内部字体也复制一份
			CCObject* pObj = NULL;
			CCARRAY_FOREACH(pSprite->getChildren(), pObj)
			{
				CCLabelTTF* pLabelTTF = dynamic_cast<CCLabelTTF*>(pObj);
				if (pLabelTTF)
				{
					CCLabelTTF* pLabel = CCLabelTTF::create(pLabelTTF->getString(), pLabelTTF->getFontName(), pLabelTTF->getFontSize());
					pLabel->setAnchorPoint(pLabelTTF->getAnchorPoint());
					pLabel->setPosition(pLabelTTF->getPosition());
					pSpriteSelected->addChild(pLabel);
				}
			}

			pSpriteSelected->setScale(fRate);
			pMenuItemImage->setSelectedImage(pSpriteSelected);
		}
	}
}

void ControlUtil::SetMenuItemSelectedImageWithNormalImageAndLabel( cocos2d::CCMenu* pMenu, float fRate /*= 1.1f */ )
{
	CCObject* pObj = NULL;
	CCARRAY_FOREACH(pMenu->getChildren(), pObj)
	{
		CCMenuItemImage* pMenuItem = dynamic_cast<CCMenuItemImage* >(pObj);
		SetMenuItemSelectedImageWithNormalImageAndLabel(pMenuItem, fRate);
	}
}

void ControlUtil::ShowLoadingWaitLayer()
{
//	WaitingLayer::shareWaitingLayer()->showProgress(NULL);
	LoadingSprit::ShowLoading(CCDirector::sharedDirector()->getRunningScene(), ccpMult(ccpFromSize(CCDirector::sharedDirector()->getWinSize()), 0.5f));
}

void ControlUtil::HideLoadingWaitLayer()
{
//	WaitingLayer::shareWaitingLayer()->closeProgress();
	LoadingSprit::HideLoading(CCDirector::sharedDirector()->getRunningScene());
}


//为 sprit 增加菜单事件监听。menuItemTag 是用来做标记的，事件回调时候可以知道是哪个菜单。touchArea 可以指定相对于 sprit 的任意区域
bool ControlUtil::AddEventListenToSprit( cocos2d::CCSprite* sprit ,CCObject *target, SEL_MenuHandler selector , int menuItemTag)
{
	if (sprit == NULL )
	{
		return false;
	}
	if (sprit->getChildByTag(menuItemTag) != NULL)
	{
		sprit->removeChildByTag(menuItemTag);
	}
    
	CCMenu* menuCombo = CCMenu::create();
	menuCombo->setTouchPriority(1);//设置触摸优先级，这样可以不会抢走 scrollview  的触摸优先级
    
	CCMenuItem* item = CCMenuItem::create(target,selector);
	menuCombo->addChild(item);
	item->setAnchorPoint(ccp(0,0));
	item->setPosition(ccp(0,0));
	item->setContentSize(sprit->getContentSize());
    
	item->setTag(menuItemTag);//记录这个 index
	menuCombo->setAnchorPoint(ccp(0,0));
	menuCombo->setPosition(ccp(0,0));
	menuCombo->setContentSize(sprit->getContentSize());
	menuCombo->setTag(menuItemTag);//记录这个 index
	sprit->addChild(menuCombo);
    
	return true;
}

bool ControlUtil::isButtonCanTouch(double dTimeIntervalMillionSecound)
{
    bool bRet = true;
    
    if (dTimeIntervalMillionSecound <= 0)
        dTimeIntervalMillionSecound = 1000;
    
    struct cc_timeval now;
    CCTime::gettimeofdayCocos2d(&now, NULL);
    
    if (g_sTimeVal.tv_sec == 0 && g_sTimeVal.tv_usec == 0)
    {
        g_sTimeVal = now;
        return bRet;
    }
    
    double dTimeStep = CCTime::timersubCocos2d(&g_sTimeVal, &now);
    
    CCLog("isButtonCanTouch dTimeStep:%.02f", dTimeStep);
    
    // 时间间隔大于设定的按钮按下的间隔
    bRet = dTimeStep >= dTimeIntervalMillionSecound;
    if (bRet)
    {
        g_sTimeVal = now;
    }
    
    return bRet;
}

cocos2d::CCPoint ControlUtil::getCenterPointWithNode(cocos2d::CCNode *pNode)
{
    return ccpMult(ccpFromSize(pNode->getContentSize()), 0.5f);
}

cocos2d::CCPoint ControlUtil::getBottomLeftByNode(cocos2d::CCNode *pNode)
{
    CCPoint ap = pNode->getAnchorPoint();
    CCPoint offset = ccp(-pNode->getContentSize().width*ap.x, -pNode->getContentSize().height*ap.y);
    
    return ccpAdd(pNode->getPosition(), offset);
}

cocos2d::CCPoint ControlUtil::getBottomRightByNode(cocos2d::CCNode *pNode)
{
    CCPoint ap = pNode->getAnchorPoint();
    CCPoint offset = ccp(pNode->getContentSize().width*ap.x, -pNode->getContentSize().height*ap.y);
    
    return ccpAdd(pNode->getPosition(), offset);
}

cocos2d::CCPoint ControlUtil::getTopLeftByNode(cocos2d::CCNode *pNode)
{
    CCPoint ap = pNode->getAnchorPoint();
    CCPoint offset = ccp(-pNode->getContentSize().width*ap.x, pNode->getContentSize().height*ap.y);
    
    return ccpAdd(pNode->getPosition(), offset);
}

cocos2d::CCPoint ControlUtil::getTopRightByNode(cocos2d::CCNode *pNode)
{
    CCPoint ap = pNode->getAnchorPoint();
    CCPoint offset = ccp(pNode->getContentSize().width*ap.x, pNode->getContentSize().height*ap.y);
    
    return ccpAdd(pNode->getPosition(), offset);
}
NS_BAILIN_UTIL_END

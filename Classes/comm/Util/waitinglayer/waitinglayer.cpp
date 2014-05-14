//
//  waitinglayer.cpp
//  KillBulls
//
//  Created by mac-zrb on 13-6-3.
//
//

#include "waitinglayer.h"
#include "ControlUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

WaitingLayer* g_waitingLayer = NULL ;

#define TAG_ZORDER_WAITINGLAYER  2000

bool WaitingLayer::init()
{
    
    
    bool bRet = false;
    
    do {
        CC_BREAK_IF(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 125)));
        setColor(ccc3(0, 0, 0));
        setOpacity(0);
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        setAnchorPoint(CCPointMake(0.5, 0.5));
        CCSprite* sprite = CCSprite::create("ccbResources/common/loading_bg.png");
        if (sprite) {
            CCSize spritesize = sprite->getContentSize();
            setContentSize(spritesize);
            sprite->setAnchorPoint(CCPointZero);
            sprite->setPosition(CCPointZero);
            addChild(sprite);
        }
       // setContentSize(CCSizeMake(200, 200));
        float rate =  CCDirector::sharedDirector()->getContentScaleFactor();
        m_pLabelTitle = CCLabelTTF::create("", "", 28);
        
//         std::string strRequst = EasyUseApi::getUTF8StringInDictionary("common", "requesting");
//         LabelStringSetWithFormat(m_pLabelTitle, "%s", strRequst.c_str())    ;

		m_pLabelTitle->setString("Please wait...");
		ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTitle);

        CCSize contentSize = getContentSize();
        setPosition(ccp(size.width/2-contentSize.width/2, size.height/2-contentSize.height/2));
        m_bShowing = false;
        m_pLabelTitle->setPosition(ccp(0.5, 0.5));
        m_pLabelTitle->setPosition(ccp(contentSize.width/2, (contentSize.height/2)*0.65f));
        this->addChild(m_pLabelTitle);
        bRet = true;
        
        createSprite();
    } while (0);
    
    return bRet;
    //	pushMenu(okMenuItem);
    //	pushMenu(cancelMenuItem);
    
   
//    SetVisibleBoard(m_sprite);
    return true;
}

void WaitingLayer::createSprite()
{
    float rate =  CCDirector::sharedDirector()->getContentScaleFactor();
    //创建第一帧
    m_sprite = CCSprite::create("ccbResources/common/loading.png");
    if (m_sprite)
    {
        CCSize size = getContentSize();
        CCSize contentSize = m_sprite->getContentSize()   ;
        //设置位置，要与上面的CCRect对牢。
        m_sprite->setAnchorPoint(ccp(0.5f, 0.5f));
        m_sprite->setPosition(ccp(size.width/2, size.height/2*1.2f));
        this->addChild(m_sprite);
        m_sprite->retain();
    }
}

bool WaitingLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	//CCLOG("waitingLayer::ccTouchBegan");
	// 吞掉消息
    return true;
}

void WaitingLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//CCLOG("waitingLayer::ccTouchMoved");
}

void WaitingLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//CCLOG("waitingLayer::ccTouchEnded");
    closeProgress();
}

void WaitingLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//	CCLOG("waitingLayer::ccTouchCancelled");
     closeProgress();
}

void WaitingLayer::destroySprite()
{
//    if (m_sprite)
//    {
//
//        m_sprite->stopAllActions();
//        if (m_sprite->getParent())
//        {
//             m_sprite->removeFromParentAndCleanup(true);
//        }
//        
//        m_sprite->release();
//        m_sprite = NULL;
//    }
//    m_bShowing = false;
//    CCLOG("destroySprite");
}

WaitingLayer::~WaitingLayer()
{
    
}

void WaitingLayer::onEnter()
{
    CCLayerColor::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 1, true);
}

void WaitingLayer::onExit()
{
//    if (m_sprite)
//    {
//        m_sprite->release();
//        m_sprite = NULL;
//    }
    CCLayerColor::onExit();
	// 自己注销
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

WaitingLayer*  WaitingLayer::shareWaitingLayer()
{
    if (g_waitingLayer == NULL)
    {
        g_waitingLayer = WaitingLayer::create();
        g_waitingLayer->retain();
    }
    
    return g_waitingLayer;
}

void WaitingLayer::addProgressToScene(float dt)
{
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(WaitingLayer::addProgressToScene), this);
    CCScene* pScene = CCDirector::sharedDirector()->getRunningScene();
    if (m_title.length()<=0 )
    {//默认
        // m_title = EasyUseApi::getUTF8StringInDictionary("common", "requesting");
		m_title = "Please Wait...";
    }

    if (m_bShowing)
    {
        //已经显示  更新文字
        if (m_pLabelTitle && m_title.length() > 0)
        {
            m_pLabelTitle->setString(m_title.c_str());
        }
        
        if (m_lAutoCloseTime > 0)
        {
            CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(WaitingLayer::autoCloseProgress), this);
            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(WaitingLayer::autoCloseProgress), this, m_lAutoCloseTime, false);
        }
        return;
    }

    if (pScene)
    {
        if (m_pLabelTitle && m_title.length()>0)
        {
            m_pLabelTitle->setString(m_title.c_str());
        }
        startAnimation();
        m_bShowing = true;
        //已经加到场景里
        if (getParent() == pScene && pScene)
        {
            
        }  //不是同个场景
        else if(getParent())
        {
            removeFromParent();
            pScene->addChild(this,TAG_ZORDER_WAITINGLAYER);
        }
        else
            pScene->addChild(this,TAG_ZORDER_WAITINGLAYER);
    }
    else
    {
        
    }
    
    if (m_lAutoCloseTime>0)
    {
        
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(WaitingLayer::autoCloseProgress), this, m_lAutoCloseTime, false);
    }
}
void WaitingLayer::showProgress( std::string title, float lAutoCloseTime, bool bChangeScene )
{
    m_lAutoCloseTime = lAutoCloseTime;
    m_title = title;
    if (bChangeScene)
    {//切换场景 
        CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(WaitingLayer::addProgressToScene), this);
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(WaitingLayer::addProgressToScene), this, 0.001f, false);
    }
    else
        addProgressToScene(0);
   
}

void  WaitingLayer::showProgress(CCString* pTitle ,float lAutoCloseTime)
{
 
    if (pTitle)
    {
          showProgress(pTitle->getCString(),lAutoCloseTime,false);
    }
    else
        // showProgress(EasyUseApi::getUTF8StringInDictionary("common", "requesting"));
		showProgress("Please wait...");
}
void WaitingLayer::autoCloseProgress(float dt)
{
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(WaitingLayer::autoCloseProgress), this);
    
    closeProgress();
}
void WaitingLayer::startAnimation()
{
    CCRotateBy * rotateAction = CCRotateBy::create(1.0f/25, 45/2.0f);
    //用数组frames创建CCAnimation
  //  CCAnimation* action = CCAnimation::createWithSpriteFrames(frames,0.1f);
    //运行动画pFo
    CCAction* pAction = CCRepeatForever::create(rotateAction);
    if (m_sprite)
    {
         m_sprite->runAction(pAction);
    }
    
  
}

void WaitingLayer::stopAnimation()
{
  
    if (m_sprite)
    {
        
        m_sprite->stopAllActions();
        
    }
    m_bShowing = false;
}

void  WaitingLayer::closeProgress()
{
    if (!m_bShowing)
    {
        return;
    }
    stopAnimation();
    if (getParent())
    {
        this->removeFromParent();
    }
    m_bShowing = false;
}

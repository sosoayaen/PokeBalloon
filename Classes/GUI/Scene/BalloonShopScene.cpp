// =====================================================================================
//
//       Filename:  BalloonShopScene.cpp
//
//    Description:  商店的实现文件
//
//        Version:  1.0
//        Created:  07/27/2014 02:10:48
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Jason Tou (), sosoayaen@gmail.com
//   Organization:  
//
// =====================================================================================

#include "BalloonShopScene.h"
#include "bailinUtil.h"
#include "NDKBridge.h"
#include "Balloon_macro.h"
#include "UserData.h"
#include "BalloonSoundManager.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

BalloonShopScene::BalloonShopScene()
{
    m_pArrayItemData = NULL;
    m_pTableViewItems = NULL;
    
    m_pSpriteCoin = NULL;
    m_pLabelBMFontCoins = NULL;
}

BalloonShopScene::~BalloonShopScene()
{
    // 释放内存
    CC_SAFE_RELEASE(m_pArrayItemData);
}

CCScene* BalloonShopScene::scene()
{
    CCScene* pScene = CCScene::create();
    
    BalloonShopScene* pChild = BalloonShopScene::create();
    
    pScene->addChild(pChild);
    
    return pScene;
}

bool BalloonShopScene::init()
{
	bool bRet = false;

	do
	{
		CC_BREAK_IF(!CCLayer::init());
        
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/common/common.plist");
        // 创建列表View
        m_pTableViewItems = CCTableView::create(this, CCSizeMake(getContentSize().width, getContentSize().height*0.7f));
        m_pTableViewItems->setPosition(ccp(0, getContentSize().height*0.15f));
        m_pTableViewItems->setDelegate(this);
        m_pTableViewItems->setDirection(kCCScrollViewDirectionHorizontal);
        m_pTableViewItems->setVerticalFillOrder(kCCTableViewFillTopDown);
        // 不能接收触摸消息，完全由本层控制
        // m_pTableViewItems->setTouchEnabled(false);
        addChild(m_pTableViewItems);
        
        bRet = true;

	} while (0);

	return bRet;
}

void BalloonShopScene::onEnter()
{
	CCLayer::onEnter();

    updateCoins();
    
	// 启动一个定时器，看苹果商店的数据是否到达，如果到达了就刷新表格
    schedule(schedule_selector(BalloonShopScene::checkProduct), 0.1f);
}

void BalloonShopScene::onExit()
{
	CCLayer::onExit();
}

void BalloonShopScene::checkProduct(float dt)
{
	CCArray* pArrayData = dynamic_cast<CCArray*>(DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataObject("IAP_ProductArray"));
    if (pArrayData)
    {
        // 停止定时器
        unschedule(schedule_selector(BalloonShopScene::checkProduct));
        
        CC_SAFE_RELEASE(m_pArrayItemData);
        m_pArrayItemData = pArrayData;
        CC_SAFE_RETAIN(m_pArrayItemData);
        
        m_pTableViewItems->reloadData();
    }
}

void BalloonShopScene::checkPurchaseStatus(float dt)
{
    long status = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS);
    
    switch (status)
    {
        case IAP_PURCHASE_PURCHASING:
            // 购买中就要不停扫描
            break;
        case IAP_PURCHASE_PURCHASED:
            // 购买成功，结束定时器
            unschedule(schedule_selector(BalloonShopScene::checkPurchaseStatus));
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_NONE);
            // 刷新下数据
            updateCoins();
            BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
            break;
        case IAP_PURCHASE_FAILED:
            // 购买失败，也结束定时器，顺便把全局变量重置
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_NONE);
            unschedule(schedule_selector(BalloonShopScene::checkPurchaseStatus));
            break;
        default:
            // 其他不知名的情况也停止定时器
            unschedule(schedule_selector(BalloonShopScene::checkPurchaseStatus));
            break;
    }
}

void BalloonShopScene::scrollViewDidScroll(cocos2d::extension::CCScrollView *view)
{
    
}

void BalloonShopScene::scrollViewDidZoom(cocos2d::extension::CCScrollView *view)
{
    
}

void BalloonShopScene::tableCellTouched(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    unsigned int idx = cell->getIdx();
    
    // 避免重复按下
    if (!ControlUtil::sharedControlUtil()->isButtonCanTouch(1000.0)) return;
    
    CCDictionary* pDict = dynamic_cast<CCDictionary*>(m_pArrayItemData->objectAtIndex(idx));
    if (pDict)
    {
        NDKBridge::sharedNDKBridge()->buyIAPProduct(idx);
        // 启动定时器检查是否购买成功
        schedule(schedule_selector(BalloonShopScene::checkPurchaseStatus), 0.1f);
    }
}

CCTableViewCell* BalloonShopScene::tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx)
{
    CCTableViewCell* pCell = table->dequeueCell();
    
    if (!pCell)
    {
        pCell = new CCTableViewCell;
        pCell->autorelease();
    }
    
    if (pCell)
    {
        pCell->removeAllChildren();
        
        CCSize cellSize = cellSizeForTable(table);
        
        // 先创建一个背景
        CCScale9Sprite* pSpriteBG = CCScale9Sprite::create("texture/handBook/scale9frame.png");
        pSpriteBG->setPreferredSize(CCSizeMake(cellSize.width*0.9f, cellSize.height));
        pSpriteBG->setPosition(ccpMult(ccpFromSize(cellSize), 0.5f));
        pCell->addChild(pSpriteBG);
        
        CCSize bgSize = pSpriteBG->getContentSize();
        
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(m_pArrayItemData->objectAtIndex(idx));
        if (pDict)
        {
            CCLabelTTF* pLabelTTFTitle = CCLabelTTF::create(pDict->valueForKey("title")->getCString(), "", 30);
            pLabelTTFTitle->setPosition(ccp(bgSize.width*0.5f, bgSize.height*0.75f));
            CCLabelTTF* pLabelTTFDesc = CCLabelTTF::create(pDict->valueForKey("description")->getCString(), "", 20);
            pLabelTTFDesc->setPosition(ccp(bgSize.width*0.5f, bgSize.height*0.5f));
            CCLabelTTF* pLabelTTFPrice = CCLabelTTF::create(pDict->valueForKey("price")->getCString(), "", 30);
            pLabelTTFPrice->setPosition(ccp(bgSize.width*0.5f, bgSize.height*0.25f));
            
            pSpriteBG->addChild(pLabelTTFTitle);
            pSpriteBG->addChild(pLabelTTFDesc);
            pSpriteBG->addChild(pLabelTTFPrice);
        }
    }
    return pCell;
}

CCSize BalloonShopScene::cellSizeForTable(cocos2d::extension::CCTableView *table)
{
    return ccp(getContentSize().width, getContentSize().height*0.7f);
}

unsigned int BalloonShopScene::numberOfCellsInTableView(cocos2d::extension::CCTableView *table)
{
    unsigned int nRet = 0;
    if (m_pArrayItemData)
        nRet = m_pArrayItemData->count();
    return nRet;
}

void BalloonShopScene::updateCoins()
{
    if (!m_pSpriteCoin)
    {
        m_pSpriteCoin = CCSprite::createWithSpriteFrameName("common_coin.png");
        m_pSpriteCoin->setAnchorPoint(ccp(1.0f, 0.5f));
        addChild(m_pSpriteCoin);
    }
    
    if (!m_pLabelBMFontCoins)
    {
        m_pLabelBMFontCoins = CCLabelBMFont::create("", "texture/fonts/font.fnt");
        m_pLabelBMFontCoins->setAnchorPoint(ccp(1.0f, 0.5f));
        m_pLabelBMFontCoins->setScale(4.0f);
        m_pLabelBMFontCoins->setColor(ccYELLOW);
        m_pLabelBMFontCoins->setPosition(ccp(getContentSize().width*0.99f, m_pSpriteCoin->boundingBox().size.height));
        addChild(m_pLabelBMFontCoins);
    }
    
    m_pLabelBMFontCoins->setString(CCString::createWithFormat("%lld", UserDataManager::sharedUserDataManager()->getGoldenCoins())->getCString());
    m_pSpriteCoin->setPosition(ccp(m_pLabelBMFontCoins->getPositionX() - m_pLabelBMFontCoins->boundingBox().size.width, m_pLabelBMFontCoins->getPositionY()));
}
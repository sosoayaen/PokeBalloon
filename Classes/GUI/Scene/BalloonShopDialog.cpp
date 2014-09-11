#include "BalloonShopDialog.h"
#include "bailinUtil.h"
#include "UserData.h"
#include "BalloonSoundManager.h"
#include "NDKBridge.h"
#include <string>

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define TAG_ID_ITEM_ITEM_SPRITE 100
#define TAG_ID_ITEM_BANNER_SPRITE 101
#define TAG_ID_ITEM_PRICE_LABEL 200
#define TAG_ID_ITEM_TITLE_LABEL 201
#define TAG_ID_ITEM_DESC_LABEL 202

BalloonShopDialog::~BalloonShopDialog()
{
    CC_SAFE_RELEASE(m_pArrayItemData);
}

bool BalloonShopDialog::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!DialogLayer::init());
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		std::string strCCBFileName = "BalloonShopDialog.ccbi";

		const std::vector<std::string> vSearchOrder = CCFileUtils::sharedFileUtils()->getSearchResolutionsOrder();

		std::string strCCBRootPath = vSearchOrder[0];

		pCCBReader->setCCBRootPath(strCCBRootPath.c_str());

		CCNode* pNode = pCCBReader->readNodeGraphFromFile(strCCBFileName.c_str(), this);

		if (pNode != NULL)
		{
			this->addChild(pNode);
		}

		pCCBReader->release();
		
		setKeypadEnabled(true);

		initLabelTTF();
        
        initSysMenu();
        
        initTableView();
        
        // 隐藏模板
        m_pSpriteItemTemplate->setVisible(false);
        
        // 设定主面板
        m_pMainBoard = m_pSpriteBoard;
        m_pSpriteBoard->setScale(0.01f);
        setOnEnterAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
        setOnExitAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3f, 0.1f)));
        
        // 注册按钮和表的触摸消息
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonShopDialog::onEnter()
{
	DialogLayer::onEnter();
	
    updateCoins();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	// 启动一个定时器，看苹果商店的数据是否到达，如果到达了就刷新表格
    schedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckProduct), 0.1f);
#endif
}

void BalloonShopDialog::onExit()
{
	// TODO: clean the scene
	
	DialogLayer::onExit();
}

bool BalloonShopDialog::initLabelTTF()
{
	// init labelTTF with original size in ccb's description
    do
    {
        CC_BREAK_IF(!m_pSpriteItemTemplate);
        
        CCObject* pObj = NULL;
        CCARRAY_FOREACH(m_pSpriteItemTemplate->getChildren(), pObj)
        {
            CCLabelTTF* pLabel = dynamic_cast<CCLabelTTF*>(pObj);
            if (pLabel)
            {
                ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(pLabel);
            }
        }
    } while (0);
    
	// your init code here
	
	return true;
}

SEL_CallFuncN BalloonShopDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonShopDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuShare", BalloonShopDialog::onPressMenuShare);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuClose", BalloonShopDialog::onPressMenuClose);

	return NULL;
}

bool BalloonShopDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerTableViewContainer", CCLayer*, this->m_pLayerTableViewContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTop", CCMenu*, this->m_pMenuTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteContainer", CCSprite*, this->m_pSpriteContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteItemTemplate", CCSprite*, this->m_pSpriteItemTemplate);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBoard", CCSprite*, this->m_pSpriteBoard);

	return true;
}

SEL_CCControlHandler BalloonShopDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonShopDialog::onPressMenuShare(CCObject* pSender)
{
	// TODO:
}

void BalloonShopDialog::onPressMenuClose(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    endDialog();
}

void BalloonShopDialog::keyBackClicked( void )
{
    endDialog();
}

void BalloonShopDialog::keyMenuClicked( void )
{
    
}


void BalloonShopDialog::scrollViewDidScroll( CCScrollView* view )
{

}

void BalloonShopDialog::scrollViewDidZoom( CCScrollView* view )
{

}

void BalloonShopDialog::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
    unsigned int idx = cell->getIdx();
    
    // 避免重复按下
    if (!ControlUtil::sharedControlUtil()->isButtonCanTouch(1000.0)) return;
    
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
    
    do
    {
        CC_BREAK_IF(!m_pArrayItemData);
        
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(m_pArrayItemData->objectAtIndex(idx));
        if (pDict)
        {
            NDKBridge::sharedNDKBridge()->buyIAPProduct(idx);
            // 启动定时器检查是否购买成功
            schedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckPurchaseStatus), 1.0f);
        }
    } while (0);
}

CCSize BalloonShopDialog::cellSizeForTable( CCTableView *table )
{
	// Return a CCSize with the item size you want to show
    return table->getViewSize();
}

CCTableViewCell* BalloonShopDialog::tableCellAtIndex( CCTableView *table, unsigned int idx )
{
	CCTableViewCell* pCell = table->dequeueCell();
	
	bool bCreateCell = false;
	
	if (!pCell)
	{
		pCell = new CCTableViewCell();
		pCell->autorelease();
		
		bCreateCell = true;
	}
	
	if (pCell)
	{
        pCell->removeAllChildren();
        
        CCSize cellSize = cellSizeForTable(table);
        
        // 先创建一个背景
        /*
        CCScale9Sprite* pSpriteBG = CCScale9Sprite::create("texture/handBook/scale9frame.png");
        pSpriteBG->setPreferredSize(CCSizeMake(cellSize.width*0.9f, cellSize.height));
        */
        CCSprite* pSpriteBG = CCSprite::createWithSpriteFrame(m_pSpriteItemTemplate->displayFrame());
        pSpriteBG->setPosition(ccpMult(ccpFromSize(cellSize), 0.5f));
        pCell->addChild(pSpriteBG);
        
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(m_pArrayItemData->objectAtIndex(idx));
        if (pDict)
        {
            // 底图
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCSprite* pItem = dynamic_cast<CCSprite*>(m_pSpriteItemTemplate->getChildByTag(TAG_ID_ITEM_ITEM_SPRITE));
            CCSprite* pSpriteItem = NULL;
            std::string strProduct = pDict->valueForKey("productId")->getCString();
            if (strProduct.find("removead") != std::string::npos)
            {
                pSpriteItem = CCSprite::createWithSpriteFrame(pItem->displayFrame());
            }
            else if (strProduct.find("buyCoinsOne") != std::string::npos)
            {
                pSpriteItem = CCSprite::createWithSpriteFrameName("balloon_coin.png");
            }
            else if (strProduct.find("buyCoinsTwo") != std::string::npos)
            {
                pSpriteItem = CCSprite::createWithSpriteFrameName("shop_package.png");
            }
            else if (strProduct.find("buyCoinsThree") != std::string::npos)
            {
                pSpriteItem = CCSprite::createWithSpriteFrameName("shop_chest.png");
            }
            if (pSpriteItem)
            {
                pSpriteItem->setPosition(pItem->getPosition());
                pSpriteBG->addChild(pSpriteItem);
            }
#endif
            
            // 价格条
            CCSprite* pBanner = dynamic_cast<CCSprite*>(m_pSpriteItemTemplate->getChildByTag(TAG_ID_ITEM_BANNER_SPRITE));
            if (pBanner)
            {
                CCSprite* pSpriteBanner = CCSprite::createWithSpriteFrame(pBanner->displayFrame());
                pSpriteBanner->setAnchorPoint(pBanner->getAnchorPoint());
                pSpriteBanner->setPosition(pBanner->getPosition());
                pSpriteBG->addChild(pSpriteBanner);
            }
        
            // 标题
            CCLabelTTF* pLabelTitle = dynamic_cast<CCLabelTTF*>(m_pSpriteItemTemplate->getChildByTag(TAG_ID_ITEM_TITLE_LABEL));
            if (pLabelTitle)
            {
                CCLabelTTF* pLabelTTFTitle = CCLabelTTF::create(pDict->valueForKey("title")->getCString(), "", pLabelTitle->getFontSize());
                pLabelTTFTitle->setPosition(pLabelTitle->getPosition());
                pSpriteBG->addChild(pLabelTTFTitle);
            }
            
            // 描述
            CCLabelTTF* pLabelDesc = dynamic_cast<CCLabelTTF*>(m_pSpriteItemTemplate->getChildByTag(TAG_ID_ITEM_DESC_LABEL));
            if (pLabelDesc)
            {
                CCLabelTTF* pLabelTTFDesc = CCLabelTTF::create(pDict->valueForKey("description")->getCString(), "", pLabelDesc->getFontSize());
                pLabelTTFDesc->setPosition(pLabelDesc->getPosition());
                pLabelTTFDesc->setDimensions(pLabelDesc->getDimensions());
                pLabelTTFDesc->setVerticalAlignment(pLabelDesc->getVerticalAlignment());
                pLabelTTFDesc->setHorizontalAlignment(pLabelDesc->getHorizontalAlignment());
                pSpriteBG->addChild(pLabelTTFDesc);
            }
            
            // 价格
            CCLabelTTF* pLabelPrice = dynamic_cast<CCLabelTTF*>(m_pSpriteItemTemplate->getChildByTag(TAG_ID_ITEM_PRICE_LABEL));
            if (pLabelPrice)
            {
                CCLabelTTF* pLabelTTFPrice = CCLabelTTF::create(pDict->valueForKey("price")->getCString(), "", pLabelPrice->getFontSize());
                pLabelTTFPrice->setPosition(pLabelPrice->getPosition());
                pLabelTTFPrice->setRotation(pLabelPrice->getRotation());
                pSpriteBG->addChild(pLabelTTFPrice);
            }
        }
	}
	return pCell;
}

unsigned int BalloonShopDialog::numberOfCellsInTableView( CCTableView *table )
{
    unsigned int nRet = 0;
    if (m_pArrayItemData)
        nRet = m_pArrayItemData->count();
    return nRet;
}

void BalloonShopDialog::initTableView()
{
    do
    {
        CC_BREAK_IF(m_pTableView);
        
        m_pTableView = CCTableView::create(this, m_pLayerTableViewContainer->getContentSize());
        m_pTableView->setDirection(kCCScrollViewDirectionHorizontal);
        m_pTableView->setDelegate(this);
        m_pLayerTableViewContainer->addChild(m_pTableView);
        
        pushScrollView(m_pTableView);
        
    } while (0);
}

void BalloonShopDialog::updateCoins()
{
    m_pLabelBMFontCoins->setString(CCString::createWithFormat("%lld", UserDataManager::sharedUserDataManager()->getGoldenCoins())->getCString());
    if (m_pSpriteBoard->boundingBox().size.width*0.25f < m_pLabelBMFontCoins->boundingBox().size.width)
        m_pLabelBMFontCoins->setScale(m_pSpriteBoard->getContentSize().width*0.25f/m_pLabelBMFontCoins->boundingBox().size.width);
    else
        m_pLabelBMFontCoins->setScale(2.0f);
}

void BalloonShopDialog::scheduleCallbackCheckProduct(float dt)
{
	CCArray* pArrayData = dynamic_cast<CCArray*>(DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataObject("IAP_ProductArray"));
    if (pArrayData)
    {
        // 停止定时器
        unschedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckProduct));
        
        CC_SAFE_RELEASE(m_pArrayItemData);
        m_pArrayItemData = pArrayData;
        CC_SAFE_RETAIN(m_pArrayItemData);
        
        m_pTableView->reloadData();
    }
}

void BalloonShopDialog::scheduleCallbackCheckPurchaseStatus(float dt)
{
    long status = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS);
    
    switch (status)
    {
        case IAP_PURCHASE_PURCHASING:
            // 购买中就要不停扫描
            break;
        case IAP_PURCHASE_PURCHASED:
            // 购买成功，结束定时器
            unschedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckPurchaseStatus));
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_NONE);
            // 刷新下数据
            updateCoins();
            BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
            break;
        case IAP_PURCHASE_FAILED:
            // 购买失败，也结束定时器，顺便把全局变量重置
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_NONE);
            unschedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckPurchaseStatus));
            break;
        default:
            // 其他不知名的情况也停止定时器
            unschedule(schedule_selector(BalloonShopDialog::scheduleCallbackCheckPurchaseStatus));
            break;
    }
}

void BalloonShopDialog::initSysMenu()
{
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuTop);
    // 让按钮动起来
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(m_pMenuTop->getChildren(), pObj)
    {
        CCMenuItem* pMenuItem = dynamic_cast<CCMenuItem*>(pObj);
        if (pMenuItem)
        {
            CCRotateTo* pActionRotate1 = CCRotateTo::create(1.0f, 15);
            CCRotateTo* pActionRotate2 = CCRotateTo::create(1.0f, 0);
            CCRotateTo* pActionRotate3 = CCRotateTo::create(1.0f, -15);
            CCRotateTo* pActionRotate4 = CCRotateTo::create(1.0f, 0);
            float delayTime = rand()%30/10.0f + 1.5f;
            pMenuItem->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(delayTime), pActionRotate1, CCDelayTime::create(delayTime), pActionRotate2, CCDelayTime::create(delayTime), pActionRotate3, CCDelayTime::create(delayTime), pActionRotate4, NULL)));
        }
    }
    
    // 移除分享菜单
    m_pMenuTop->getChildren()->removeLastObject();
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuTop);
    
    pushMenu(m_pMenuTop);
}
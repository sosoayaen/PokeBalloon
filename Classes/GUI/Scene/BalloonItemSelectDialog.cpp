#include "BalloonItemSelectDialog.h"
#include "bailinUtil.h"
#include "UserData.h"
#include "BalloonSoundManager.h"
#include "BalloonMission.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define TAG_ID_ITEM_ICON_BACKGROUND_SPRITE  1000
#define TAG_ID_ITEM_CNT_LABEL 100
#define MISSION_REFRESH_COST 1

BalloonItemSelectDialog::~BalloonItemSelectDialog()
{
    CC_SAFE_DELETE(m_pArrayItems);
    // CC_SAFE_DELETE(m_pDictExtItemConfig);
}

bool BalloonItemSelectDialog::init()
{
	// TODO:
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!DialogLayer::init());
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonItemSelectDialog.ccbi";

		// 得到第一个搜索路径
		const std::vector<std::string> vSearchOrder = CCFileUtils::sharedFileUtils()->getSearchResolutionsOrder();

		// 本程序中是对应的第一个元素即为对应的资源路径
		std::string strCCBRootPath = vSearchOrder[0];

		// 设置CCB的资源路径
		pCCBReader->setCCBRootPath(strCCBRootPath.c_str());

		CCNode* pNode = pCCBReader->readNodeGraphFromFile(strCCBFileName.c_str(), this);

		if (pNode != NULL)
		{
			this->addChild(pNode);
		}

		pCCBReader->release();
        
        // 界面需要点击按钮才能取消
        // SetVisibleBoard(m_pSpriteBoard);
        
        // 初始化字体
        initLabelTTFFont();
        
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuTop);
        pushMenu(m_pMenuTop);
        
        initMissionRefreshMenu();
        
        // 设置金币数量的位置
        updateCoins();
        
        // 初始化道具内容
        CCDictionary* pDictExtItemConfig = CCDictionary::createWithContentsOfFile("configuration/buyExtendItems.plist");
        CCAssert(pDictExtItemConfig, "configruation/buyExtendItems.plist is incorrect!!");
        
        // 得到配置数据
        CCArray* pArrayConfig = dynamic_cast<CCArray*>(pDictExtItemConfig->objectForKey("items"));
        
        unsigned int nCnts = pArrayConfig->count();
        
        m_pArrayItems = CCArray::createWithCapacity(nCnts);
        CC_SAFE_RETAIN(m_pArrayItems);
        
        CCObject* pObj = NULL;
        CCARRAY_FOREACH(pArrayConfig, pObj)
        {
            CCDictionary* pDictData = dynamic_cast<CCDictionary*>(pObj);
            if (pDictData)
            {
                CCDictionary* pDict = CCDictionary::create();
                pDict->setObject(pDictData->objectForKey("iconSpriteName"), "iconSpriteName");
                pDict->setObject(pDictData->objectForKey("cost"), "cost");
                pDict->setObject(pDictData->objectForKey("type"), "type");
                const char* pszItemName = pDictData->valueForKey("name")->getCString();
                pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", CCString::createWithFormat("%sDesc", pszItemName)->getCString())), "desc");
                pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", CCString::createWithFormat("%sTitle", pszItemName)->getCString())), "title");
                m_pArrayItems->addObject(pDict);
            }
        }
        
        // 创建表格
        CCSize viewSize = m_pLayerTableView->getContentSize();
        m_pTableView = CCTableView::create(this, viewSize);
        m_pTableView->setDelegate(this);
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
        m_pLayerTableView->addChild(m_pTableView);
        
        pushScrollView(m_pTableView);
        
        // 让按钮动起来
        pObj = NULL;
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
        
        // 设置进场和出场的动画
        m_pMainBoard = m_pSpriteBoard;
        m_pSpriteBoard->setScale(0.01f);
        setOnEnterAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
        setOnExitAction(CCEaseExponentialIn::create(CCScaleTo::create(0.5f, 0.01f)));
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonItemSelectDialog::onEnter()
{
	DialogLayer::onEnter();
    
    // 更新任务
    updateMission();
}

void BalloonItemSelectDialog::onExit()
{
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
    
	DialogLayer::onExit();
}

SEL_CallFuncN BalloonItemSelectDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonItemSelectDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuStart", BalloonItemSelectDialog::onPressMenuStart);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuReturnMain", BalloonItemSelectDialog::onPressMenuReturnMain);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuRefreshMission", BalloonItemSelectDialog::onPressMenuRefreshMission);
    
	return NULL;
}

bool BalloonItemSelectDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBoard", CCSprite*, this->m_pSpriteBoard);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteContainer", CCSprite*, this->m_pSpriteContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteMiddleBar", CCSprite*, this->m_pSpriteMiddleBar);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTop", CCMenu*, this->m_pMenuTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuRefreshMission", CCMenu*, this->m_pMenuRefreshMission);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerTableView", CCLayer*, this->m_pLayerTableView);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFMissionTitle", CCLabelTTF*, this->m_pLabelTTFMissionTitle);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFMissionDesc", CCLabelTTF*, this->m_pLabelTTFMissionDesc);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontReward", CCLabelBMFont*, this->m_pLabelBMFontReward);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteRewardCoin", CCSprite*, this->m_pSpriteRewardCoin);

	return true;
}

SEL_CCControlHandler BalloonItemSelectDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonItemSelectDialog::scrollViewDidScroll( CCScrollView* view )
{

}

void BalloonItemSelectDialog::scrollViewDidZoom( CCScrollView* view )
{

}

void BalloonItemSelectDialog::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
    unsigned int idx = cell->getIdx();
	CCLOG("cell touched at index: %i, table's children counts:%i", idx+1, numberOfCellsInTableView(NULL));
	
	if (cell != NULL)
	{
        // 按下后确定是否支付得起价格
        
        CCDictionary* pDict = (CCDictionary*) m_pArrayItems->objectAtIndex(idx);
        unsigned int nCost = pDict->valueForKey("cost")->uintValue();
        if (nCost > UserDataManager::sharedUserDataManager()->getGoldenCoins())
        {
            // 买不起
            CCMessageBox("You can't afford it!", "Information");
            return;
        }
        
        // 减去价格
        UserDataManager::sharedUserDataManager()->subGoldenCoins(nCost);
        updateCoins();
        
        // 播放花钱音效
        BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
        
        // 由类型对应取得道具的个数
        int nItemType = pDict->valueForKey("type")->intValue();
        long lItemsCounts = UserDataManager::sharedUserDataManager()->addItemExOneCountByID((ItemExType)nItemType);
        CCNode* pNode = cell->getChildByTag(TAG_ID_ITEM_ICON_BACKGROUND_SPRITE);
        if (pNode)
        {
            CCLabelBMFont* pLabelBMFontCnt = dynamic_cast<CCLabelBMFont*>(pNode->getChildByTag(TAG_ID_ITEM_CNT_LABEL));
            if (pLabelBMFontCnt)
            {
                // 设置右上角的个数
                pLabelBMFontCnt->setString(CCString::createWithFormat("%ld", lItemsCounts)->getCString());
            }
        }
	} // end if (cell != NULL)
}

CCSize BalloonItemSelectDialog::cellSizeForTable( CCTableView *table )
{
	// Return a CCSize with the item size you want to show
    CCSize size = CCSizeMake(table->getContentSize().width, table->getContentSize().width*0.25f);
	return size;
}

CCTableViewCell* BalloonItemSelectDialog::tableCellAtIndex( CCTableView *table, unsigned int idx )
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
        // 先删除监听的按钮
        pCell->removeAllChildren();
        
        CCDictionary* pDict = (CCDictionary*)m_pArrayItems->objectAtIndex(idx);
        CCSize cellSize = cellSizeForTable(table);
        // 设置背景
        CCSprite* pSprite = CCSprite::createWithSpriteFrameName("prepare_item_bg.png");
        pSprite->setPosition(ccp(cellSize.width * 0.1f, cellSize.height*0.5f));
        pSprite->setTag(TAG_ID_ITEM_ICON_BACKGROUND_SPRITE);
        
        // 设置道具图片
        CCSprite* pSpriteIcon = CCSprite::createWithSpriteFrameName(pDict->valueForKey("iconSpriteName")->getCString());
        pSpriteIcon->setPosition(ccpMult(ccpFromSize(pSprite->getContentSize()), 0.5f));
        pSpriteIcon->setScale(pSprite->getContentSize().height*0.7f/pSpriteIcon->getContentSize().height);
        pSprite->addChild(pSpriteIcon);
        pCell->addChild(pSprite);
        
        // 右左上角的数字
        ItemExType type = (ItemExType) (pDict->valueForKey("type")->intValue());
        long lCnts = UserDataManager::sharedUserDataManager()->getItemExCountsByID(type);
        CCLabelBMFont* pLBFLeftCnt = CCLabelBMFont::create(CCString::createWithFormat("%ld", lCnts)->getCString(), "texture/fonts/font.fnt");
        pLBFLeftCnt->setColor(ccYELLOW);
        pLBFLeftCnt->setTag(TAG_ID_ITEM_CNT_LABEL);
        pLBFLeftCnt->setPosition(ccpFromSize(pSprite->getContentSize()));
        pSprite->addChild(pLBFLeftCnt);
        
        // 设置描述文字
        CCLabelTTF* pLabelTTFTitle = CCLabelTTF::create(pDict->valueForKey("title")->getCString(), "", cellSize.height*0.25f);
        pLabelTTFTitle->setAnchorPoint(ccp(0.5f, 0));
        pLabelTTFTitle->setPosition(ccp(pSprite->getPositionX() + pSprite->getContentSize().width*0.5f + cellSize.width*0.6f*0.5f, cellSize.height*0.7f));
        pCell->addChild(pLabelTTFTitle);
        
        CCLabelTTF* pLabelTTFDesc = CCLabelTTF::create(pDict->valueForKey("desc")->getCString(), "", cellSize.height*0.165f);
        pLabelTTFDesc->setDimensions(CCSizeMake(cellSize.width*0.55f, cellSize.height*0.6f));
        pLabelTTFDesc->setHorizontalAlignment(kCCTextAlignmentLeft);
        pLabelTTFDesc->setVerticalAlignment(kCCVerticalTextAlignmentTop);
        pLabelTTFDesc->setAnchorPoint(ccp(0, 1.0f));
        pLabelTTFDesc->setPosition(ccp(pSprite->getPositionX() + pSprite->getContentSize().width*0.5f, pLabelTTFTitle->getPositionY()));
        pCell->addChild(pLabelTTFDesc);
        
        // 放置购买的按钮
        CCSprite* pSpriteBtnBuy = CCSprite::createWithSpriteFrameName("prepare_btn_item_buy.png");
        // 放置金币标志和金币数值
        CCSprite* pSpriteCoin = CCSprite::createWithSpriteFrame(m_pSpriteCoin->displayFrame());
        pSpriteCoin->setScale(pSpriteBtnBuy->getContentSize().height*0.6f/pSpriteCoin->getContentSize().height);
        pSpriteCoin->setPosition(ccp(pSpriteCoin->boundingBox().size.width, pSpriteBtnBuy->getContentSize().height*0.55f));
        pSpriteBtnBuy->addChild(pSpriteCoin);
        
        CCLabelBMFont* pBMFontValue = CCLabelBMFont::create(pDict->valueForKey("cost")->getCString(), "texture/fonts/font.fnt");
        pBMFontValue->setScale(1.5f);
        pBMFontValue->setAnchorPoint(ccp(0, 0.5f));
        pBMFontValue->setPosition(ccp(pSpriteCoin->getPositionX() + pSpriteCoin->boundingBox().size.width*0.6f, pSpriteBtnBuy->getContentSize().height*0.6f));
        pSpriteBtnBuy->addChild(pBMFontValue);
        
        pSpriteBtnBuy->setPosition(ccp(cellSize.width - pSpriteBtnBuy->getContentSize().width*0.5f, cellSize.height*0.5f));
        pCell->addChild(pSpriteBtnBuy);
        
	}
	return pCell;
}

unsigned int BalloonItemSelectDialog::numberOfCellsInTableView( CCTableView *table )
{
	// TODO: return the counts of TableView
	return m_pArrayItems->count();
}

void BalloonItemSelectDialog::updateCoins()
{
    m_pLabelBMFontCoins->setString(CCString::createWithFormat("%lld", UserDataManager::sharedUserDataManager()->getGoldenCoins())->getCString());
    
    if (m_pSpriteBoard->boundingBox().size.width*0.25f < m_pLabelBMFontCoins->boundingBox().size.width)
        m_pLabelBMFontCoins->setScale(m_pSpriteBoard->boundingBox().size.width*0.25f/m_pLabelBMFontCoins->boundingBox().size.width);
    else
        m_pLabelBMFontCoins->setScale(2.0f);
}

void BalloonItemSelectDialog::onPressMenuReturnMain(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    // 弹出到外层
    CCDirector::sharedDirector()->popScene();
}

void BalloonItemSelectDialog::onPressMenuStart(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    // 顺利结束对话框
    endDialog();
}

void BalloonItemSelectDialog::onPressMenuRefreshMission(cocos2d::CCObject* pSender)
{
    // 校验是否买得起
    if (UserDataManager::sharedUserDataManager()->getGoldenCoins() >= MISSION_REFRESH_COST)
    {
        UserDataManager::sharedUserDataManager()->addGoldenCoins(-MISSION_REFRESH_COST);
        
        BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
        
        // 更新随机任务
        updateMission(true);
        updateCoins();
    }
    else
    {
        // TODO: 修改成自己的对话框，可以做跳转等动作
        CCMessageBox("You need more golden coins!", "Tips");
    }
}

/*
void BalloonItemSelectDialog::onPressMenuBuyItem(cocos2d::CCObject *pSender)
{
    
}
*/

void BalloonItemSelectDialog::initMissionRefreshMenu()
{
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuRefreshMission);
    pushMenu(m_pMenuRefreshMission);
    
    // 添加个金币
    CCString* pStringCost = CCString::createWithFormat("%d", MISSION_REFRESH_COST);
    const char* pszMissionRefreshCost = pStringCost->getCString();
    
    CCMenuItemImage* pMenuItem = (CCMenuItemImage*)m_pMenuRefreshMission->getChildren()->lastObject();
    CCSprite* pSpriteNormal = (CCSprite*)pMenuItem->getNormalImage();
    CCSprite* pSpriteSelected = (CCSprite*)pMenuItem->getSelectedImage();
    
    // 增加到普通图标上
    CCSprite* pSpriteCoin = CCSprite::createWithSpriteFrameName("balloon_coin.png");
    pSpriteCoin->setScale(pSpriteNormal->getContentSize().height*0.7f/pSpriteCoin->getContentSize().height);
    pSpriteCoin->setPosition(ccp(pSpriteCoin->boundingBox().size.width, pSpriteNormal->getContentSize().height*0.55f));
    pSpriteNormal->addChild(pSpriteCoin);
    CCLabelBMFont* pBMFont = CCLabelBMFont::create(pszMissionRefreshCost, "texture/fonts/font.fnt");
    pBMFont->setPosition(ccp(pSpriteNormal->getContentSize().width*0.55f, pSpriteNormal->getContentSize().height*0.6f));
    pBMFont->setScale(1.5f);
    pSpriteNormal->addChild(pBMFont);
    
    // 添加到选中图标上
    pSpriteCoin = CCSprite::createWithSpriteFrameName("balloon_coin.png");
    pSpriteCoin->setScale(pSpriteSelected->getContentSize().height*0.7f/pSpriteCoin->getContentSize().height);
    pSpriteCoin->setPosition(ccp(pSpriteCoin->boundingBox().size.width, pSpriteSelected->getContentSize().height*0.55f));
    pSpriteSelected->addChild(pSpriteCoin);
    pBMFont = CCLabelBMFont::create(pszMissionRefreshCost, "texture/fonts/font.fnt");
    pBMFont->setPosition(ccp(pSpriteSelected->getContentSize().width*0.55f, pSpriteSelected->getContentSize().height*0.6f));
    pBMFont->setScale(1.5f);
    pSpriteSelected->addChild(pBMFont);
}

void BalloonItemSelectDialog::initLabelTTFFont()
{
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFMissionTitle);
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFMissionDesc);
}

void BalloonItemSelectDialog::updateMission(bool bNeedRefresh /* = false */ )
{
    if (bNeedRefresh)
    {
        // 设置生成一个新的随机任务
        BalloonMission::sharedBalloonMission()->setNeedNewMission(true);
    }
    // 得到任务
    const Mission* pMission = BalloonMission::sharedBalloonMission()->getRandomMission();
    if (pMission)
    {
        m_pLabelTTFMissionTitle->setString(pMission->strMissionName.c_str());
        m_pLabelTTFMissionDesc->setString(pMission->strMissionDesc.c_str());
        m_pLabelBMFontReward->setString(CCString::createWithFormat("%d", pMission->nReward)->getCString());
    }
}
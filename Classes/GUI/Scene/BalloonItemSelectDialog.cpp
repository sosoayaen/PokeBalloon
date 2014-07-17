#include "BalloonItemSelectDialog.h"
#include "bailinUtil.h"
#include "UserData.h"
#include "BalloonSoundManager.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define TAG_ID_ITEM_ICON_BACKGROUND_SPRITE  1000
#define TAG_ID_ITEM_CNT_LABEL 100

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
        
        SetVisibleBoard(m_pScale9SpriteBuyBoard);
        
        //*
        m_pLabelBMFontCoins->retain();
        m_pLabelBMFontCoins->removeFromParent();
        m_pScale9SpriteBuyBoard->addChild(m_pLabelBMFontCoins);
        m_pLabelBMFontCoins->release();
        m_pSpriteCoin->retain();
        m_pSpriteCoin->removeFromParent();
        m_pScale9SpriteBuyBoard->addChild(m_pSpriteCoin);
        m_pSpriteCoin->release();
        //*/
        
        updateCoins();
		
        // 设定头部的名称
        CCLabelBMFont* pBMFontTitle = CCLabelBMFont::create("Would you like buy Items?", "texture/fonts/font.fnt");
        pBMFontTitle->setPosition(ccp(m_pScale9SpriteBuyBoard->getContentSize().width*0.5f, m_pScale9SpriteBuyBoard->getContentSize().height - pBMFontTitle->getContentSize().height));
        m_pScale9SpriteBuyBoard->addChild(pBMFontTitle);
        
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
        /*
        CCDictionary* pDict = CCDictionary::create();
        pDict->setObject(ccs("item_pump.png"), "iconSpriteName");
        pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", "pumpDesc")), "desc");
        pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", "pumpTitle")), "title");
        pDict->setObject(ccs("10"), "cost");
        pDict->setObject(ccs("1"), "type");
        m_pArrayItems->addObject(pDict);
        pDict = CCDictionary::create();
        pDict->setObject(ccs("item_time.png"), "iconSpriteName");
        pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", "timeDesc")), "desc");
        pDict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("itemPreBuy_section", "timeTitle")), "title");
        pDict->setObject(ccs("5"), "cost");
        pDict->setObject(ccs("2"), "type");
        m_pArrayItems->addObject(pDict);
        //*/
        
        // 创建表格
        CCSize boardSize = m_pScale9SpriteBuyBoard->getContentSize();
        CCSize viewSize = CCSizeMake(boardSize.width*0.9f, boardSize.height*0.9f);
        m_pTableView = CCTableView::create(this, viewSize);
        m_pTableView->setDelegate(this);
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
        m_pTableView->setPosition(ccp((boardSize.width - viewSize.width)*0.5f, (boardSize.height - viewSize.height)*0.5f));
        
        m_pScale9SpriteBuyBoard->addChild(m_pTableView);
        
        pushScrollView(m_pTableView);
        
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonItemSelectDialog::onEnter()
{
	DialogLayer::onEnter();
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
}

void BalloonItemSelectDialog::onExit()
{
	DialogLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
}

SEL_CallFuncN BalloonItemSelectDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonItemSelectDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonItemSelectDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pScale9SpriteBuyBoard", CCScale9Sprite*, this->m_pScale9SpriteBuyBoard);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);

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
        pCell->removeAllChildren();
        
        CCDictionary* pDict = (CCDictionary*)m_pArrayItems->objectAtIndex(idx);
        CCSize cellSize = cellSizeForTable(table);
        // 设置背景
        CCSprite* pSprite = CCSprite::create("texture/items/item_background.png");
        pSprite->setPosition(ccp(cellSize.width * 0.1f, cellSize.height*0.5f));
        pSprite->setTag(TAG_ID_ITEM_ICON_BACKGROUND_SPRITE);
        
        // 设置道具图片
        CCSprite* pSpriteIcon = CCSprite::createWithSpriteFrameName(pDict->valueForKey("iconSpriteName")->getCString());
        pSpriteIcon->setPosition(ccpMult(ccpFromSize(pSprite->getContentSize()), 0.5f));
        pSpriteIcon->setScale(pSprite->getContentSize().height*0.7f/pSpriteIcon->getContentSize().height);
        pSprite->addChild(pSpriteIcon);
        pCell->addChild(pSprite);
        
        // 左上角的数字
        ItemExType type = (ItemExType) (pDict->valueForKey("type")->intValue());
        long lCnts = UserDataManager::sharedUserDataManager()->getItemExCountsByID(type);
        CCLabelBMFont* pLBFLeftCnt = CCLabelBMFont::create(CCString::createWithFormat("%ld", lCnts)->getCString(), "texture/fonts/font.fnt");
        pLBFLeftCnt->setColor(ccYELLOW);
        pLBFLeftCnt->setTag(TAG_ID_ITEM_CNT_LABEL);
        pLBFLeftCnt->setPosition(ccp(0, pSprite->getContentSize().height));
        pSprite->addChild(pLBFLeftCnt);
        
        // 设置描述文字
        CCLabelTTF* pLabelTTFTitle = CCLabelTTF::create(pDict->valueForKey("title")->getCString(), "", cellSize.height*0.25f);
        pLabelTTFTitle->setAnchorPoint(ccp(0, 0));
        pLabelTTFTitle->setPosition(ccp(pSprite->getPositionX() + pSprite->getContentSize().width*0.5f, cellSize.height*0.7f));
        pCell->addChild(pLabelTTFTitle);
        
        CCLabelTTF* pLabelTTFDesc = CCLabelTTF::create(pDict->valueForKey("desc")->getCString(), "", cellSize.height*0.165f);
        pLabelTTFDesc->setDimensions(CCSizeMake(cellSize.width*0.60f, cellSize.height*0.6f));
        pLabelTTFDesc->setHorizontalAlignment(kCCTextAlignmentLeft);
        pLabelTTFDesc->setVerticalAlignment(kCCVerticalTextAlignmentTop);
        pLabelTTFDesc->setAnchorPoint(ccp(0, 1.0f));
        pLabelTTFDesc->setPosition(ccp(pSprite->getPositionX() + pSprite->getContentSize().width*0.5f, pLabelTTFTitle->getPositionY()));
        pCell->addChild(pLabelTTFDesc);
        
        // 放置金币标志和金币数值
        CCSprite* pSpriteCoin = CCSprite::createWithSpriteFrame(m_pSpriteCoin->displayFrame());
        pSpriteCoin->setAnchorPoint(ccp(0, 0.5f));
        pSpriteCoin->setPosition(ccp(pLabelTTFDesc->getContentSize().width + pLabelTTFDesc->getPositionX(), cellSize.height*0.5f));
        pSpriteCoin->setScale(0.5f);
        pCell->addChild(pSpriteCoin);
        
        CCLabelBMFont* pBMFontValue = CCLabelBMFont::create(pDict->valueForKey("cost")->getCString(), "texture/fonts/font.fnt");
        pBMFontValue->setScale(1.5f);
        pBMFontValue->setAnchorPoint(ccp(0, 0.5f));
        pBMFontValue->setPosition(ccp(pSpriteCoin->getPositionX() + pSpriteCoin->boundingBox().size.width*1.15f, cellSize.height*0.5f));
        pCell->addChild(pBMFontValue);
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
    
    m_pSpriteCoin->setPositionX(m_pLabelBMFontCoins->getPositionX() - m_pLabelBMFontCoins->getContentSize().width*m_pLabelBMFontCoins->getScaleX());
}
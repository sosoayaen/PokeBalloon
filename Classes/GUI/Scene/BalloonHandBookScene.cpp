#include "BalloonHandBookScene.h"
#include "BalloonSoundManager.h"
#include "bailinUtil.h"
#include "UserData.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

BalloonHandBookScene::~BalloonHandBookScene()
{
    CC_SAFE_RELEASE_NULL(m_pDictHandbookData);
}

bool BalloonHandBookScene::init()
{
	bool bRet = false;
	do
	{
		// CC_BREAK_IF(!CCLayer::init());
		CC_BREAK_IF(!AutoTextureManagerLayer::init());
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonHandBookScene.ccbi";

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
		
        // CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/balloon/balloon.plist");
        // CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/items/items.plist");
        
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		// 拉伸底图
        m_pSpriteBackground->setScaleX(winSize.width/m_pSpriteBackground->getContentSize().width);
        m_pSpriteBackground->setScaleY(winSize.height/m_pSpriteBackground->getContentSize().height);
        
        // 设定图鉴的列表容器位置
        m_pLayerItemContainer->setPosition(ccp((winSize.width - m_pLayerItemContainer->getContentSize().width)*0.5f, (winSize.height - m_pLayerItemContainer->getContentSize().height)*0.5f));
		
        // 设定屏幕大笑
        m_winSize = CCDirector::sharedDirector()->getWinSize();
        
        // 初始化TableView
        initItemTableView();
        
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenu);
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonHandBookScene::onEnter()
{
	// CCLayer::onEnter();
    AutoTextureManagerLayer::onEnter();
	// 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    
    updateCoins();
    
    // 删除下未用到的纹理缓存
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void BalloonHandBookScene::onExit()
{
	// 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
    AutoTextureManagerLayer::onExit();
    
}

SEL_CallFuncN BalloonHandBookScene::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonHandBookScene::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuReturnMainBoard", BalloonHandBookScene::onPressMenuReturnMainBoard);
	return NULL;
}

bool BalloonHandBookScene::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerItemContainer", CCLayer*, this->m_pLayerItemContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenu", CCMenu*, this->m_pMenu);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);

	return true;
}

SEL_CCControlHandler BalloonHandBookScene::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonHandBookScene::scrollViewDidScroll( CCScrollView* view )
{

}

void BalloonHandBookScene::scrollViewDidZoom( CCScrollView* view )
{

}

void BalloonHandBookScene::tableCellHighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    unsigned int idx = cell->getIdx();
    CCNode* pNode = cell->getChildByTag(100);
    if (pNode)
        pNode->runAction(CCScaleTo::create(0.2f, 1.1f));
}

void BalloonHandBookScene::tableCellUnhighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    unsigned int idx = cell->getIdx();
    CCNode* pNode = cell->getChildByTag(100);
    if (pNode)
        pNode->runAction(CCScaleTo::create(0.2f, 1.0f));
}

void BalloonHandBookScene::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
    unsigned int idx = cell->getIdx();
	CCLOG("cell touched at index: %i, table's children counts:%i", idx + 1, numberOfCellsInTableView(NULL));
	
	if (cell != NULL)
	{
        CCArray* pArray = dynamic_cast<CCArray* >(m_pDictHandbookData->objectForKey("items"));
        if (pArray)
        {
            CCObject* pObj = pArray->objectAtIndex(idx);
            CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
            
            if (!pDict) return;
            
            bool bUnFold = pDict->valueForKey("unfold")->boolValue();
            
            // 记录当前的位置
            CCPoint beforeOffset = table->getContentOffset();
            CCSize beforeSize = tableCellSizeForIndex(table, idx);
            
            // 设置点击后的状态，更新表格数据显示
            pDict->setObject(bUnFold ? ccs("0") : ccs("1"), "unfold");
            table->reloadData();
            
            // 之前的偏移量
            CCSize afterSize = tableCellSizeForIndex(table, idx);
            
            // 偏移量
            float fOffsetY = afterSize.height - beforeSize.height;
            float fOffsetX = afterSize.width - beforeSize.width;
            
            // 修正后的偏移量
            CCPoint offset = beforeOffset;
            offset.y -= fOffsetY;
            offset.x -= fOffsetX;
            
            // 重新设置偏移位置
            table->setContentOffset(offset);
        }
	} // end if (cell != NULL)
}

//*
CCSize BalloonHandBookScene::cellSizeForTable( CCTableView *table )
{
	// Return a CCSize with the item size you want to show
	return CCSizeMake(m_winSize.width*0.8f, m_winSize.width*0.3f);
}
//*/

CCSize BalloonHandBookScene::tableCellSizeForIndex(cocos2d::extension::CCTableView *table, unsigned int idx)
{
    //*
    // 根据内容的状态设置是否伸展
    do
    {
        CC_BREAK_IF(!m_pDictHandbookData);
        
        CCArray* pArray = dynamic_cast<CCArray* >(m_pDictHandbookData->objectForKey("items"));
        if (pArray)
        {
            CC_BREAK_IF(idx >= pArray->count());
            
            CCObject* pObj = pArray->objectAtIndex(idx);
            CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
            
            CC_BREAK_IF(!pDict);
            
            // 判断打开收拢的状态
            bool bUnFold = pDict->valueForKey("unfold")->boolValue();
            if (bUnFold)
            {
                return CCSizeMake(m_winSize.width*0.8f, m_winSize.width*0.6f);
            }
        }
    } while (0);
    
    return CCSizeMake(m_winSize.width*0.8f, m_winSize.width*0.3f);
    //*/
}

CCTableViewCell* BalloonHandBookScene::tableCellAtIndex( CCTableView *table, unsigned int idx )
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
        // 清空内部数据
        pCell->removeAllChildren();
        // 单元大小
        CCSize cellSize = tableCellSizeForIndex(table, idx);
        // 创建单元背景
        CCScale9Sprite* pSpriteCellBackground = CCScale9Sprite::create("texture/handBook/handbook_background.png");
        pSpriteCellBackground->setTag(100);
        pSpriteCellBackground->setPreferredSize(cellSize);
        pSpriteCellBackground->setPosition(ccp(cellSize.width*0.5f, cellSize.height*0.5f));
        pCell->addChild(pSpriteCellBackground);
        
        CCArray* pArray = dynamic_cast<CCArray* >(m_pDictHandbookData->objectForKey("items"));
        CCObject* pObj = pArray->objectAtIndex(idx);
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
        if (pDict)
        {
            // 创建图形
            CCSprite* pSpriteBalloon = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("balloon_1_%d.png", (idx%8) + 1)->getCString());
            
            // 根据配置得到内部图片
            CCNode* pNode = NULL;
            const CCString* pStringType = pDict->valueForKey("displayType");
            switch (pStringType->intValue())
            {
                case 1: // 图片
                    pNode = CCSprite::createWithSpriteFrameName(pDict->valueForKey("frameName")->getCString());
                    break;
                case 2: // 文字
                    pNode = CCLabelBMFont::create(pDict->valueForKey("bmfValue")->getCString(), "texture/fonts/font.fnt");
                    pNode->setScale(pSpriteBalloon->getContentSize().width*0.5f/pNode->getContentSize().width);
                    break;
                default:
                    break;
            }
            
            if (pNode)
            {
                pNode->setAnchorPoint(ccp(0.5f, 0.5f));
                pNode->setPosition(ccpMult(ccpFromSize(pSpriteBalloon->getContentSize()), 0.5f));
                pSpriteBalloon->addChild(pNode);
            }
            
            pSpriteBalloon->setScale(cellSize.height*0.6f/pSpriteBalloon->getContentSize().height);
            pSpriteBalloon->setPosition(ccp(cellSize.width*0.155f, cellSize.height*0.5f));
            // pCell->addChild(pSpriteBalloon);
            pSpriteCellBackground->addChild(pSpriteBalloon);
            
            // 创建名称
            const char* pszTitle = DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("handbook_section", pDict->valueForKey("name")->getCString());
            CCLabelTTF* pLabelTTFTitle = CCLabelTTF::create(pszTitle, "", 30);
            pLabelTTFTitle->setPosition(ccp(cellSize.width*0.3f, cellSize.height - 30 - pLabelTTFTitle->getContentSize().height));
            pLabelTTFTitle->setAnchorPoint(CCPointZero);
            // pCell->addChild(pLabelTTFTitle);
            pSpriteCellBackground->addChild(pLabelTTFTitle);
            
            // 描述内容
            const char* pszDesc = DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("handbook_section", pDict->valueForKey("description")->getCString());
            CCLabelTTF* pLabelTTFDesc = CCLabelTTF::create(pszDesc, "", 20);
            pLabelTTFDesc->setPosition(ccp(cellSize.width*0.3f, 30));
            pLabelTTFDesc->setAnchorPoint(CCPointZero);
            pLabelTTFDesc->setDimensions(CCSizeMake(cellSize.width*0.65f, (cellSize.height-30*2-pLabelTTFTitle->getContentSize().height)));
            pLabelTTFDesc->setHorizontalAlignment(kCCTextAlignmentLeft);
            pLabelTTFDesc->setVerticalAlignment(kCCVerticalTextAlignmentTop);
            // pCell->addChild(pLabelTTFDesc);
            pSpriteCellBackground->addChild(pLabelTTFDesc);
            
            // 根据配置文件创建图片
            
            // 当前项目是否折叠标志
            bool bFold = pDict->valueForKey("unfold")->boolValue();
            // 如果是是展开的
            
        }
	}
	return pCell;
}

unsigned int BalloonHandBookScene::numberOfCellsInTableView( CCTableView *table )
{
	// return the counts of TableView
    if (m_pDictHandbookData)
    {
        CCArray* pArrayData = dynamic_cast<CCArray*>(m_pDictHandbookData->objectForKey("items"));
        if (pArrayData)
        {
            return pArrayData->count();
        }
    }
	return 0;
}

CCScene* BalloonHandBookScene::scene()
{
    CCScene* pScene = CCScene::create();
    pScene->addChild(BalloonHandBookScene::create());
    
    return pScene;
}

void BalloonHandBookScene::initItemTableView()
{
    if (!m_pDictHandbookData)
    {
        m_pDictHandbookData = CCDictionary::createWithContentsOfFile("configuration/handBookItems.plist");
        CCAssert(m_pDictHandbookData, "configruation/handBookItems.plist is incorrect!!");
        CC_SAFE_RETAIN(m_pDictHandbookData);
    }
    
    if (!m_pTableViewHandbook)
    {
       m_pTableViewHandbook = CCTableView::create(this, m_pLayerItemContainer->getContentSize(), NULL);
       m_pTableViewHandbook->setDelegate(this);
       
       m_pTableViewHandbook->setDirection(kCCScrollViewDirectionVertical);
       m_pTableViewHandbook->setVerticalFillOrder(kCCTableViewFillTopDown);
       
       m_pLayerItemContainer->addChild(m_pTableViewHandbook);
       
       m_pTableViewHandbook->reloadData();
    }
}

void BalloonHandBookScene::onPressMenuReturnMainBoard(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCDirector::sharedDirector()->popScene();
}

void BalloonHandBookScene::reLayoutCoins()
{
    // 根据金钱的数量重新定位金币标志
    m_pSpriteCoin->setPosition(ccp(m_pLabelBMFontCoins->getPositionX() - m_pLabelBMFontCoins->boundingBox().size.width - m_pSpriteCoin->getContentSize().width*0.2f, m_pSpriteCoin->getPositionY()));
}

void BalloonHandBookScene::updateCoins()
{
    m_pLabelBMFontCoins->setString(CCString::createWithFormat("%lld", UserDataManager::sharedUserDataManager()->getGoldenCoins())->getCString());
    // 设置完数据顺便重新定位下
    reLayoutCoins();
}

bool BalloonHandBookScene::setResourceString()
{
    m_vTexturesString.push_back("texture/balloon/balloon.plist");
    m_vTexturesString.push_back("texture/items/items.plist");
    m_vTexturesString.push_back("texture/common/common.plist");
    m_vTexturesString.push_back("texture/balloonScene/background.png");
    
    return true;
}
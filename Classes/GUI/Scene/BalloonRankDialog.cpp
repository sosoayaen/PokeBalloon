#include "BalloonRankDialog.h"
#include "BalloonSoundManager.h"
#include "CCJSONConverter.h"
#include "UserData.h"
#include "MemBufferObject.h"
#include "bailinUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define NOTIFY_HTTP_RANK_DATA_CALLBACK "NotifyHttpRankDataCallback"

BalloonRankDialog::~BalloonRankDialog()
{
    CC_SAFE_RELEASE(m_pDictRankData);
}

bool BalloonRankDialog::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!DialogLayer::init());
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		std::string strCCBFileName = "BalloonRankDialog.ccbi";

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
        
        initTableView();
        
        initMenu();
        
        
        // set animation onEnter and onExit
        m_pMainBoard = m_pSpriteBoard;
        m_pSpriteBoard->setScale(0.01f);
        setOnEnterAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
        setOnExitAction(CCEaseExponentialIn::create(CCScaleTo::create(0.5f, 0.01f)));
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonRankDialog::onEnter()
{
	DialogLayer::onEnter();
    
    // add observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BalloonRankDialog::NotifyHttpRankDataCallback), NOTIFY_HTTP_RANK_DATA_CALLBACK, NULL);
	
    requestRankData();
}

void BalloonRankDialog::onExit()
{
	// TODO: clean the scene
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFY_HTTP_RANK_DATA_CALLBACK);
	
	DialogLayer::onExit();
}

bool BalloonRankDialog::initLabelTTF()
{
	// init labelTTF with original size in ccb's description
	float fRate = CCDirector::sharedDirector()->getContentScaleFactor();


	// your init code here
	
	return true;
}

SEL_CallFuncN BalloonRankDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonRankDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuShare", BalloonRankDialog::onPressMenuShare);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuClose", BalloonRankDialog::onPressMenuClose);

	return NULL;
}

bool BalloonRankDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerTableViewContainer", CCLayer*, this->m_pLayerTableViewContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTop", CCMenu*, this->m_pMenuTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteContainer", CCSprite*, this->m_pSpriteContainer);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBoard", CCSprite*, this->m_pSpriteBoard);

	return true;
}

SEL_CCControlHandler BalloonRankDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonRankDialog::onPressMenuShare(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
}

void BalloonRankDialog::onPressMenuClose(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    endDialog();
}

void BalloonRankDialog::keyBackClicked( void )
{
    endDialog();
}

void BalloonRankDialog::keyMenuClicked( void )
{
	// TODO:
}


void BalloonRankDialog::scrollViewDidScroll( CCScrollView* view )
{
    
}

void BalloonRankDialog::scrollViewDidZoom( CCScrollView* view )
{

}

void BalloonRankDialog::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
	CCLOG("cell touched at index: %i, table's children counts:%i", cell->getIdx()+1, numberOfCellsInTableView(NULL));
	
	if (cell != NULL)
	{
		// TODO: Do something when TOUCH the item
	} // end if (cell != NULL)
}

CCSize BalloonRankDialog::cellSizeForTable( CCTableView *table )
{
	// Return a CCSize with the item size you want to show
    CCSize size = m_pLayerTableViewContainer->getContentSize();
	return CCSizeMake(size.width, size.height*0.1f);
}

void BalloonRankDialog::createCellForScoreRank(cocos2d::extension::CCTableViewCell *cell, cocos2d::extension::CCTableView *table, unsigned int idx)
{
    CCSize cellSize = tableCellSizeForIndex(table, idx);
    
    do
    {
        // get array data
        CCArray* pArrayData = getRankDataArray();
        CC_BREAK_IF(!pArrayData);
        
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(pArrayData->objectAtIndex(idx));
        CC_BREAK_IF(!pDict);
        
        // Rank
        long lRank = DataManagerUtil::sharedDataManagerUtil()->GetLongValueWithObject(pDict->objectForKey("rank"));
        CCLabelTTF* pLabelRank = CCLabelTTF::create(CCString::createWithFormat("%ld", lRank)->getCString(), "", cellSize.height*0.5f);
        pLabelRank->setPosition(ccp(cellSize.width*0.05f, cellSize.height*0.5f));
        cell->addChild(pLabelRank);
        
        // NickName
        CCLabelTTF* pLabelNickName = CCLabelTTF::create(pDict->valueForKey("user_name")->getCString(), "", cellSize.height*0.5f);
        pLabelNickName->setAnchorPoint(ccp(0, 0.5f));
        pLabelNickName->setPosition(ccp(cellSize.width*0.1f, cellSize.height*0.5f));
        cell->addChild(pLabelNickName);
        
        // Score
        CCLabelTTF* pLabelScore = CCLabelTTF::create(pDict->valueForKey("score")->getCString(), "", cellSize.height*0.5f);
        pLabelScore->setAnchorPoint(ccp(0, 0.5f));
        pLabelScore->setPosition(ccp(pLabelNickName->getPositionX() + pLabelNickName->getContentSize().width + 10,
                                     pLabelNickName->getPositionY()));
        cell->addChild(pLabelScore);
        
        // Date
        CCLabelTTF* pLabelDate = CCLabelTTF::create(pDict->valueForKey("gmt_mtime")->getCString(), "", cellSize.height*0.2f);
        pLabelDate->setAnchorPoint(ccp(1.0f, 0.5f));
        pLabelDate->setPosition(ccp(cellSize.width*0.98f, cellSize.height*0.5f));
        cell->addChild(pLabelDate);
        
    } while (0);
}

CCTableViewCell* BalloonRankDialog::tableCellAtIndex( CCTableView *table, unsigned int idx )
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
        createCellForScoreRank(pCell, table, idx);
	}
	return pCell;
}

unsigned int BalloonRankDialog::numberOfCellsInTableView( CCTableView *table )
{
    unsigned int nCnt = 0;
    
    do
    {
        CCArray* pArrayData = getRankDataArray();
        CC_BREAK_IF(!pArrayData);
        
        nCnt = pArrayData->count();
        
    } while (0);
    
	return nCnt;
}

void BalloonRankDialog::initTableView()
{
    if (m_pTableViewRank) return;
    
    // crate the tableview obj
    m_pTableViewRank = CCTableView::create(this, m_pLayerTableViewContainer->getContentSize());
    m_pTableViewRank->setDelegate(this);
    m_pTableViewRank->setDirection(kCCScrollViewDirectionVertical);
    m_pTableViewRank->setVerticalFillOrder(kCCTableViewFillTopDown);
    m_pLayerTableViewContainer->addChild(m_pTableViewRank);
    
    pushScrollView(m_pTableViewRank);
}

void BalloonRankDialog::requestRankData()
{
    CCDictionary* pDict = CCDictionary::create();
    pDict->setObject(ccs("12345678901234567890123456789012"), "uid");
    pDict->setObject(ccs(UserDataManager::sharedUserDataManager()->getNickName().c_str()), "uname");
    
    const char* pszJSON = CCJSONConverter::sharedConverter()->strFrom(pDict);
    // request http data
    HttpCenter::sharedHttpCenter()->request("http://121.40.76.13/score_rank.php", NOTIFY_HTTP_RANK_DATA_CALLBACK, CCHttpRequest::kHttpPost, pszJSON);
    
    free((void*)pszJSON);
}

void BalloonRankDialog::NotifyHttpRankDataCallback(cocos2d::CCObject *pData)
{
    MemBufferObject* pMemObj = dynamic_cast<MemBufferObject*>(pData);
    
    // Convert JSON string to CCDictionary object
    std::string strJSON(pMemObj->GetBuffer(), pMemObj->GetBufferLen());
    if (!strJSON.empty())
    {
        CCDictionary* pDict = CCJSONConverter::sharedConverter()->dictionaryFrom(strJSON.c_str());
        CC_SAFE_RELEASE(m_pDictRankData);
        m_pDictRankData = pDict;
        CC_SAFE_RETAIN(m_pDictRankData);
        
        // reload data
        if (m_pTableViewRank) m_pTableViewRank->reloadData();
    }
}

CCArray* BalloonRankDialog::getRankDataArray()
{
    CCArray* pArray = NULL;
    
    do
    {
        CC_BREAK_IF(!m_pDictRankData);
        
        // get data object
        CCDictionary* pDictData = dynamic_cast<CCDictionary*>(m_pDictRankData->objectForKey("data"));
        CC_BREAK_IF(!pDictData);
        
        // get real data array object
        pArray = dynamic_cast<CCArray*>(pDictData->objectForKey("res"));
        
    } while (0);
    
    return pArray;
}

void BalloonRankDialog::initMenu()
{
    
    // init the top menu
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuTop);
    
    // rotate the menu on top
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
    
    pushMenu(m_pMenuTop);
}
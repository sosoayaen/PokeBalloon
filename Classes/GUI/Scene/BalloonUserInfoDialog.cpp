#include "BalloonUserInfoDialog.h"
#include "UserData.h"
#include "bailinUtil.h"
#include "BalloonSoundManager.h"
#include "UMSocial2DX.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define SNS_SHARE_IMAGE_FILE_NAME "sns_share.png"
#define TAG_ID_MENU_ITEM_MUSIC_FORBIDDEN_SPRITE 100 // 音乐开关上的禁止图片

BalloonUserInfoDialog::~BalloonUserInfoDialog()
{
    CC_SAFE_RELEASE(m_pDictHandbookData);
}

bool BalloonUserInfoDialog::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!DialogLayer::init());
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonUserInfoDialog.ccbi";

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
		
		setKeypadEnabled(true);
        
        // SetVisibleBoard(m_pSpriteBoard);
        
        pushMenu(m_pMenuTab);
        pushMenu(m_pMenuMusic);
        pushMenu(m_pMenuTop);
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
        
        initTableView();
        initLabelTTF();
        initMenuTabText();
        initPressMenuEffect();
        initEditBox();
        
        // 这是入场和出场的动画
        m_pSpriteBoard->setScale(0.01f);
        m_pMainBoard = m_pSpriteBoard;
        setOnEnterAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
        setOnExitAction(CCEaseExponentialIn::create(CCScaleTo::create(0.4f, 0.01f)));
        
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/balloon/balloon.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/items/items.plist");
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonUserInfoDialog::initLabelTTF()
{
    // lock size of labelTTF
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFAchivementHolder);
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFAchivementValue);
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFNickname);
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFHighScoreHolder);
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFHighestScoreValue);
    m_pLabelTTFNickname->setVisible(false);
    
    // 把名字搞成按钮
    m_pMenuNickname = CCMenu::create();
    m_pMenuNickname->setPosition(m_pLabelTTFNickname->getPosition());
    
    CCLabelTTF* plbl = CCLabelTTF::create(m_pLabelTTFNickname->getString(), "", m_pLabelTTFNickname->getFontSize());
    m_pMenuItemLabelNickName = CCMenuItemLabel::create(plbl, this, menu_selector( BalloonUserInfoDialog::onPressMenuNickname));
    m_pMenuNickname->addChild(m_pMenuItemLabelNickName);
    m_pSpriteBoard->addChild(m_pMenuNickname);
    pushMenu(m_pMenuNickname);
    
}

void BalloonUserInfoDialog::onEnter()
{
	DialogLayer::onEnter();
    
    // 初始化分数等
    updateLabelData();
    
    // 默认选中第一个标签
    onPressMenuTabDetail(m_pMenuTab->getChildren()->objectAtIndex(0));
}

void BalloonUserInfoDialog::onExit()
{
    
	DialogLayer::onExit();
}

SEL_CallFuncN BalloonUserInfoDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonUserInfoDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuMusic", BalloonUserInfoDialog::onPressMenuMusic);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuSound", BalloonUserInfoDialog::onPressMenuSound);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuTabDetail", BalloonUserInfoDialog::onPressMenuTabDetail);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuShare", BalloonUserInfoDialog::onPressMenuShare);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuClose", BalloonUserInfoDialog::onPressMenuClose);

	return NULL;
}

bool BalloonUserInfoDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFAchivementHolder", CCLabelTTF*, this->m_pLabelTTFAchivementHolder);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFAchivementValue", CCLabelTTF*, this->m_pLabelTTFAchivementValue);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFHighScoreHolder", CCLabelTTF*, this->m_pLabelTTFHighScoreHolder);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFHighestScoreValue", CCLabelTTF*, this->m_pLabelTTFHighestScoreValue);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteScoreArea", CCSprite*, this->m_pSpriteScoreArea);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemMusic", CCMenuItemImage*, this->m_pMenuItemMusic);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemSound", CCMenuItemImage*, this->m_pMenuItemSound);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItem1", CCMenuItem*, this->m_pMenuItem1);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuMusic", CCMenu*, this->m_pMenuMusic);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItem1", CCMenuItem*, this->m_pMenuItem1);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItem2", CCMenuItem*, this->m_pMenuItem2);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItem3", CCMenuItem*, this->m_pMenuItem3);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTab", CCMenu*, this->m_pMenuTab);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTop", CCMenu*, this->m_pMenuTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteTableViewBackground", CCSprite*, this->m_pSpriteTableViewBackground);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFNickname", CCLabelTTF*, this->m_pLabelTTFNickname);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBoard", CCSprite*, this->m_pSpriteBoard);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerTableContainer", CCLayer*, this->m_pLayerTableContainer);

	return true;
}

SEL_CCControlHandler BalloonUserInfoDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonUserInfoDialog::onPressMenuMusic(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    // 根据当前状态设置开启或者终止音乐
    long lOffFlag = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF);
    lOffFlag = lOffFlag ? 0 : 1;
    DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(OPT_MUSIC_OFF, lOffFlag);
    
    // 背景音乐要根据设置立刻执行
    if (lOffFlag == 0)
    {
        BalloonSoundManager::sharedBalloonSoundManager()->resumeBackgroundMusic();
    }
    else
    {
        BalloonSoundManager::sharedBalloonSoundManager()->pauseBackgroundMusic();
    }
    
    // 当前如果允许播放背景音乐，择把按钮拿掉
    this->setMenuMusicBtnStatusEnable((CCMenuItemImage*)pSender, lOffFlag == 0);
}

void BalloonUserInfoDialog::onPressMenuSound(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
	// 根据当前状态设置开启或者终止音效
    long lOffFlag = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF);
    lOffFlag = lOffFlag ? 0 : 1;
    DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(OPT_SOUND_EFFECT_OFF, lOffFlag);
    
    // 音效不用立即处理，因为只要在下一次触发的时候判断全局变量的值来决定是否播放即可
    
    setMenuMusicBtnStatusEnable((CCMenuItemImage*)pSender, lOffFlag == 0);
}

void BalloonUserInfoDialog::onPressMenuTabDetail(CCObject* pSender)
{
    CCNode* pNode = (CCNode*)pSender;
    
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(m_pMenuTab->getChildren(), pObj)
    {
        CCMenuItemImage* pItem = dynamic_cast<CCMenuItemImage*>(pObj);
        if (pItem)
        {
            pItem->setEnabled(pItem != pSender);
        }
    }
    
    m_nTabIndex = pNode->getTag() - 1;
    if (m_pTableViewDetail)
    {
        m_pTableViewDetail->reloadData();
    }
}

void BalloonUserInfoDialog::keyBackClicked( void )
{
	endDialog();
}

void BalloonUserInfoDialog::keyMenuClicked( void )
{
    
}


void BalloonUserInfoDialog::scrollViewDidScroll( CCScrollView* view )
{

}

void BalloonUserInfoDialog::scrollViewDidZoom( CCScrollView* view )
{

}

void BalloonUserInfoDialog::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
    unsigned int idx = cell->getIdx();
	CCLOG("cell touched at index: %i, table's children counts:%i", idx + 1, numberOfCellsInTableView(NULL));
	
	if (cell != NULL)
	{
        switch (m_nTabIndex)
        {
            case 0: // 升级道具标签
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
            }
                break;
                
            case 1: // 成就一览
                break;
            case 2: // 保留项
                break;
                
            default:
                break;
        }
	} // end if (cell != NULL)
}

void BalloonUserInfoDialog::tableCellHighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    CCNode* pNode = cell->getChildByTag(100);
    if (pNode)
    {
        pNode->stopAllActions();
        pNode->runAction(CCScaleTo::create(0.2f, 1.1f));
    }
}

void BalloonUserInfoDialog::tableCellUnhighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    CCNode* pNode = cell->getChildByTag(100);
    if (pNode)
    {
        pNode->stopAllActions();
        pNode->runAction(CCScaleTo::create(0.2f, 1.0f));
    }
}

CCSize BalloonUserInfoDialog::cellSizeForTable( CCTableView *table )
{
	// Return a CCSize with the item size you want to show
    return CCSizeZero;
}

CCSize BalloonUserInfoDialog::tableCellSizeForIndex(cocos2d::extension::CCTableView *table, unsigned int idx)
{
    switch (m_nTabIndex)
    {
        case 0:
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
                    
                    CCSize size = CCSizeZero;
                    // 判断打开收拢的状态
                    bool bUnFold = pDict->valueForKey("unfold")->boolValue();
                    if (bUnFold)
                        size = CCSizeMake(m_pLayerTableContainer->getContentSize().width, m_pSpriteTableViewBackground->getContentSize().height*0.75f);
                    else
                        size = CCSizeMake(m_pLayerTableContainer->getContentSize().width, m_pSpriteTableViewBackground->getContentSize().height*0.37f);
                    
                    return size;
                }
            } while (0);
            break;
            
        case 1:
            break;
            
        case 2:
            break;
            
        default:
            break;
    }
    
    return CCSizeZero;
}

CCTableViewCell* BalloonUserInfoDialog::tableCellAtIndex( CCTableView *table, unsigned int idx )
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
        switch (m_nTabIndex)
        {
            case 0: // 第一个标签页
                createTableCellLevelUp(table, pCell, idx);
                break;
                
            default:
                break;
        }
	}
	return pCell;
}

unsigned int BalloonUserInfoDialog::numberOfCellsInTableView( CCTableView *table )
{
    int nRet = 0;
    switch (m_nTabIndex) {
        case 0:
            if (m_pDictHandbookData)
            {
                CCArray* pArrayData = dynamic_cast<CCArray*>(m_pDictHandbookData->objectForKey("items"));
                if (pArrayData)
                {
                    return pArrayData->count();
                }
            }
            break;
            
        default:
            break;
    }
	return nRet;
}

void BalloonUserInfoDialog::updateLabelData()
{
    UserDataManager* pSharedUserData = UserDataManager::sharedUserDataManager();
    // update the user's nickname
    // m_pLabelTTFNickname->setString(pSharedUserData->getNickName().c_str());
    m_pMenuItemLabelNickName->setString(pSharedUserData->getNickName().c_str());
    // update archivement value
    m_pLabelTTFAchivementValue->setString(CCString::createWithFormat("%ld", pSharedUserData->getArchivmentValue())->getCString());
    // update highest score value
    m_pLabelTTFHighestScoreValue->setString(CCString::createWithFormat("%lld", pSharedUserData->getHighestScore())->getCString());
    // update coins
    m_pLabelBMFontCoins->setString(CCString::createWithFormat("%lld", pSharedUserData->getGoldenCoins())->getCString());
    if (m_pSpriteBoard->boundingBox().size.width*0.25f < m_pLabelBMFontCoins->boundingBox().size.width)
        m_pLabelBMFontCoins->setScale(m_pSpriteBoard->getContentSize().width*0.25f/m_pLabelBMFontCoins->boundingBox().size.width);
    else
        m_pLabelBMFontCoins->setScale(2.0f);
}

void BalloonUserInfoDialog::addLabelTTFToNode(cocos2d::CCNode *pNode, const char *pszText)
{
    CCLabelTTF* pLabel = CCLabelTTF::create(pszText, "", pNode->getContentSize().height*0.6f);
    pLabel->setColor(ccBLACK);
    pLabel->setPosition(ControlUtil::sharedControlUtil()->getCenterPointWithNode(pNode));
    pNode->addChild(pLabel);
}

void BalloonUserInfoDialog::initMenuTabText()
{
    // TODO: 这里的数据要移到对应语种的plist中
    const char* pszTabText[] = {
        "道具升级",
        "成就一览",
        "滥竽充数",
    };
    
    CCObject* pObj = NULL;
    int i = 0;
    CCARRAY_FOREACH(m_pMenuTab->getChildren(), pObj)
    {
        CCMenuItemImage* pMenuItemImage = dynamic_cast<CCMenuItemImage*>(pObj);
        if (pMenuItemImage)
        {
            addLabelTTFToNode(pMenuItemImage->getNormalImage(), pszTabText[i]);
            addLabelTTFToNode(pMenuItemImage->getSelectedImage(), pszTabText[i]);
            addLabelTTFToNode(pMenuItemImage->getDisabledImage(), pszTabText[i]);
        }
        i++;
    }
    
    ((CCNode*)m_pMenuTab->getChildren()->lastObject())->setVisible(false);
    
    
}

void BalloonUserInfoDialog::addForbiddenSpriteToNode(cocos2d::CCNode *pNode)
{
    CCSprite* pSpriteForbidden = CCSprite::createWithSpriteFrameName("useirnfo_forbidden.png");
    pSpriteForbidden->setAnchorPoint(ccp(1.0f, 0));
    pSpriteForbidden->setPosition(ccp(pNode->getContentSize().width, 0));
    pSpriteForbidden->setTag(TAG_ID_MENU_ITEM_MUSIC_FORBIDDEN_SPRITE);
    pNode->addChild(pSpriteForbidden);
}

void BalloonUserInfoDialog::initPressMenuEffect()
{
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuMusic, 1.5f);
    
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(m_pMenuMusic->getChildren(), pObj)
    {
        CCMenuItemImage* pMenuItemImage = dynamic_cast<CCMenuItemImage*>(pObj);
        if (pMenuItemImage)
        {
            // 增加一个禁用的右下角角标
            addForbiddenSpriteToNode(pMenuItemImage->getNormalImage());
            addForbiddenSpriteToNode(pMenuItemImage->getSelectedImage());
        }
    }
    
    // 根据状态确定是否显示关闭图片
    
    // 根据全局数据设置按钮状态
    long lMusicOFF = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF);
    setMenuMusicBtnStatusEnable(m_pMenuItemMusic, lMusicOFF == 0);
   
    long lSoundEffectOFF = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF);
    
    setMenuMusicBtnStatusEnable(m_pMenuItemSound, lSoundEffectOFF == 0);
}

void BalloonUserInfoDialog::setForbiddenSpriteVisible(cocos2d::CCNode *pNode, bool bVisible)
{
    CCNode* pNodeForbidden = pNode->getChildByTag(TAG_ID_MENU_ITEM_MUSIC_FORBIDDEN_SPRITE);
    if (pNodeForbidden)
    {
        pNodeForbidden->setVisible(bVisible);
    }
}

void BalloonUserInfoDialog::setMenuMusicBtnStatusEnable(cocos2d::CCMenuItemImage* pItem, bool bEnable)
{
    setForbiddenSpriteVisible(pItem->getNormalImage(), !bEnable);
    setForbiddenSpriteVisible(pItem->getSelectedImage(), !bEnable);
}

void BalloonUserInfoDialog::saveUserinfoData()
{
    // 保存音乐音效开关数据
    CCUserDefault::sharedUserDefault()->setIntegerForKey(OPT_MUSIC_OFF, DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF));
    CCUserDefault::sharedUserDefault()->setIntegerForKey(OPT_SOUND_EFFECT_OFF, DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF));
    
    // TODO: 保存用户道具升级数据
    UserDataManager::sharedUserDataManager()->saveData();
}

bool BalloonUserInfoDialog::endDialog(cocos2d::CCActionInterval* pAction /* = NULL */, float fDuration /* = 1.0f */)
{
    // 先数据落地
    saveUserinfoData();
    
    return DialogLayer::endDialog(pAction, fDuration);
}

void BalloonUserInfoDialog::onPressMenuNickname(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    m_pMenuNickname->setVisible(false);
    // 弹出一个对话框提供名字修改，修改后更新下名字的显示
    m_pEditBoxNickname->setPosition(m_pMenuNickname->getPosition());
    m_pEditBoxNickname->setText(UserDataManager::sharedUserDataManager()->getNickName().c_str());
    m_pEditBoxNickname->setVisible(true);
    m_pEditBoxNickname->sendActionsForControlEvents(CCControlEventTouchUpInside);
}

void BalloonUserInfoDialog::onPressMenuClose(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    endDialog();
}

void BalloonUserInfoDialog::onPressMenuShare(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    // save the picture
    CCSize size = m_pSpriteBoard->getContentSize();
    
    // 保存并设置需要裁剪的节点的图形的位置，和画板位置对齐
    CCPoint pos = m_pSpriteBoard->getPosition();
    CCPoint anchorPt = m_pSpriteBoard->getAnchorPoint();
    m_pSpriteBoard->setPosition(CCPointZero);
    m_pSpriteBoard->setAnchorPoint(CCPointZero);
    
    CCRenderTexture* pTexture = CCRenderTexture::create(size.width, size.height, kCCTexture2DPixelFormat_RGBA8888);
    pTexture->clear(255, 255, 255, 255);
    
    pTexture->begin();
    m_pSpriteBoard->visit();
    pTexture->end();
    
    // 还原位置
    m_pSpriteBoard->setPosition(pos);
    m_pSpriteBoard->setAnchorPoint(anchorPt);
    
    // 保存截图并返回图片路径
    if (pTexture->saveToFile(SNS_SHARE_IMAGE_FILE_NAME, kCCImageFormatPNG))
    {
        std::string strImagePath = CCFileUtils::sharedFileUtils()->getWritablePath() + SNS_SHARE_IMAGE_FILE_NAME;
        
        CCDictionary* pDictData = CCDictionary::create();
        CCString* pStrShareText = ccs("Share the Rank!");
        pDictData->setObject(pStrShareText, "shareText");
        pDictData->setObject(ccs(strImagePath.c_str()), "shareImage");
        UMSocial2DX::shareSNS(pDictData);
    }
}

void BalloonUserInfoDialog::initTableView()
{
    if (!m_pTableViewDetail)
    {
        CCSize size = m_pLayerTableContainer->getContentSize();
        m_pTableViewDetail = CCTableView::create(this, size);
        m_pTableViewDetail->setDelegate(this);
        
        m_pTableViewDetail->setDirection(kCCScrollViewDirectionVertical);
        m_pTableViewDetail->setVerticalFillOrder(kCCTableViewFillTopDown);
        
        pushScrollView(m_pTableViewDetail);
        m_pLayerTableContainer->addChild(m_pTableViewDetail);
        
    }
    
    // 初始化升级数据，暂时用
    if (!m_pDictHandbookData)
    {
        m_pDictHandbookData = CCDictionary::createWithContentsOfFile("configuration/handBookItems.plist");
        CCAssert(m_pDictHandbookData, "configruation/handBookItems.plist is incorrect!!");
        CC_SAFE_RETAIN(m_pDictHandbookData);
    }
}

void BalloonUserInfoDialog::createTableCellLevelUp(CCTableView* table, CCTableViewCell* pCell, unsigned int idx)
{
    // 清空内部数据
    pCell->removeAllChildren();
    // 单元大小
    CCSize cellSize = tableCellSizeForIndex(table, idx);
    // 创建单元背景
    CCScale9Sprite* pSpriteCellBackground = CCScale9Sprite::create("texture/handBook/scale9frame.png");
    pSpriteCellBackground->setTag(100);
    // pSpriteCellBackground->setPreferredSize(CCSizeMake(cellSize.width*0.95f, cellSize.height*0.95f));
    pSpriteCellBackground->setPreferredSize(CCSizeMake(cellSize.width, cellSize.height*0.96f));
    pSpriteCellBackground->setPosition(ccp(cellSize.width*0.5f, cellSize.height*0.5f));
    pCell->addChild(pSpriteCellBackground);
    
    CCArray* pArray = dynamic_cast<CCArray* >(m_pDictHandbookData->objectForKey("items"));
    CCObject* pObj = pArray->objectAtIndex(idx);
    CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
    if (pDict)
    {
        // 创建图形
        CCSprite* pSpriteBalloon = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("balloon_4_%d.png", (idx%8) + 1)->getCString());
        
        // 根据配置得到内部图片
        CCNode* pNodeDesc = NULL;
        const CCString* pStringType = pDict->valueForKey("displayType");
        switch (pStringType->intValue())
        {
            case 1: // 图片
                pNodeDesc = CCSprite::createWithSpriteFrameName(pDict->valueForKey("frameName")->getCString());
                break;
            case 2: // 文字
                pNodeDesc = CCLabelBMFont::create(pDict->valueForKey("bmfValue")->getCString(), "texture/fonts/font.fnt");
                pNodeDesc->setScale(pSpriteBalloon->getContentSize().width*0.5f/pNodeDesc->getContentSize().width);
                break;
            default:
                break;
        }
        
        // 放置气球的位置
        if (pNodeDesc)
        {
            pNodeDesc->setAnchorPoint(ccp(0.5f, 0.5f));
            pNodeDesc->setPosition(ccpMult(ccpFromSize(pSpriteBalloon->getContentSize()), 0.5f));
            pSpriteBalloon->addChild(pNodeDesc);
        }
        
        float offsetBalloonY = 0;
        // 顺便放下星星
        for (int i = 0; i < 3; ++i)
        {
            const char* pszStarName = NULL;
            if (i < 2)
                pszStarName = "userinfo_star_full.png";
            else
                pszStarName = "userinfo_star_empty.png";
            
            CCSprite* star = CCSprite::createWithSpriteFrameName(pszStarName);
            CCSize starSize = star->getContentSize();
            // 根据位置摆放星星
            star->setPosition(ccp(pSpriteBalloon->getContentSize().width*0.5 - starSize.width + i*starSize.width, -starSize.height));
            pSpriteBalloon->addChild(star);
            
            offsetBalloonY = starSize.height*0.5f;
        }
        
        // 摆放位置
        
        pSpriteBalloon->setScale(cellSize.width*0.23f/pSpriteBalloon->getContentSize().height);
        // pSpriteBalloon->setPosition(ccp(cellSize.width*0.155f, cellSize.height*0.5f + offsetBalloonY));
        pSpriteBalloon->setPosition(ccp(pSpriteBalloon->boundingBox().size.width*0.8f, cellSize.height - pSpriteBalloon->boundingBox().size.height*0.75f + offsetBalloonY));
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
        // pLabelTTFDesc->setPosition(ccp(cellSize.width*0.3f, 30));
        pLabelTTFDesc->setPosition(pLabelTTFTitle->getPosition());
        pLabelTTFDesc->setAnchorPoint(ccp(0, 1.0f));
        pLabelTTFDesc->setHorizontalAlignment(kCCTextAlignmentLeft);
        pLabelTTFDesc->setVerticalAlignment(kCCVerticalTextAlignmentTop);
        // pCell->addChild(pLabelTTFDesc);
        pSpriteCellBackground->addChild(pLabelTTFDesc);
        
        // 根据配置文件创建图片
        
        CCSprite* pSpriteBtnExtend = CCSprite::createWithSpriteFrameName("userinfo_btn_extend.png");
        pSpriteBtnExtend->setPosition(ccp(cellSize.width - pSpriteBtnExtend->getContentSize().width*0.8f, cellSize.height - pSpriteBtnExtend->getContentSize().height));
        pSpriteCellBackground->addChild(pSpriteBtnExtend);
        // 当前项目是否折叠标志
        bool bFold = pDict->valueForKey("unfold")->boolValue();
        // 如果是是展开的
        if (bFold)
        {
            pLabelTTFDesc->setDimensions(CCSizeMake(cellSize.width*0.5f, 0));
            pSpriteBtnExtend->setRotation(90);
            
            // 展开的就增加升级的按钮
            CCSprite* pSpriteBtnLevelup = CCSprite::createWithSpriteFrameName("userinfo_btn_levelup.png");
            pSpriteBtnLevelup->setAnchorPoint(ccp(1.0f, 0.5f));
            pSpriteBtnLevelup->setPosition(ccp(pSpriteBtnExtend->getPositionX(), pLabelTTFDesc->getPositionY() - pLabelTTFDesc->getContentSize().height - pSpriteBtnLevelup->getContentSize().height));
            pSpriteCellBackground->addChild(pSpriteBtnLevelup);
        }
        else
        {
            pLabelTTFDesc->setDimensions(CCSizeMake(cellSize.width*0.5f, (cellSize.height-30*2-pLabelTTFTitle->getContentSize().height)));
            pSpriteBtnExtend->setRotation(0);
        }
    }
}

void BalloonUserInfoDialog::editBoxReturn(cocos2d::extension::CCEditBox *editBox)
{
    m_pMenuNickname->setVisible(true);
    UserDataManager::sharedUserDataManager()->setNickName(editBox->getText());
    m_pMenuItemLabelNickName->setString(editBox->getText());
    editBox->setVisible(false);
}

void BalloonUserInfoDialog::initEditBox()
{
    // 初始化名字输入的EditBox
    CCScale9Sprite* pNormal9SpriteBg = CCScale9Sprite::createWithSpriteFrameName("userinfo_score_dise.png");
    m_pEditBoxNickname = CCEditBox::create(CCSizeMake(m_pSpriteBoard->getContentSize().width*0.8f, m_pMenuItemLabelNickName->getContentSize().height), pNormal9SpriteBg);
    m_pEditBoxNickname->setFontColor(ccBLACK);
    m_pEditBoxNickname->setPosition(m_pMenuNickname->getPosition());
    m_pEditBoxNickname->setInputFlag(kEditBoxInputFlagInitialCapsWord);
    m_pEditBoxNickname->setInputMode(kEditBoxInputModeSingleLine);
    m_pEditBoxNickname->setMaxLength(20);
    m_pEditBoxNickname->setDelegate(this);
    m_pSpriteBoard->addChild(m_pEditBoxNickname);
    m_pEditBoxNickname->setVisible(false);
    pushEditBox(m_pEditBoxNickname);
}
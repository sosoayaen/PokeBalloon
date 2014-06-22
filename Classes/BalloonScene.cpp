#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "MobClickCpp.h"
#include "GAdMob2DX.h"
#include "UMSocial2DX.h"
#include "BalloonPauseDialog.h"
#include "BalloonResultDialog.h"

#include "bailinUtil.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#   include "GameKitHelper2dx.h"
#endif

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define BALLOON_SHOW_RATE    40     // 气球出现的概率，10表示1%，50就是5%
#define CLOUD_SHOW_RATE      5
#define DEFAULT_TIME         30

#define SECURITY_TIME "S_TIME"
#define SECURITY_SCORE "S_SCORE"

// 结算对话框和暂停对话框中的再玩一次与回到主界面按钮的UserData数据
#define TAG_ID_RESULT_DIALOG_PLAYAGAIN 1000
#define TAG_ID_PAUSE_DIALOG_PLAYAGAIN 1001
#define TAG_ID_RESULT_DIALOG_RETURN 1002
#define TAG_ID_PAUSE_DIALOG_RETURN 1003
// 结算对话框和暂停对话框
#define TAG_ID_RESULT_DIALOG    1000
#define TAG_ID_PAUSE_DIALOG     1001

BalloonScene::~BalloonScene()
{
    /*
    if (m_pPauseDialog && m_pPauseDialog->getParent())
        m_pPauseDialog->removeFromParent();
    CC_SAFE_RELEASE_NULL(m_pPauseDialog);
    
    if (m_pResultDialog && m_pResultDialog->getParent())
        m_pResultDialog->removeFromParent();
    CC_SAFE_RELEASE_NULL(m_pResultDialog);
    */
}

CCScene* BalloonScene::scene()
{
    CCScene* pScene = CCScene::create();
    
    pScene->addChild(BalloonScene::create());
    
    return pScene;
}

bool BalloonScene::init()
{
	bool bRet = false;
	do
	{
		// CC_BREAK_IF(!CCLayer::init());
        CC_BREAK_IF(!AutoTextureManagerLayer::init());
        /*
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/cloud/cloud.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/balloon/balloon.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/items/items.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/balloonEffect/balloon_effect_frozen.plist");
        */
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("texture/menuItems/menuItems.plist");
        
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonScene.ccbi";

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
        
        // 暂停按钮按下效果
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuPause);
        
        resetData();
        
        CCMenuItemFont::setFontSize(getContentSize().width*0.1f);
        
        m_pSpriteBackground->setScaleX(getContentSize().width/m_pSpriteBackground->getContentSize().width);
        m_pSpriteBackground->setScaleY(getContentSize().height/m_pSpriteBackground->getContentSize().height);
		
//		setKeypadEnabled(true);
        
        // 初始化气球管理类
        if (!m_BalloonManager.getInitFlag())
            m_BalloonManager.init(this, m_pLayerBalloon);
        
        // 初始化云朵管理类
        if (!m_CloudManager.getInitFlag())
            m_CloudManager.init(m_pLayerBalloon);
        
        // 初始化道具管理类
        if (!m_BalloonItemManager.getInitFlag())
            m_BalloonItemManager.init(this, m_pLayerItems);
        // 设置点击和持续性道具的显示位置
        m_BalloonItemManager.setClickItemPosition(m_pSpriteScoreBar->getPosition());
        
        m_pSpriteBalloonModel->setVisible(false);
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonScene::resetData()
{
    DataManagerUtil* pDMU = DataManagerUtil::sharedDataManagerUtil();
    
    m_bCheated = false;
    m_ulFrame = 0;
    m_llTotalScore = 0;
    m_nReadyTimeTime = 0;
    
    m_lTimeLeft = DEFAULT_TIME;
    
    if (pDMU)
    {
        // 设定初始的校验值
        pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
        pDMU->SetSecurityCode(SECURITY_TIME, crypto::Crc32(&m_lTimeLeft, sizeof(m_lTimeLeft)));
    }
    
    // 移除请求和云朵
    m_pLayerBalloon->removeAllChildren();
    
    // 移除道具层
    m_BalloonItemManager.clearItems();
    
    updateScore();
    updateTimeLeft();
    
    // 初始化统计数据
    m_BalloonAnalysis.initData();
    m_BalloonMissedAnalysis.initData();
}

void BalloonScene::updateScore()
{
    // m_pLabelTTFScore->setString(CCString::createWithFormat("Score: %ld", m_lTotalScore)->getCString());
    m_pLabelBMFontScore->setCString(CCString::createWithFormat("%lld", m_llTotalScore)->getCString());
}

void BalloonScene::updateTimeLeft()
{
    // m_pLabelTTFTime->setString(CCString::createWithFormat("%lus", m_lTimeLeft)->getCString());
    m_pLabelBMFontTimeLeft->setCString(CCString::createWithFormat("%ld", m_lTimeLeft)->getCString());
}

void BalloonScene::onEnter()
{
	// CCLayer::onEnter();
    AutoTextureManagerLayer::onEnter();
    
    // 启动单点触摸回调注册
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), false);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BalloonScene::notifyEnterBackground), NOTIFY_PAUSE, NULL);
    
    // 自动开始
    // startGame();
    readReadySecond();
    
    // 删除下未用到的纹理缓存
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void BalloonScene::notifyEnterBackground(CCObject* pData)
{
    if (m_eGameStatus == GAME_STATUS_RUNNING)
    {
        // 暂停界面
        unscheduleUpdate();
        
        showPauseDialog();
        
        m_eGameStatus = GAME_STATUS_PAUSE;
    }
}

void BalloonScene::onExit()
{
	// CCLayer::onExit();
	// 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFY_PAUSE);
    
    // 释放结算对话框
    // CC_SAFE_RELEASE_NULL(m_pResultDialog);
    
    // 停止场景回调
    unscheduleUpdate();
    
    // 注销单点触摸回调注册
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    
    AutoTextureManagerLayer::onExit();
}

SEL_CallFuncN BalloonScene::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonScene::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuPause", BalloonScene::onPressMenuPause);
	return NULL;
}

bool BalloonScene::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBalloonModel", CCSprite*, this->m_pSpriteBalloonModel);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontTimeLeft", CCLabelBMFont*, this->m_pLabelBMFontTimeLeft);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontScore", CCLabelBMFont*, this->m_pLabelBMFontScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerBalloon", CCLayer*, this->m_pLayerBalloon);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLayerItems", CCLayer*, this->m_pLayerItems);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuPause", CCMenu*, this->m_pMenuPause);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteScoreBar", CCSprite*, this->m_pSpriteScoreBar);

	return true;
}

SEL_CCControlHandler BalloonScene::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}


bool BalloonScene::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    m_BalloonManager.touchTest(pTouch->getLocation());

    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    return true;
}

void BalloonScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::ccTouchCanceled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::balloonHitTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite)
{
    
}

void BalloonScene::balloonTouchTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite)
{
    // 增加统计
    m_BalloonAnalysis.countWithBalloonObject(pBalloon);
    
    DataManagerUtil* pDMU = DataManagerUtil::sharedDataManagerUtil();
    
    if (!pDMU) return;
    
    // 判断气球的类型
    switch (pBalloon->getBalloonType())
    {
        case kBalloonTypeNormal:
            pBalloon->explosive();
            // 根据对应的气球分数增加到总分上
            if (!pDMU->SetSecurityData(SECURITY_SCORE, &m_llTotalScore, pBalloon->getBalloonScore()))
            {
                m_bCheated = true;
                CCMessageBox("Cheat!!!", "Warnning");
                m_llTotalScore = 0;
                return;
            }
            
            // m_lTotalScore += pBalloon->getBalloonScore();
            if (m_llTotalScore < 0)
            {
                m_llTotalScore = 0;
                pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
            }
            updateScore();
            break;
        case kBalloonTypeMulti:
            pBalloon->explosive();
            // 乘分气球，当前场景下的所有普通气球分数乘以对应的分值
            m_BalloonManager.multipleBalloonScore(pBalloon->getBalloonScore());
            break;
        case kBalloonTypeBoom:
            pBalloon->explosive();
            // 除分气球
            if (!pDMU->CheckSecurityData(SECURITY_SCORE, m_llTotalScore))
            {
                m_bCheated = true;
                // 分数有问题
                CCMessageBox("Cheat!!!", "Warnning");
                m_llTotalScore = 0;
                return;
            }
                
            m_llTotalScore /= pBalloon->getBalloonScore();
            if (m_llTotalScore < 0)
            {
                m_llTotalScore = 0;
            }
            pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
            updateScore();
            break;
        case kBalloonTypeAddTime:
            pBalloon->explosive();
            // 时间增加
            // m_lTimeLeft += pBalloon->getBalloonScore();
            
            // 校验时间数据确保未被修改
            if (!pDMU->SetSecurityData(SECURITY_TIME, &m_lTimeLeft, pBalloon->getBalloonScore()))
            {
                m_bCheated = true;
                CCMessageBox("Cheat!!!", "Warnning");
                m_lTimeLeft = 0;
            }
            updateTimeLeft();
            break;
        case kBalloonTypePump:
            pBalloon->explosive();
            // 屏幕出现打气筒按钮，并且设置按钮的小时时间
            {
                BalloonItemClick* pBalloonItem = BalloonItemClick::create(this, CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("item_pump.png"), pBalloon->getBalloonScore());
                pBalloonItem->setItemId(kBalloonItemId_Pumps);
                
                m_BalloonItemManager.setScreenBalloonItem(pBalloonItem);
            }
            
            break;
        case kBalloonTypeAddBalloon:
            break;
        case kBalloonTypeFrozen:
            pBalloon->explosive();
            // 冻结屏幕上的所有气球
            m_BalloonManager.setAllBalloonEffect(kBalloonEffectFrozen);
            // 设定统一的Y轴时间
            m_BalloonManager.setAllBalloonSpeedY(1.5f);
            break;
		case kBalloonTypeGiant:
            if (pBalloon->getBalloonClickCnt() == pBalloon->getBalloonClickableCnt())
            {
                pBalloon->explosive();
                // 根据对应的气球分数增加到总分上
                if (!pDMU->SetSecurityData(SECURITY_SCORE, &m_llTotalScore, pBalloon->getBalloonScore()))
                {
                    m_bCheated = true;
                    CCMessageBox("Cheat!!!", "Warnning");
                    m_llTotalScore = 0;
                    return;
                }
                
                // m_lTotalScore += pBalloon->getBalloonScore();
                if (m_llTotalScore < 0)
                {
                    m_llTotalScore = 0;
                    pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
                }
                updateScore();
            }
            else
            {
                pBalloon->setScale(pBalloon->getScale()*1.1f);
                BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
            }
            
			break;
		case kBalloonTypeHurricane:
			break;
		case kBalloonTypeReverse:
            pBalloon->explosive();
            // 把屏幕上的所有气球分数乘以对应的值
            m_BalloonManager.multipleBalloonScore(pBalloon->getBalloonScore());
			break;
        default:
            break;
    }
}

void BalloonScene::balloonMoveOutOfScreen(Balloon *pBalloon)
{
    // 这里可以计算哪些气球被漏过了，根据气球的属性设置对应的结算数据
    m_BalloonMissedAnalysis.countWithBalloonObject(pBalloon);
}

void BalloonScene::onBalloonItemEffectTrigger(BalloonItem* pItem)
{
    CCLOG("BalloonScene::onBalloonItemEffectTrigger(BalloonItem* pItem), called...");
    
    switch (pItem->getItemId())
    {
        case kBalloonItemId_Pumps:
            // 按一下后给屏幕上随机增加1到5分到所有的积分气球
            m_BalloonManager.addBalloonScoreWithValue(1);
            break;
        default:
            break;
    }
}

void BalloonScene::onBalloonItemBeforeDisappear(BalloonItem* pItem)
{
    CCLOG("BalloonScene::onBalloonItemBeforeDisappear(BalloonItem* pItem), called...");
}

void BalloonScene::onBalloonItemAfterDisappear(BalloonItem* pItem)
{
    CCLOG("BalloonScene::onBalloonItemAfterDisappear(BalloonItem* pItem), called...");
    // 有道具移除需要重新排列
    // m_BalloonItemManager.alignItems();
}

void BalloonScene::keyBackClicked( void )
{
	// TODO:
}

void BalloonScene::keyMenuClicked( void )
{
	// TODO:
}

void BalloonScene::update(float dt)
{
    m_ulFrame++;
    
    switch (m_eGameStatus)
    {
        case GAME_STATUS_READY:
            // 读秒状态
            break;
        case GAME_STATUS_RUNNING:
            if (m_lTimeLeft > 0 && m_ulFrame % int(1/CCDirector::sharedDirector()->getAnimationInterval()) == 0)
            {
                if (!DataManagerUtil::sharedDataManagerUtil()->SetSecurityData(SECURITY_TIME, &m_lTimeLeft, -1))
                {
                    m_bCheated = true;
                    CCMessageBox("Cheat!!!", "Warnning");
                    return;
                }
                // m_lTimeLeft--;
                updateTimeLeft();
            }
            
            // 判断游戏是否结束
            if (m_lTimeLeft == 0)
            {
                m_eGameStatus = GAME_STATUS_TIMES_UP;
            }
            
            // 随机出现气球向上飘
            if (rand()%1000 < BALLOON_SHOW_RATE)
            {
                // 生成一个气球
                m_BalloonManager.addRandomBalloon();
            }
            
            // 更新气球位置
            m_BalloonManager.updatePosition();
            
            // 随机生成云朵，随机速度
            if (rand()%1000 < CLOUD_SHOW_RATE)
            {
                m_CloudManager.addRandomCloud();
            }
            
            m_CloudManager.updatePosition();
            
            break;
        case GAME_STATUS_TIMES_UP:
            // 更新气球位置
            m_BalloonManager.updatePosition();
            
            m_CloudManager.updatePosition();
            
            // 有可能这时候点中了加时间的气球，转换到进行模式
            if (m_lTimeLeft > 0)
            {
                m_eGameStatus = GAME_STATUS_RUNNING;
                break;
            }
            
            // 判断是否还有气球在屏幕，如果没有了就
            if (!m_BalloonManager.isBalloonInScreen())
                m_eGameStatus = GAME_STATUS_STOP;
            
            break;
        case GAME_STATUS_STOP:
            unscheduleUpdate();
            
            // 移除界面上的气球和云朵
            m_pLayerBalloon->removeAllChildren();
            
            // 移除道具数据
            m_BalloonItemManager.clearItems();

            // 判断是否作弊
            if (!m_bCheated)
            {
                // 合并统计数据
                BalloonGlobalAnalysis::sharedGlobalAnalysis()->merge(m_BalloonAnalysis);
                // 存盘
                BalloonGlobalAnalysis::sharedGlobalAnalysis()->saveData();
                // m_GlobalAnalysis.merge(m_BalloonAnalysis);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                // 发送成绩到GameCenter
                GameKitHelper2dx::uploadScore(m_llTotalScore);
#endif
            }
            
            // 弹出结算框
            showResultDialog();
            break;
            
        default:
            break;
    }
    
}

void BalloonScene::showResultDialog()
{
    // 显示广告条
    GAdMob2DX::sharedGAdMob2DX()->setVisible(true);
    
	// 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    
    // 生成结算对话框
    BalloonResultDialog* pResultDialog = dynamic_cast<BalloonResultDialog*>(getChildByTag(TAG_ID_RESULT_DIALOG));
    if (!pResultDialog)
    {
        pResultDialog = BalloonResultDialog::create();
        // 设定按钮回调
        pResultDialog->m_pMenuItemReturn->setTarget(this, menu_selector(BalloonScene::onPressMenuReturnMainMenu));
        pResultDialog->m_pMenuItemReturn->setUserData((void*)TAG_ID_RESULT_DIALOG_RETURN);
        pResultDialog->m_pMenuItemPlayAgain->setTarget(this, menu_selector(BalloonScene::onPressMenuRestartGame));
        pResultDialog->m_pMenuItemPlayAgain->setUserData((void*)TAG_ID_RESULT_DIALOG_PLAYAGAIN);
        pResultDialog->m_pMenuItemShare->setTarget(this, menu_selector(BalloonScene::onPressMenuShare));
        pResultDialog->setEndCallbackFuncN(CCCallFuncN::create(this, callfuncN_selector(BalloonScene::onResultDialogEndCall)));
        pResultDialog->setTag(TAG_ID_RESULT_DIALOG);
    }
    
    // 设定面板分数
    pResultDialog->setScore(m_llTotalScore);
    std::string strHighScore = DataManagerUtil::sharedDataManagerUtil()->ReadDataFromLocal("HighestScore");
    long long llHighestScore = atoll(strHighScore.c_str());// pHighestScore->intValue();
    
    bool bNewScore = llHighestScore < m_llTotalScore;
    
    pResultDialog->setNewFlagVisible(bNewScore);
    
    if (bNewScore)
    {
        llHighestScore = m_llTotalScore;
        const std::string strHighestScore = CCString::createWithFormat("%lld", llHighestScore)->m_sString;
        DataManagerUtil::sharedDataManagerUtil()->WriteDataToLocal("HighestScore", strHighestScore);
    }
        
    pResultDialog->setHighScore(llHighestScore);
    
#if COCOS2D_DEBUG > 0
    CCLabelTTF* pLabelTTFAnalysis = dynamic_cast<CCLabelTTF*>(pResultDialog->getChildByTag(101));
    if (!pLabelTTFAnalysis)
    {
        pLabelTTFAnalysis = CCLabelTTF::create("", "", 30);
        pLabelTTFAnalysis->setTag(101);
        pLabelTTFAnalysis->setHorizontalAlignment(kCCTextAlignmentLeft);
        pLabelTTFAnalysis->setVerticalAlignment(kCCVerticalTextAlignmentBottom);
        pLabelTTFAnalysis->setDimensions(getContentSize());
        pLabelTTFAnalysis->setAnchorPoint(CCPointZero);
        pLabelTTFAnalysis->setPosition(CCPointZero);
        pResultDialog->addChild(pLabelTTFAnalysis);
    }
    pLabelTTFAnalysis->setString(m_BalloonAnalysis.dumpDebugInfo().c_str());
#endif
    
    addChild(pResultDialog);
}

void BalloonScene::onPressMenuRestartGame(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCNode* pNode = dynamic_cast<CCNode*>(pSender);
    if (!pNode) return;
    /*
    // 移除按钮层
    pNode->getParent()->removeFromParent();
    */
    
    int nValue = int(pNode->getUserData());
    switch (nValue)
    {
        case TAG_ID_RESULT_DIALOG_PLAYAGAIN:
        {
            BalloonResultDialog* pResultDialog = dynamic_cast<BalloonResultDialog*>(getChildByTag(TAG_ID_RESULT_DIALOG));
            if (pResultDialog)
                pResultDialog->endDialog();
            break;
        }
        case TAG_ID_PAUSE_DIALOG_PLAYAGAIN:
        {
            BalloonPauseDialog* pPauseDialog = dynamic_cast<BalloonPauseDialog*>(getChildByTag(TAG_ID_PAUSE_DIALOG));
            if (pPauseDialog)
                pPauseDialog->endDialog();
            break;
        }
        default:
            break;
    }
    
    // startGame();
    readReadySecond();
}

void BalloonScene::onPressMenuReturnMainMenu(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    CCNode* pNode = dynamic_cast<CCNode*>(pSender);
    if (!pNode) return;
    
    DialogLayer* pDialog = NULL;
    int nValue = int(pNode->getUserData());
    switch (nValue)
    {
        case TAG_ID_RESULT_DIALOG_RETURN:
            pDialog = dynamic_cast<DialogLayer*>(getChildByTag(TAG_ID_RESULT_DIALOG));
            break;
        case TAG_ID_PAUSE_DIALOG_RETURN:
            pDialog = dynamic_cast<DialogLayer*>(getChildByTag(TAG_ID_PAUSE_DIALOG));
            break;
        default:
            break;
    }
    // 把对话框删掉
    if (pDialog && pDialog->getParent())
        pDialog->removeFromParent();
    
    CCDirector::sharedDirector()->popScene();
}

void BalloonScene::onPressMenuShare(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCDictionary* pDictData = CCDictionary::create();
    
    BalloonResultDialog* pDialog = dynamic_cast<BalloonResultDialog*>(getChildByTag(TAG_ID_RESULT_DIALOG));
    if (!pDialog) return;
    
    // 截取得分图片
    std::string strPath = pDialog->getSharedPictureFilePath();
    
    // pDictData->setObject(ccs("一起来【气球大作战】吧～伸出你的指头，释放你的压力"), "shareText");
    const char* pszKey = "high_score_shares";
    if (m_llTotalScore <= 1)
    {
        // 采用单数模式的字符串
        pszKey = "high_score_share";
    }
    pDictData->setObject(CCString::createWithFormat(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("share_section", pszKey), m_llTotalScore), "shareText");
    pDictData->setObject(ccs(strPath.c_str()), "shareImage");
    UMSocial2DX::shareSNS(pDictData);
}

void BalloonScene::onPressMenuPause(cocos2d::CCObject *pSender)
{
    // 校验下是否在运行状态
    if (m_eGameStatus != GAME_STATUS_RUNNING)
        return;
    
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    // 暂停游戏
    unscheduleUpdate();
    
    // 显示暂停界面
    showPauseDialog();
    
    // 暂停
    m_eGameStatus = GAME_STATUS_PAUSE;
}

void BalloonScene::onPressMenuResume(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    m_eGameStatus = GAME_STATUS_RUNNING;
    
    // 移除暂停对话框
    BalloonPauseDialog* pDialog = dynamic_cast<BalloonPauseDialog*>(getChildByTag(TAG_ID_PAUSE_DIALOG));
    if (pDialog)
        pDialog->endDialog();
    
    // 继续游戏
    scheduleUpdate();
    
    // 隐藏广告条
    GAdMob2DX::sharedGAdMob2DX()->setVisible(false);
}

void BalloonScene::onResultDialogEndCall(CCNode* pNode)
{
    
}

void BalloonScene::timeCountCallback(CCNode* pNode)
{
    m_nReadyTimeTime++;
    if (m_nReadyTimeTime == 3)
    {
        pNode->removeFromParent();
        startGame();
    }
    else
    {
        CCLabelBMFont* pLabel = dynamic_cast<CCLabelBMFont*>(pNode);
        if (pLabel)
        {
            pLabel->setString(CCString::createWithFormat("%d", 3 - m_nReadyTimeTime)->getCString());
            pNode->setScale(6.0f);
            pNode->runAction(CCSequence::create(CCScaleTo::create(1.0f, 0), CCCallFuncN::create(this, callfuncN_selector(BalloonScene::timeCountCallback)), NULL));
        }
    }
}

void BalloonScene::readReadySecond()
{
    // 启动读秒回调
    CCLabelBMFont* pLabelBMFontTime = CCLabelBMFont::create("3", "texture/fonts/font.fnt");
    pLabelBMFontTime->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
    pLabelBMFontTime->setScale(6.0f);
    addChild(pLabelBMFontTime);
    pLabelBMFontTime->runAction(CCSequence::create(CCScaleTo::create(1.0f, 0), CCCallFuncN::create(this, callfuncN_selector(BalloonScene::timeCountCallback)), NULL));
}

void BalloonScene::startGame()
{
    MobClickCpp::event("StartGameCount");
    
    // 开始游戏隐藏广告条
    GAdMob2DX::sharedGAdMob2DX()->setVisible(false);
    
    resetData();
    
    m_eGameStatus = GAME_STATUS_RUNNING;
    
    scheduleUpdate();
}

void BalloonScene::showPauseDialog()
{
    BalloonPauseDialog* pPauseDialog = dynamic_cast<BalloonPauseDialog*>(getChildByTag(TAG_ID_PAUSE_DIALOG));
    if (!pPauseDialog)
    {
        pPauseDialog = BalloonPauseDialog::create();
        
        // 绑定按钮效果
        pPauseDialog->m_pMenuItemAgain->setTarget(this, menu_selector(BalloonScene::onPressMenuRestartGame));
        pPauseDialog->m_pMenuItemAgain->setUserData((void*)TAG_ID_PAUSE_DIALOG_PLAYAGAIN);
        pPauseDialog->m_pMenuItemReturn->setTarget(this, menu_selector(BalloonScene::onPressMenuReturnMainMenu));
        pPauseDialog->m_pMenuItemReturn->setUserData((void*)TAG_ID_PAUSE_DIALOG_RETURN);
        pPauseDialog->m_pMenuItemResume->setTarget(this, menu_selector(BalloonScene::onPressMenuResume));
    }
    
    addChild(pPauseDialog);
    GAdMob2DX::sharedGAdMob2DX()->setVisible(true);
}

bool BalloonScene::setResourceString()
{
    m_vTexturesString.push_back("texture/cloud/cloud.plist");
    m_vTexturesString.push_back("texture/balloon/balloon.plist");
    m_vTexturesString.push_back("texture/balloonScene/balloonScene.plist");
    m_vTexturesString.push_back("texture/items/items.plist");
//    m_vTexturesString.push_back("texture/menuItems/menuItems.plist");
    m_vTexturesString.push_back("texture/balloonEffect/balloon_effect_frozen.plist");
    m_vTexturesString.push_back("texture/balloonScene/background.png");
    
    return true;
}
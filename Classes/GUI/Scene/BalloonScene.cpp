#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "MobClickCpp.h"
#include "UMSocial2DX.h"
#include "BalloonPauseDialog.h"
#include "BalloonResultDialog.h"
#include "BalloonItemSelectDialog.h"
#include "UserData.h"
#include "BalloonMission.h"
#include "NDKBridge.h"

#include "bailinUtil.h"
#include "MemBufferObject.h"
#include "CCJSONConverter.h"

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

#define NOTIFY_HTTP_COMMIT_DATA_CALLBACK_NAME "HTTP_UPDATE_SCORE"

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
        
        // 设置能量动画，不停播放
        CCArray* pArrayAnime = CCArray::createWithCapacity(6);
        for (int idx = 1; idx <= 6; idx++)
        {
            pArrayAnime->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("energy_fire%d.png", idx)->getCString()));
        }
        CCAnimation* pAnimation = CCAnimation::createWithSpriteFrames(pArrayAnime, 0.1f);
        CCAnimate* pAnimate = CCAnimate::create(pAnimation);
        m_pSpriteEnergyFire->runAction(CCRepeatForever::create(pAnimate));
        
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
    
    long lExTimeLeft = 0;
    if (m_bUseItemExPreTime)
    {
        // 获得额外5秒钟
        lExTimeLeft = 5;
    }
    m_lTimeLeft = DEFAULT_TIME + lExTimeLeft;
    
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
    UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->initData();
}

void BalloonScene::updateScore()
{
    // m_pLabelTTFScore->setString(CCString::createWithFormat("Score: %ld", m_lTotalScore)->getCString());
    m_pLabelBMFontScore->setCString(CCString::createWithFormat("%lld", m_llTotalScore)->getCString());
    // 更新能量球位置
    m_pSpriteEnergyFire->setPosition(ccp(m_pLabelBMFontScore->getPositionX() - m_pLabelBMFontScore->boundingBox().size.width, m_pLabelBMFontScore->getPositionY()));
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
    
    // 注册游戏暂停的消息
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BalloonScene::notifyEnterBackground), NOTIFY_PAUSE, NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(BalloonScene::notifyHttpCallbackUpdateScore), NOTIFY_HTTP_COMMIT_DATA_CALLBACK_NAME, NULL);
    
    
    // 自动开始
    // startGame();
    // readReadySecond();
    showBuyItemsDialog();
    
    // 背景音乐降低
    BalloonSoundManager::sharedBalloonSoundManager()->setBackgroundMusicVolume(0.2f);
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
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFY_HTTP_COMMIT_DATA_CALLBACK_NAME);
    
    // 释放结算对话框
    // CC_SAFE_RELEASE_NULL(m_pResultDialog);
    
    BalloonSoundManager::sharedBalloonSoundManager()->setBackgroundMusicVolume(1.0f);
    
    // 停止场景回调
    unscheduleUpdate();
    
    UserDataManager::sharedUserDataManager()->saveData();
    
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
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteEnergyFire", CCSprite*, this->m_pSpriteEnergyFire);
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

#if COCOS2D_DEBUG > 1
    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
#endif
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
    DataManagerUtil* pDMU = DataManagerUtil::sharedDataManagerUtil();
    
    if (!pDMU) return;
    
    // 判断气球的类型
    switch (pBalloon->getBalloonType())
    {
        case kBalloonTypeNormal:
            pBalloon->explosive();
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            // 根据对应的气球分数增加到总分上
            if (!pDMU->SetSecurityData(SECURITY_SCORE, &m_llTotalScore, (long long)pBalloon->getBalloonScore()))
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
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            // 乘分气球，当前场景下的所有普通气球分数乘以对应的分值
            m_BalloonManager.multipleBalloonScore(pBalloon->getBalloonScore());
            break;
        case kBalloonTypeBoom:
            pBalloon->explosive();
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            {
                // 除分气球
                if (!pDMU->CheckSecurityData(SECURITY_SCORE, m_llTotalScore))
                {
                    m_bCheated = true;
                    // 分数有问题
                    CCMessageBox("Cheat!!!", "Warnning");
                    m_llTotalScore = 0;
                    return;
                }
                
                // 返还比例
                if (m_bUseItemExSafeGuard)
                {
                    m_llTotalScore -= m_llTotalScore/pBalloon->getBalloonScore()/2;
                }
                else
                {
                    m_llTotalScore /= pBalloon->getBalloonScore();
                }
                                       
                if (m_llTotalScore < 0)
                {
                    m_llTotalScore = 0;
                }
                pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
                updateScore();
            }
            break;
        case kBalloonTypeAddTime:
            pBalloon->explosive();
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            {
                // 时间增加
                long lTimeItemFix = 0;
                if (m_bUseItemExTimeAdd) lTimeItemFix = 2;
                // 校验时间数据确保未被修改
                if (!pDMU->SetSecurityData(SECURITY_TIME, &m_lTimeLeft, pBalloon->getBalloonScore() + lTimeItemFix))
                {
                    m_bCheated = true;
                    CCMessageBox("Cheat!!!", "Warnning");
                    m_lTimeLeft = 0;
                }
                updateTimeLeft();
            }
            break;
        case kBalloonTypePump:
            pBalloon->explosive();
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            // 屏幕出现打气筒按钮，并且设置按钮的小时时间
            {
                BalloonItemClick* pBalloonItem = BalloonItemClick::create(this, CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("item_pump.png"), pBalloon->getBalloonScore());
                // 这里设置充气筒按下后的值，从1开始，如果有使用道具啥的就在此基础往上累加
                pBalloonItem->setItemValue(1);
                
                pBalloonItem->setItemId(kBalloonItemId_Pumps);
                
                m_BalloonItemManager.setScreenBalloonItem(pBalloonItem);
            }
            
            break;
        case kBalloonTypeAddBalloon:
            break;
        case kBalloonTypeFrozen:
            pBalloon->explosive();
            // 增加统计
            UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
            // 冻结屏幕上的所有气球
            m_BalloonManager.setAllBalloonEffect(kBalloonEffectFrozen);
            // 设定统一的Y轴时间
            m_BalloonManager.setAllBalloonSpeedY(1.5f);
            break;
		case kBalloonTypeGiant:
            if (m_bUseItemExGiant)
            {
                // 每按一下，就加1分到总分上
                if (!pDMU->SetSecurityData(SECURITY_SCORE, &m_llTotalScore, 1))
                {
                    m_bCheated = true;
                    CCMessageBox("Cheat!!!", "Warnning");
                    m_llTotalScore = 0;
                    return;
                }
                
                if (m_llTotalScore < 0)
                {
                    m_llTotalScore = 0;
                    pDMU->SetSecurityCode(SECURITY_SCORE, crypto::Crc32(&m_llTotalScore, sizeof(m_llTotalScore)));
                }
                updateScore();
            }
            
            if (pBalloon->getBalloonClickCnt() == pBalloon->getBalloonClickableCnt())
            {
                pBalloon->explosive();
                // 增加统计
                UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
                // 根据对应的气球分数增加到总分上
                if (!pDMU->SetSecurityData(SECURITY_SCORE, &m_llTotalScore, (long long)pBalloon->getBalloonScore()))
                {
                    m_bCheated = true;
                    CCMessageBox("Cheat!!!", "Warnning");
                    m_llTotalScore = 0;
                    return;
                }
                
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
            {
                pBalloon->explosive();
                // 增加统计
                UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->countWithBalloonObject(pBalloon);
            
                // 把屏幕上的所有气球分数乘以对应的值
                int nScore = pBalloon->getBalloonScore();
                int nExtendValue = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong("reverseEffectExtendValue");
                if (nExtendValue)
                {
                    nScore *= nExtendValue;
                }
                m_BalloonManager.multipleBalloonScore(nScore);
            }
            break;
        default:
            break;
    }
}

void BalloonScene::balloonMoveOutOfScreen(Balloon *pBalloon)
{
    // 这里可以计算哪些气球被漏过了，根据气球的属性设置对应的结算数据
    // m_BalloonMissedAnalysis.countWithBalloonObject(pBalloon);
}

void BalloonScene::onBalloonItemEffectTrigger(BalloonItem* pItem)
{
    CCLOG("BalloonScene::onBalloonItemEffectTrigger(BalloonItem* pItem), called...");
    
    switch (pItem->getItemId())
    {
        case kBalloonItemId_Pumps:
            // 按一下后给屏幕上的普通气球增加分数
            {
                long lExScore = 0;
                if (m_bUseItemExPump)
                {
                    lExScore = 1;
                }
                m_BalloonManager.addBalloonScoreWithValue(pItem->getItemValue() + lExScore);
            }
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
                if (!DataManagerUtil::sharedDataManagerUtil()->SetSecurityData(SECURITY_TIME, &m_lTimeLeft, (long)-1))
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
                // 游戏盘数增加一盘
                UserDataManager::sharedUserDataManager()->addOneGameCount();
                // 合并统计数据
                UserDataManager::sharedUserDataManager()->getGlobalAnalysisDataRef()->merge(*UserDataManager::sharedUserDataManager()->getAnalysisDataRef());
                // 存盘
                // UserDataManager::sharedUserDataManager()->getGlobalAnalysisDataRef()->saveData();
                
                // 记录得到的金币
                long long llCoins = m_llTotalScore/SCORE_COINS_RATE;
                // TODO: 这里还需要加上任务完成的奖励
                UserDataManager::sharedUserDataManager()->addGoldenCoins(llCoins);
                
                commitScoreToServer();
                
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
    NDKBridge::sharedNDKBridge()->setAdMobVisible(true);
    
    // 生成结算对话框
    BalloonResultDialog* pResultDialog = dynamic_cast<BalloonResultDialog*>(getChildByTag(TAG_ID_RESULT_DIALOG));
    if (!pResultDialog)
    {
        pResultDialog = BalloonResultDialog::create();
        // 设定按钮回调
        pResultDialog->m_pMenuItemPlayAgain->setTarget(this, menu_selector(BalloonScene::onPressMenuRestartGame));
        pResultDialog->m_pMenuItemPlayAgain->setUserData((void*)TAG_ID_RESULT_DIALOG_PLAYAGAIN);
        pResultDialog->setEndCallbackFuncN(CCCallFuncN::create(this, callfuncN_selector(BalloonScene::onResultDialogEndCall)));
        pResultDialog->setTag(TAG_ID_RESULT_DIALOG);
    }
    
    // 设定面板分数
    pResultDialog->setScore(m_llTotalScore);
    long long llHighestScore = UserDataManager::sharedUserDataManager()->getHighestScore();
    
    bool bNewScore = llHighestScore < m_llTotalScore;
    
    pResultDialog->setNewFlagVisible(bNewScore);
    
    if (bNewScore)
    {
        llHighestScore = m_llTotalScore;
        UserDataManager::sharedUserDataManager()->setHighestScore(llHighestScore);
    }
        
    pResultDialog->setHighScore(llHighestScore);
    
#if COCOS2D_DEBUG > 2
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
    pLabelTTFAnalysis->setString(UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->dumpDebugInfo().c_str());
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // 首先判断下当前用户是否点击过评论
    if (0)
    {
        // 玩5盘弹出评论对话框
        long long llGameCounts = UserDataManager::sharedUserDataManager()->getGameCounts();
        if (llGameCounts > 20 && llGameCounts % 20)
        {
            GameKitHelper2dx::showRateMessageBox();
        }
    }
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
    // readReadySecond();
    showBuyItemsDialog();
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
    // GAdMob2DX::sharedGAdMob2DX()->setVisible(false);
    NDKBridge::sharedNDKBridge()->setAdMobVisible(false);
}

void BalloonScene::onResultDialogEndCall(CCNode* pNode)
{
    
}

void BalloonScene::onBuyItemsDialogEndCall(CCNode* pNode)
{
    // 根据全局数据设定当前的道具影响
    m_bUseItemExPump = UserDataManager::sharedUserDataManager()->getItemExCountsByID(kCCItemExTypePump) > 0;
    m_bUseItemExPreTime = UserDataManager::sharedUserDataManager()->getItemExCountsByID(kCCItemExTypePreTime) > 0;
    m_bUseItemExTimeAdd = UserDataManager::sharedUserDataManager()->getItemExCountsByID(kCCItemExTypeTimeAdd) > 0;
    m_bUseItemExGiant = UserDataManager::sharedUserDataManager()->getItemExCountsByID(kCCItemExTypeGiant) > 0;
    m_bUseItemExSafeGuard = UserDataManager::sharedUserDataManager()->getItemExCountsByID(kCCItemExTypeSafeGuard) > 0;
    
    if (m_bUseItemExPump)
    {
        // 计算打气筒的消耗道具
        UserDataManager::sharedUserDataManager()->addItemExCountsByID(kCCItemExTypePump, -1);
    }
    
    if (m_bUseItemExPreTime)
    {
        UserDataManager::sharedUserDataManager()->addItemExCountsByID(kCCItemExTypePreTime, -1);
    }
    
    if (m_bUseItemExTimeAdd)
    {
        UserDataManager::sharedUserDataManager()->addItemExCountsByID(kCCItemExTypeTimeAdd, -1);
    }
    
    if (m_bUseItemExGiant)
    {
        UserDataManager::sharedUserDataManager()->addItemExCountsByID(kCCItemExTypeGiant, -1);
    }
    
    if (m_bUseItemExSafeGuard)
    {
        UserDataManager::sharedUserDataManager()->addItemExCountsByID(kCCItemExTypeSafeGuard, -1);
    }
    
    // 读秒开始
    readReadySecond();
}

void BalloonScene::showBuyItemsDialog()
{
    BalloonItemSelectDialog* pDialog = BalloonItemSelectDialog::create();
    // 由小到达进场
    pDialog->setEndCallbackFuncN(CCCallFuncN::create(this, callfuncN_selector(BalloonScene::onBuyItemsDialogEndCall)));
    addChild(pDialog);
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
    resetData();
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
    // GAdMob2DX::sharedGAdMob2DX()->setVisible(false);
    NDKBridge::sharedNDKBridge()->setAdMobVisible(false);
    
    // resetData();
    
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
        pPauseDialog->setTag(TAG_ID_PAUSE_DIALOG);
    }
    
    addChild(pPauseDialog);
    NDKBridge::sharedNDKBridge()->setAdMobVisible(true);
}

bool BalloonScene::setResourceString()
{
    m_vTexturesString.push_back("texture/cloud/cloud.plist");
    m_vTexturesString.push_back("texture/balloon/balloon.plist");
    m_vTexturesString.push_back("texture/balloonScene/balloonScene.plist");
    m_vTexturesString.push_back("texture/animation/animation.plist");
    m_vTexturesString.push_back("texture/items/items.plist");
//    m_vTexturesString.push_back("texture/menuItems/menuItems.plist");
    m_vTexturesString.push_back("texture/balloonEffect/balloon_effect_frozen.plist");
    m_vTexturesString.push_back("texture/balloonScene/background.png");
    
    return true;
}

void BalloonScene::notifyHttpCallbackUpdateScore(cocos2d::CCObject *pData)
{
    MemBufferObject* pMemObj = (MemBufferObject*)pData;
    if (!pMemObj)
        return;
    
    // JSON字符串转化为CCDictionary对象
    std::string strJSON(pMemObj->GetBuffer(), pMemObj->GetBufferLen());
    if (!strJSON.empty())
    {
        CCDictionary* pDict = CCJSONConverter::sharedConverter()->dictionaryFrom(strJSON.c_str());
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        CCLOG("HTTP response msg is: %s", pDict->valueForKey("msg")->getCString());
#endif
    }
}

void BalloonScene::commitScoreToServer()
{
    // 把得分上传到服务器
    CCDictionary* pDict = CCDictionary::create();
    // 获得当前用户的UDID
    pDict->setObject(ccs(UserDataManager::sharedUserDataManager()->getDeviceUDID().c_str()), "uid");
    // 获得当前用户的用户名
    pDict->setObject(ccs(UserDataManager::sharedUserDataManager()->getNickName().c_str()), "uname");
    CCArray* pArray = CCArray::create();
    
    // for many score type
    {
        CCDictionary* pDictArrayData = CCDictionary::create();
        // 上传分数类型
        pDictArrayData->setObject(ccs("1"), "type");
        // 当前比赛得分
        pDictArrayData->setObject(CCString::createWithFormat("%lld", m_llTotalScore), "score");
        pArray->addObject(pDictArrayData);
    }
    
    // 把数组插进去，作为一个数据项
    pDict->setObject(pArray, "data");
    
    const char* pszJSON = CCJSONConverter::sharedConverter()->strFrom(pDict);
    // 请求Http数据
    HttpCenter::sharedHttpCenter()->request("http://pokeballoon.renqiz.com/score_update.php", NOTIFY_HTTP_COMMIT_DATA_CALLBACK_NAME, CCHttpRequest::kHttpPost, pszJSON);
    free((void*)pszJSON);
    
}
#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "MobClickCpp.h"
#include "GAdMob2DX.h"

#include "bailinUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define BALLOON_SHOW_RATE    18     // 气球出现的概率，10表示1%，50就是5%
#define CLOUD_SHOW_RATE      5
#define DEFAULT_TIME         30

BalloonScene::~BalloonScene()
{
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
		CC_BREAK_IF(!CCLayer::init());
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
        
        resetData();
        
        CCMenuItemFont::setFontSize(getContentSize().width*0.1f);
        
        m_pSpriteBackground->setScaleX(getContentSize().width/m_pSpriteBackground->getContentSize().width);
        m_pSpriteBackground->setScaleY(getContentSize().height/m_pSpriteBackground->getContentSize().height);
		
		setKeypadEnabled(true);
        
        if (!m_BalloonManger.getInitFlag())
            m_BalloonManger.init(this, m_pLayerBalloon);
        
        if (!m_CloudManager.getInitFlag())
            m_CloudManager.init(m_pLayerBalloon);
        
        m_pSpriteBalloonModel->setVisible(false);
        
        BalloonSoundManager::sharedBalloonSoundManager();
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonScene::resetData()
{
    m_ulFrame = 0;
    m_lTotalScore = 0;
    
    m_ulTimeLeft = DEFAULT_TIME;
    
    updateScore();
    updateTimeLeft();
}

void BalloonScene::updateScore()
{
    // m_pLabelTTFScore->setString(CCString::createWithFormat("Score: %ld", m_lTotalScore)->getCString());
    m_pLabelBMFontScore->setCString(CCString::createWithFormat("%ld", m_lTotalScore)->getCString());
}

void BalloonScene::updateTimeLeft()
{
    // m_pLabelTTFTime->setString(CCString::createWithFormat("%lus", m_ulTimeLeft)->getCString());
    m_pLabelBMFontTimeLeft->setCString(CCString::createWithFormat("%lu", m_ulTimeLeft)->getCString());
}

void BalloonScene::onEnter()
{
	CCLayer::onEnter();
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    createResultDialog();
    
    // 启动单点触摸回调注册
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), false);
    
    startGame();
    
}

void BalloonScene::onExit()
{
	CCLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
    
    // 释放结算对话框
    CC_SAFE_RELEASE_NULL(m_pResultDialog);
    
    // 停止场景回调
    unscheduleUpdate();
    
    // 注销单点触摸回调注册
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

SEL_CallFuncN BalloonScene::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonScene::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonScene::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBalloonModel", CCSprite*, this->m_pSpriteBalloonModel);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelTTFScore", CCLabelTTF*, this->m_pLabelTTFScore);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelTTFTime", CCLabelTTF*, this->m_pLabelTTFTime);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontTimeLeft", CCLabelBMFont*, this->m_pLabelBMFontTimeLeft);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontScore", CCLabelBMFont*, this->m_pLabelBMFontScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLayerBalloon", CCLayer*, this->m_pLayerBalloon);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);

	return true;
}

SEL_CCControlHandler BalloonScene::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonScene::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    m_BalloonManger.touchTest(pTouch->getLocation());
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
    // 增加粒子特效
    CCParticleSystemQuad* pParticleExplosive = CCParticleSystemQuad::create("particles/explosive.plist");
    pParticleExplosive->setAutoRemoveOnFinish(true);
    pParticleExplosive->setPosition(pBalloon->getPosition());
    pParticleExplosive->setEndColor(pBalloon->getBalloonColor4F());
    addChild(pParticleExplosive);
    
    switch (pBalloon->getBalloonType())
    {
        case kBalloonTypeNormal:
            // 根据对应的气球分数增加到总分上
            m_lTotalScore += pBalloon->getBalloonScore();
            if (m_lTotalScore < 0)
            {
                m_lTotalScore = 0;
            }
            updateScore();
            break;
        case kBalloonTypeMulti:
            // 乘分气球，当前场景下的所有普通气球分数乘
            m_BalloonManger.multipleBalloonScore(pBalloon->getBalloonScore());
            break;
        case kBalloonTypeDiv:
            // 除分气球
            m_lTotalScore /= pBalloon->getBalloonScore();
            if (m_lTotalScore < 0)
            {
                m_lTotalScore = 0;
            }
            updateScore();
            break;
        case kBalloonTypeAddTime:
            // 时间增加
            m_ulTimeLeft += pBalloon->getBalloonScore();
            updateTimeLeft();
            break;
            
        default:
            break;
    }
    
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
            break;
        case GAME_STATUS_RUNNING:
            if (m_ulTimeLeft > 0 && m_ulFrame % int(1/CCDirector::sharedDirector()->getAnimationInterval()) == 0)
            {
                m_ulTimeLeft--;
                updateTimeLeft();
            }
            
            // 判断游戏是否结束
            if (m_ulTimeLeft == 0)
            {
                m_eGameStatus = GAME_STATUS_STOP;
            }
            
            // 随机出现气球向上飘
            if (rand()%1000 < BALLOON_SHOW_RATE)
            {
                // 生成一个气球
                m_BalloonManger.addRandomBalloon();
            }
            
            // 更新气球位置
            m_BalloonManger.updatePosition();
            
            // 随机生成云朵，随机速度
            if (rand()%1000 < CLOUD_SHOW_RATE)
            {
                m_CloudManager.addRandomCloud();
            }
            
            m_CloudManager.updatePosition();
            
            break;
        case GAME_STATUS_STOP:
            unscheduleUpdate();
            
            m_pLayerBalloon->removeAllChildren();
            
            // 弹出结算框
            showResultDialog();
            
            // 显示广告条
            GAdMob2DX::sharedGAdMob2DX()->setVisible(true);
            
            break;
        default:
            break;
    }
    
}

void BalloonScene::showResultDialog()
{
    /*
    CCMenuItemImage* pMenuItemRestart = CCMenuItemImage::create("balloon/menu_item_restart.png", "balloon/menu_item_restart.png", this, menu_selector(BalloonScene::onPressMenuRestartGame));
    
    pMenuItemRestart->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(3.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(3.0f, -10.0f), NULL)));
    
    CCMenuItemImage* pMenuItemReturnMainMenu = CCMenuItemImage::create("balloon/menu_item_return_menu.png", "balloon/menu_item_return_menu.png", this, menu_selector(BalloonScene::onPressMenuReturnMainMenu));
    
    pMenuItemReturnMainMenu->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(3.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(3.0f, -10.0f), NULL)));
    
    CCMenu* pMenu = CCMenu::create(pMenuItemRestart, pMenuItemReturnMainMenu, NULL);
    pMenu->alignItemsVerticallyWithPadding(pMenuItemRestart->getContentSize().height);
    
    CCPoint posStart = ccp(getContentSize().width*0.5f, getContentSize().height);
    CCPoint posEnd = ccpMult(ccpFromSize(getContentSize()), 0.5f);
    pMenu->setPosition(posStart);
    
    pMenu->runAction(CCEaseBounceOut::create(CCMoveTo::create(1.0f, posEnd)));
    
    addChild(pMenu);
    */
   
    
    // 设定面板分数
    m_pResultDialog->setScore(m_lTotalScore);
    CCString* pHighestScore = ccs(DataManagerUtil::sharedDataManagerUtil()->ReadDataFromLocal("HighestScore").c_str());
    long lHighestScore = pHighestScore->intValue();
    
    bool bNewScore = lHighestScore < m_lTotalScore;
    
    m_pResultDialog->setNewFlagVisible(bNewScore);
    
    if (bNewScore)
    {
        lHighestScore = m_lTotalScore;
        const std::string strHighestScore = CCString::createWithFormat("%ld", lHighestScore)->m_sString;
        DataManagerUtil::sharedDataManagerUtil()->WriteDataToLocal("HighestScore", strHighestScore);
    }
        
    m_pResultDialog->setHighScore(lHighestScore);
    
    addChild(m_pResultDialog);
    
}

void BalloonScene::onPressMenuRestartGame(cocos2d::CCObject *pSender)
{
    /*
    CCNode* pNode = dynamic_cast<CCNode*>(pSender);
    if (!pNode) return;
    
    // 移除按钮层
    pNode->getParent()->removeFromParent();
    */
    
    m_pResultDialog->endDialog();
    
    startGame();
}

void BalloonScene::onPressMenuReturnMainMenu(cocos2d::CCObject *pSender)
{
    m_pResultDialog->endDialog();
    
    CCDirector::sharedDirector()->popScene();
}

void BalloonScene::onResultDialogEndCall(CCNode* pNode)
{
    
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

void BalloonScene::createResultDialog()
{
    if (!m_pResultDialog)
    {
        // 生成结算对话框
        m_pResultDialog = BalloonOptionsDialog::create();
        
        // 设定按钮回调
        m_pResultDialog->m_pMenuItemReturn->setTarget(this, menu_selector(BalloonScene::onPressMenuReturnMainMenu));
        m_pResultDialog->m_pMenuItemPlayAgain->setTarget(this, menu_selector(BalloonScene::onPressMenuRestartGame));
        m_pResultDialog->setEndCallbackFuncN(CCCallFuncN::create(this, callfuncN_selector(BalloonScene::onResultDialogEndCall)));
        
        // 保存对话框
        CC_SAFE_RETAIN(m_pResultDialog);
    }
}
#include "BalloonResultDialog.h"
#include "bailinUtil.h"
#include "UserData.h"
#include "BalloonMission.h"
#include <string>
#include "BalloonSoundManager.h"
#include "UMSocial2DX.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define SNS_SHARE_IMAGE_FILE_NAME "sns_share.png"

BalloonResultDialog::~BalloonResultDialog()
{
    
}

bool BalloonResultDialog::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!DialogLayer::init());
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonResultDialog.ccbi";

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
		
        initMenuTop();
        
        pushMenu(m_pMenuShare);
        
        // 设置按钮按下后变大的效果
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuShare);
        
        /*
        // 初始化位置
        m_posOri = m_pSpriteResultBoard->getPosition();
        CCPoint posUp = ccp(m_pSpriteResultBoard->getPositionX(), getContentSize().height*2);
        
        // 进场动画
        m_pSpriteResultBoard->setPosition(posUp);
        
        addBoardChildWithAction(m_pSpriteResultBoard,
                                CCEaseBounceOut::create(CCMoveTo::create(0.5f, m_posOri)),
                                CCEaseExponentialIn::create(CCMoveTo::create(0.2f, posUp)));
        SetVisibleBoard(NULL);
        */
        m_pSpriteResultBoard->setScale(0.1f);
        setOnEnterAction(CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)));
        setOnExitAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3f, 0.1f)));
        m_pMainBoard = m_pSpriteResultBoard;
        
        // 更新分享按钮数据
        m_posShareItemOri = m_pMenuItemShare->getPosition();
        m_fShareItemAngleOri = m_pMenuItemShare->getRotation();
        // 默认隐藏新记录标志
        setNewFlagVisible(false);
        
        initLabelTTF();
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonResultDialog::initLabelTTF()
{
    ControlUtil::sharedControlUtil()->FitLabelTTFFontSize(m_pLabelTTFMissionDesc);
}

void BalloonResultDialog::onEnter()
{
	DialogLayer::onEnter();
    addFireworks(rand()%3+3);
    
    m_pMenuItemShare->stopAllActions();
    m_pMenuItemShare->setPosition(m_posShareItemOri);
    m_pMenuItemShare->setRotation(m_fShareItemAngleOri);
    
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.5f, 30.0f), CCRotateBy::create(1.5f, -30.0f), NULL)));
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(2.7f, ccp(0, m_pMenuItemShare->getContentSize().height*0.3f)), CCMoveBy::create(2.5f, ccp(0, -m_pMenuItemShare->getContentSize().height*0.3f)), NULL)));
    
    initRotateStar();
    
    // 当前用户拥有金币数量
    m_llUserCoins = UserDataManager::sharedUserDataManager()->getGoldenCoins();
    // 得到当前局分数转换得到的金币数量
    m_lCoinsWithScores = m_llTotalScore/SCORE_COINS_RATE;
    m_lCoinsWithScoresCnt = m_lCoinsWithScores;
    // 显示的时候先减去奖励的金币
    m_llUserCoins -= m_lCoinsWithScores;
    
    // 计算是否完成任务
    const Mission* pMission = BalloonMission::sharedBalloonMission()->getRandomMission();
    // 设置任务描述
    m_pLabelTTFMissionDesc->setString(pMission->strMissionDesc.c_str());
    // 更新任务奖励数值
    m_pLabelBMFontMissionReward->setString(CCString::createWithFormat("%d", pMission->nReward)->getCString());
    
    // 这里生成一个任务结构体，用于比对是否完成
    MissionData md;
    // 当前分数赋值
    md.score = m_llTotalScore;
    // 拷贝当前盘的数据
    md.analysisData = UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->getAnalysisData();
    
    if (pMission && pMission->isMissionComplete(md))
    {
        // 完成任务，显示奖励
        CCLOG("Mission Complete!!");
        // 增加完成任务的奖励，目前应该只增加金币
        if (pMission->cbRewardType == kCCMissionRewardTypeGoldenCoins)
        {
            /*
            // 把金币跳到屏幕上
            CCSprite* pSpirteCoin = CCSprite::createWithSpriteFrame(m_pSpriteCoin->displayFrame());
            CCLabelBMFont* pBonus = CCLabelBMFont::create(CCString::createWithFormat("%d", pMission->nReward)->getCString(), "texture/fonts/font.fnt");
            pBonus->setAnchorPoint(ccp(0, 0.5f));
            pBonus->setPosition(ccp(pSpirteCoin->getContentSize().width, 0));
            CCNode* pNode = CCNode::create();
            pNode->addChild(pSpirteCoin);
            pNode->addChild(pBonus);
            this->addChild(pNode);
            
            pNode->setPosition(ccp(0, getContentSize().height*0.5f));
            pNode->runAction(CCSequence::create(CCMoveTo::create(5.0f, ccp(this->getContentSize().width*1.2f, pNode->getPositionY())), CCRemoveSelf::create(), NULL));
            */
            BalloonMission::sharedBalloonMission()->setNeedNewMission(true);
            m_bMissionCompleted = true;
            m_nCoinsReward = pMission->nReward;
            // 增加任务获取的金币数量
            UserDataManager::sharedUserDataManager()->addGoldenCoins(pMission->nReward);
        }
    }
    else
    {
        // 任务失败
        /*
        CCLabelBMFont* pLabel = CCLabelBMFont::create("Mission Failed", "texture/fonts/font.fnt");
        pLabel->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.5f));
        pLabel->setOpacity(0);
        this->addChild(pLabel);
        pLabel->runAction(CCSequence::create(CCSpawn::create(CCFadeIn::create(0.5f), CCEaseBounceOut::create(CCScaleTo::create(1.0f, 3.0f)), NULL), CCFadeOut::create(1.0f), CCRemoveSelf::create(), NULL));
        */
    }
    // 计算得到当前局可获取的总金币数量
    m_nCoinsAdd = m_nCoinsReward + m_lCoinsWithScores;
    // 显示任务信息
    // m_pLabelTTFDetail->setString(UserDataManager::sharedUserDataManager()->getAnalysisDataRef()->dumpDebugInfo().c_str());
    
    // 初始化金币步进的值
    m_llScoreStep = 0;
    
    // 设置任务状态的大小很小
    m_pSpriteMissionStatus->setScale(0.01f);
    m_pSpriteMissionStatus->setVisible(false);
    
    // 开始清算当前分数和金币数量
    startScoreAndCoinUpdateAnimation();
    
    // 显示下用户金币数量
    updateUserinfoCoins(m_llUserCoins);
}

void BalloonResultDialog::onExit()
{
    unschedule(schedule_selector(BalloonResultDialog::timerCallbackUpdateScoreAndCoin));
    
	DialogLayer::onExit();
}

SEL_CallFuncN BalloonResultDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonResultDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuReturnMain", BalloonResultDialog::onPressMenuReturnMain);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuShare", BalloonResultDialog::onPressMenuShare);
    
	return NULL;
}

bool BalloonResultDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCurrentScore", CCLabelBMFont*, this->m_pLabelBMFontCurrentScore);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontHighestScore", CCLabelBMFont*, this->m_pLabelBMFontHighestScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuShare", CCMenu*, this->m_pMenuShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemPlayAgain", CCMenuItem*, this->m_pMenuItemPlayAgain);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemReturn", CCMenuItem*, this->m_pMenuItemReturn);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemShare", CCMenuItem*, this->m_pMenuItemShare);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemGotoShop", CCMenuItem*, this->m_pMenuItemGotoShop);
	// CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuResult", CCMenu*, this->m_pMenuResult);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuTop", CCMenu*, this->m_pMenuTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteNewFlag", CCSprite*, this->m_pSpriteNewFlag);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteResultBoard", CCSprite*, this->m_pSpriteResultBoard);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteStar", CCSprite*, this->m_pSpriteStar);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteSeperate", CCSprite*, this->m_pSpriteSeperate);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoinTop", CCSprite*, this->m_pSpriteCoinTop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoinReward", CCSprite*, this->m_pSpriteCoinReward);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteMissionStatus", CCSprite*, this->m_pSpriteMissionStatus);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoinQty", CCLabelBMFont*, this->m_pLabelBMFontCoinQty);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontMissionReward", CCLabelBMFont*, this->m_pLabelBMFontMissionReward);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelTTFMissionDesc", CCLabelTTF*, this->m_pLabelTTFMissionDesc);

	return true;
}

SEL_CCControlHandler BalloonResultDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonResultDialog::setScore(long long llScore)
{
    m_llTotalScore = llScore;
    m_pLabelBMFontCurrentScore->setCString(CCString::createWithFormat("%lld", llScore)->getCString());
}

void BalloonResultDialog::setHighScore(long long llScore)
{
    m_llHighestScore = llScore;
    // m_pLabelBMFontHighestScore->setCString(CCString::createWithFormat("%lld", llScore)->getCString());
}

void BalloonResultDialog::setNewFlagVisible(bool bShow)
{
    m_pSpriteNewFlag->setVisible(bShow);
}

std::string BalloonResultDialog::getSharedPictureFilePath()
{
    // 隐藏按钮
    // m_pMenuResult->setVisible(false);
    m_pMenuShare->setVisible(false);
    m_pMenuTop->setVisible(false);
    
    CCSize size = m_pSpriteResultBoard->getContentSize();
    
    // 保存并设置需要裁剪的节点的图形的位置，和画板位置对齐
    CCPoint pos = m_pSpriteResultBoard->getPosition();
    CCPoint anchorPt = m_pSpriteResultBoard->getAnchorPoint();
    m_pSpriteResultBoard->setPosition(CCPointZero);
    m_pSpriteResultBoard->setAnchorPoint(CCPointZero);
    
    CCRenderTexture* pTexture = CCRenderTexture::create(size.width, size.height, kCCTexture2DPixelFormat_RGBA8888);
    pTexture->clear(255, 255, 255, 255);
    
    pTexture->begin();
    m_pSpriteResultBoard->visit();
    pTexture->end();
    
    // 还原位置
    m_pSpriteResultBoard->setPosition(pos);
    m_pSpriteResultBoard->setAnchorPoint(anchorPt);
    
    // m_pMenuResult->setVisible(true);
    m_pMenuShare->setVisible(true);
    m_pMenuTop->setVisible(true);
    
    // 保存截图并返回图片路径
    if (pTexture->saveToFile(SNS_SHARE_IMAGE_FILE_NAME, kCCImageFormatPNG))
        return CCFileUtils::sharedFileUtils()->getWritablePath() + SNS_SHARE_IMAGE_FILE_NAME;
    
    return "";
}

void BalloonResultDialog::initRotateStar()
{
    m_pSpriteStar->stopAllActions();
    float fDuration = 0.5f;
    m_pSpriteStar->setScale(0.1f);
    m_pSpriteStar->setOpacity(0);
    
    CCAction* pAction1 = CCRepeatForever::create(CCSequence::create(CCSpawn::createWithTwoActions(CCFadeIn::create(fDuration), CCScaleTo::create(fDuration, 2.0f)), CCSpawn::createWithTwoActions(CCFadeOut::create(fDuration), CCScaleTo::create(fDuration, 0.1f)), CCCallFuncN::create(this, callfuncN_selector(BalloonResultDialog::changeStarPosition)), CCDelayTime::create(fDuration), NULL));
    
    CCAction* pAction2 = CCRepeatForever::create(CCRotateBy::create(fDuration*2, 360.0f));
    m_pSpriteStar->runAction(pAction1);
    m_pSpriteStar->runAction(pAction2);
}

void BalloonResultDialog::changeStarPosition(cocos2d::CCNode *pNode)
{
    if (pNode)
    {
        // 随机20%到80%的X轴
        float width = pNode->getParent()->getContentSize().width;
        
        pNode->setPositionX(((rand()%6 + 2) / 10.0f) * width);
    }
}

void BalloonResultDialog::updateGameCoins(long long llCoins)
{
    std::stringstream stream;
    stream << llCoins;
    m_pLabelBMFontCoins->setString(stream.str().c_str());
    
    // 更新位置
    // m_pSpriteCoin->setPosition(ccp(m_pLabelBMFontCoins->getPositionX() - m_pLabelBMFontCoins->getContentSize().width*m_pLabelBMFontCoins->getScaleX(), m_pSpriteCoin->getPositionY()));
}

void BalloonResultDialog::updateUserinfoCoins(long long llCoins)
{
    std::stringstream stream;
    stream << llCoins;
    m_pLabelBMFontCoinQty->setString(stream.str().c_str());
    m_pLabelBMFontCoinQty->setScale(2.0f);
    
    if (m_pSpriteResultBoard->boundingBox().size.width*0.25f < m_pLabelBMFontCoinQty->boundingBox().size.width)
        m_pLabelBMFontCoinQty->setScale(m_pSpriteResultBoard->getContentSize().width*0.25f/m_pLabelBMFontCoinQty->boundingBox().size.width);
    else
        m_pLabelBMFontCoinQty->setScale(2.0f);
}

void BalloonResultDialog::updateRewardCoins(int nCoins)
{
    std::stringstream stream;
    stream << nCoins;
    m_pLabelBMFontMissionReward->setString(stream.str().c_str());
}

void BalloonResultDialog::onPressMenuShare(CCObject* pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCDictionary* pDictData = CCDictionary::create();
    
    // 截取得分图片
    std::string strPath = getSharedPictureFilePath();
    
    // pDictData->setObject(ccs("一起来【气球大作战】吧～伸出你的指头，释放你的压力"), "shareText");
    const char* pszKey = "high_score_shares";
    if (m_llTotalScore <= 1)
    {
        // 采用单数模式的字符串
        pszKey = "high_score_share";
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#endif
    
    // 尾部挂上应用程序的链接地址
    CCString* pStrShareText = CCString::createWithFormat("%s %s", CCString::createWithFormat(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("share_section", pszKey), m_llTotalScore)->getCString(), "http://itunes.apple.com/app/id882836376");
    
    pDictData->setObject(pStrShareText, "shareText");
    pDictData->setObject(ccs(strPath.c_str()), "shareImage");
    UMSocial2DX::shareSNS(pDictData);
    
}

void BalloonResultDialog::onPressMenuReturnMain(CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCDirector::sharedDirector()->popScene();
}

void BalloonResultDialog::initMenuTop()
{
    // 先设置按下效果，这里会替换对应实体的对象，所以对应的动作要放到后面
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
    
    // 注册点击菜单
    pushMenu(m_pMenuTop);
}

void BalloonResultDialog::startScoreAndCoinUpdateAnimation()
{
    schedule(schedule_selector(BalloonResultDialog::timerCallbackUpdateScoreAndCoin), 0.05f);
    
    // 这里顺便把用户的当前游戏的详细记录罗列下
    do
    {
        CCDictionary* pDictConfiguration = CCDictionary::createWithContentsOfFile("configuration/scoreDetail.plist");
        CC_BREAK_IF(!pDictConfiguration);
        const BalloonAnalysis* pAnalysisData = UserDataManager::sharedUserDataManager()->getAnalysisDataRef();
        // 得到颜色配置的选项，颜色的文字配置key
        CCArray* pArrayConfigurationColor = dynamic_cast<CCArray*>(pDictConfiguration->objectForKey("color"));
        CC_BREAK_IF(!pArrayConfigurationColor);
        const tagBalloonColorAnalysisData& colorData = pAnalysisData->getAnalysisData().colorData;
        unsigned int size = sizeof(tagBalloonColorAnalysisData)/sizeof(long long);
        long long* pData = (long long*)&colorData;
        for (unsigned int idx = 0; idx < size; ++idx)
        {
            CCScale9Sprite* pDataBackground = CCScale9Sprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("balloon_detail_bg.png"));
            CCString* pStrOptionsKey = dynamic_cast<CCString*>(pArrayConfigurationColor->objectAtIndex(idx));
            CC_BREAK_IF(!pStrOptionsKey);
            const char* pszOptionsName = DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("scoreDetailOptions_section", pStrOptionsKey->getCString());
            CCLabelTTF* pScore = CCLabelTTF::create(CCString::createWithFormat("%s:%lld", pszOptionsName, *pData)->getCString(), "", pDataBackground->getContentSize().height*0.5f);
            // 设置背景的长和宽，根据文字的长度调节
            pDataBackground->setPreferredSize(CCSizeMake(pScore->getContentSize().width+pDataBackground->getContentSize().width*0.2f, pDataBackground->getContentSize().height));
            pDataBackground->setOpacity(128);
            // 设置文字居中对齐
            pScore->setPosition(ccpMult(ccpFromSize(pDataBackground->getContentSize()), 0.5f));
            pDataBackground->addChild(pScore);
            // 设置分数背景面板的位置
            float width = m_pSpriteResultBoard->getContentSize().width;
            float x, y = 0;
            x = width/4*(idx%3 + 1);
            y = m_pSpriteCoin->getPositionY() - m_pSpriteCoin->getContentSize().height - floor(idx/3.0f)*pDataBackground->getContentSize().height*1.05f;
            pDataBackground->setPosition(ccp(x, y));
            pDataBackground->setScale(0.01f);
            pDataBackground->setVisible(false);
            m_pSpriteResultBoard->addChild(pDataBackground);
            // 所有数据一起弹出来
            pDataBackground->runAction(CCSequence::create(CCDelayTime::create(0.5f + 0.2f*idx), CCShow::create(), CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.0f)), NULL));
            // 颜色数据往后移动一个
            pData++;
        }
    } while(0);
    
}

void BalloonResultDialog::timerCallbackUpdateScoreAndCoin(float dt)
{
    // 这里默认设置更新的次数
    int nStep = m_llTotalScore / 50;
    // 小于50分的花2.5秒时间搞定
    if (m_llTotalScore < 50)
        nStep = 1;
    m_llScoreStep += nStep;
    if (m_llScoreStep <= m_llTotalScore)
    {
        m_pLabelBMFontCurrentScore->setString(CCString::createWithFormat("%lld", m_llScoreStep)->getCString());
        // 更新金币数量
        updateGameCoins(m_llScoreStep/SCORE_COINS_RATE);
    }
    else
    {
        m_pLabelBMFontCurrentScore->setString(CCString::createWithFormat("%lld", m_llTotalScore)->getCString());
        unschedule(schedule_selector(BalloonResultDialog::timerCallbackUpdateScoreAndCoin));
        // 这里计算当次金币数量
        startCoinUpdateAnimation();
    }
}

void BalloonResultDialog::actionCallbackAddCoins(CCNode* pNode, void* pData)
{
    long * pAddCoins = (long*)pData;
    // 更新用户金币
    m_llUserCoins += *pAddCoins;
    updateUserinfoCoins(m_llUserCoins);
    
    // 这里销毁掉
    delete pAddCoins;
    
    // 赚钱音效
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectSpendCoin();
    
    // TODO: 增加Node上的效果，比如搞个粒子之类的
}

void BalloonResultDialog::startCoinUpdateAnimation()
{
    // 把金币添加到用户金币上
    if (m_lCoinsWithScoresCnt > 0)
    {
        CCSprite* pSpriteCoin = CCSprite::createWithSpriteFrame(m_pSpriteCoin->displayFrame());
        pSpriteCoin->setPosition(m_pSpriteCoin->getPosition());
        pSpriteCoin->setScale(m_pSpriteCoin->getScale());
        m_pSpriteResultBoard->addChild(pSpriteCoin);
        // 填充一个数据结构
        long * pParam = new long;
        *pParam = m_lCoinsWithScores;
        
        pSpriteCoin->runAction(CCSequence::create(CCJumpTo::create(0.5f, m_pSpriteCoinTop->getPosition(), m_pSpriteCoinTop->getPositionY() - pSpriteCoin->getPositionY() + 100, 1), CCRemoveSelf::create(), CCCallFuncND::create(this, callfuncND_selector(BalloonResultDialog::actionCallbackAddCoins), pParam), NULL));
    }
    
    // 设置任务是否完成
    const char* pszMissionStatusFrameName = NULL;
    if (m_bMissionCompleted)
    {
        pszMissionStatusFrameName = "result_mission_ok.png";
        // 金币跳跃到总分上
        CCSprite* pSpriteCoin = CCSprite::createWithSpriteFrame(m_pSpriteCoinReward->displayFrame());
        pSpriteCoin->setScale(m_pSpriteCoinReward->getScale());
        pSpriteCoin->setPosition(m_pSpriteCoin->getPosition());
        m_pSpriteResultBoard->addChild(pSpriteCoin);
        // 填充一个数据结构
        long * pParam = new long;
        *pParam = m_nCoinsReward;
        
        pSpriteCoin->runAction(CCSequence::create(CCJumpTo::create(1.5f, m_pSpriteCoinTop->getPosition(), m_pSpriteCoinTop->getPositionY() - pSpriteCoin->getPositionY() + 100, 1), CCRemoveSelf::create(), CCCallFuncND::create(this, callfuncND_selector(BalloonResultDialog::actionCallbackAddCoins), pParam), NULL));
        
        // 添加一个烟花效果
        addFireworks();
    }
    else
    {
        pszMissionStatusFrameName = "result_mission_failed.png";
        m_pLabelBMFontMissionReward->setString("0");
    }
    
    // 任务完成状态动画效果
    m_pSpriteMissionStatus->setVisible(true);
    m_pSpriteMissionStatus->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszMissionStatusFrameName));
    m_pSpriteMissionStatus->runAction(CCEaseBounceOut::create(CCScaleTo::create(1.0f, 1.0f)));
}

void BalloonResultDialog::addFireworks(int nFireworks /* = 0 */)
{
    if (nFireworks <= 0)
        nFireworks = 3;
    
    for (int idx = 0; idx < nFireworks; idx++)
    {
        // 从底部升起，到点爆炸
        CCParticleSystemQuad* pFirewoksTakeoff = CCParticleSystemQuad::create("particles/fireworksTakeoff.plist");
        pFirewoksTakeoff->setAutoRemoveOnFinish(true);
        float posX = rand()%(int(getContentSize().width*0.5f)) + getContentSize().width*0.2f;
        pFirewoksTakeoff->setPosition(ccp(posX, 0));
        pFirewoksTakeoff->runAction(CCSequence::create(CCDelayTime::create((rand()%30)*0.1f), CCCallFunc::create(this, callfunc_selector(BalloonResultDialog::actionCallbackPlayFireworksTakeOffSound)), CCEaseExponentialOut::create(CCMoveTo::create(2.0f, ccp(pFirewoksTakeoff->getPositionX(), getContentSize().height*((rand()%30+40))*0.01f))), CCCallFuncN::create(this, callfuncN_selector(BalloonResultDialog::actionCallbackFireworksExplosive)), CCRemoveSelf::create(), NULL));
        addChild(pFirewoksTakeoff);
    }
}

void BalloonResultDialog::actionCallbackFireworksExplosive(cocos2d::CCNode *pNode)
{
    CCParticleSystemQuad* pFireworksExplosive = CCParticleSystemQuad::create("particles/fireworksExplosive.plist");
    pFireworksExplosive->setPosition(pNode->getPosition());
    pFireworksExplosive->setAutoRemoveOnFinish(true);
    addChild(pFireworksExplosive);
    // 爆炸音效
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectFireworksExplisive();
}

void BalloonResultDialog::actionCallbackPlayFireworksTakeOffSound()
{
    // 烟花发射的音效
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectFireworksTakeOff();
}
#include "BalloonResultDialog.h"
#include "bailinUtil.h"
#include "UserData.h"
#include "BalloonMission.h"
#include <string>

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#define SNS_SHARE_IMAGE_FILE_NAME "sns_share.png"

BalloonResultDialog::~BalloonResultDialog()
{
    CC_SAFE_RELEASE_NULL(m_pSpriteResultBoard);
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
		
        // 注册点击菜单
        pushMenu(m_pMenuShare);
        pushMenu(m_pMenuResult);
        
        // 设置按钮按下后变大的效果
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuShare);
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuResult);
        
        m_pSpriteResultBoard->retain();
        m_pSpriteResultBoard->removeFromParent();
        
        // 初始化位置
        m_posOri = m_pSpriteResultBoard->getPosition();
        CCPoint posUp = ccp(m_pSpriteResultBoard->getPositionX(), getContentSize().height*2);
        
        // 进场动画
        m_pSpriteResultBoard->setPosition(posUp);
        
        addBoardChildWithAction(m_pSpriteResultBoard,
                                CCEaseBounceOut::create(CCMoveTo::create(0.5f, m_posOri)),
                                CCEaseExponentialIn::create(CCMoveTo::create(0.2f, posUp)));
        SetVisibleBoard(NULL);
        
        // 更新分享按钮数据
        m_posShareItemOri = m_pMenuItemShare->getPosition();
        m_fShareItemAngleOri = m_pMenuItemShare->getRotation();
        // 默认隐藏新记录标志
        setNewFlagVisible(false);
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonResultDialog::onEnter()
{
	DialogLayer::onEnter();
    m_pMenuItemShare->stopAllActions();
    m_pMenuItemShare->setPosition(m_posShareItemOri);
    m_pMenuItemShare->setRotation(m_fShareItemAngleOri);
    
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.5f, 30.0f), CCRotateBy::create(1.5f, -30.0f), NULL)));
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(2.7f, ccp(0, m_pMenuItemShare->getContentSize().height*0.3f)), CCMoveBy::create(2.5f, ccp(0, -m_pMenuItemShare->getContentSize().height*0.3f)), NULL)));
    
    initRotateStar();
    
    CCAssert(m_pAnalysisData, "Analysis Data is NULL!!!");
    // 计算是否完成任务
    const Mission* pMission = BalloonMission::sharedBalloonMission()->getRandomMission();
    // 这里生成一个任务结构体，用于比对是否完成
    MissionData md;
    // 当前分数赋值
    md.score = m_llTotalScore;
    // 拷贝当前盘的数据
    md.analysisData = *m_pAnalysisData; // m_BalloonAnalysis.getAnalysisData();
    if (pMission && pMission->isMissionComplete(md))
    {
        // 完成任务，显示奖励
        CCLOG("Mission Complete!!");
        // 增加完成任务的奖励，目前应该只增加金币
        if (pMission->cbRewardType == kCCMissionRewardTypeGoldenCoins)
        {
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
            BalloonMission::sharedBalloonMission()->setNeedNewMission(true);
        }
    }
    else
    {
        // 任务失败
        CCLabelBMFont* pLabel = CCLabelBMFont::create("Mission Failed", "texture/fonts/font.fnt");
        pLabel->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.5f));
        pLabel->setOpacity(0);
        this->addChild(pLabel);
        pLabel->runAction(CCSequence::create(CCSpawn::create(CCFadeIn::create(0.5f), CCEaseBounceOut::create(CCScaleTo::create(1.0f, 3.0f)), NULL), CCFadeOut::create(1.0f), CCRemoveSelf::create(), NULL));
    }
}

void BalloonResultDialog::onExit()
{
	DialogLayer::onExit();
}

SEL_CallFuncN BalloonResultDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonResultDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonResultDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCurrentScore", CCLabelBMFont*, this->m_pLabelBMFontCurrentScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontHighestScore", CCLabelBMFont*, this->m_pLabelBMFontHighestScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuShare", CCMenu*, this->m_pMenuShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemPlayAgain", CCMenuItem*, this->m_pMenuItemPlayAgain);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemReturn", CCMenuItem*, this->m_pMenuItemReturn);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemShare", CCMenuItem*, this->m_pMenuItemShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuItemGotoShop", CCMenuItem*, this->m_pMenuItemGotoShop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuResult", CCMenu*, this->m_pMenuResult);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteNewFlag", CCSprite*, this->m_pSpriteNewFlag);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteResultBoard", CCSprite*, this->m_pSpriteResultBoard);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteStar", CCSprite*, this->m_pSpriteStar);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteCoin", CCSprite*, this->m_pSpriteCoin);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pLabelBMFontCoins", CCLabelBMFont*, this->m_pLabelBMFontCoins);

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
    
    updateCoins(llScore/SCORE_COINS_RATE);
}

void BalloonResultDialog::setHighScore(long long llScore)
{
    m_llHighestScore = llScore;
    m_pLabelBMFontHighestScore->setCString(CCString::createWithFormat("%lld", llScore)->getCString());
}

void BalloonResultDialog::setNewFlagVisible(bool bShow)
{
    m_pSpriteNewFlag->setVisible(bShow);
}

std::string BalloonResultDialog::getSharedPictureFilePath()
{
    // 隐藏按钮
    m_pMenuResult->setVisible(false);
    m_pMenuShare->setVisible(false);
    
    CCSize size = m_pSpriteResultBoard->getContentSize();
    
    // 保存并设置需要裁剪的节点的图形的位置，和画板位置对齐
    CCPoint pos = m_pSpriteResultBoard->getPosition();
    CCPoint anchorPt = m_pSpriteResultBoard->getAnchorPoint();
    m_pSpriteResultBoard->setPosition(CCPointZero);
    m_pSpriteResultBoard->setAnchorPoint(CCPointZero);
    
    CCRenderTexture* pTexture = CCRenderTexture::create(size.width, size.height, kCCTexture2DPixelFormat_RGBA8888);
    pTexture->clear(0, 0, 0, 0);
    
    pTexture->begin();
    m_pSpriteResultBoard->visit();
    pTexture->end();
    
    // 还原位置
    m_pSpriteResultBoard->setPosition(pos);
    m_pSpriteResultBoard->setAnchorPoint(anchorPt);
    
    m_pMenuResult->setVisible(true);
    m_pMenuShare->setVisible(true);
    
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

void BalloonResultDialog::updateCoins(long long llCoins)
{
    std::stringstream stream;
    stream << llCoins;
    m_pLabelBMFontCoins->setString(stream.str().c_str());
    
    // 更新位置
    m_pSpriteCoin->setPosition(ccp(m_pLabelBMFontCoins->getPositionX() - m_pLabelBMFontCoins->getContentSize().width*m_pLabelBMFontCoins->getScaleX(), m_pSpriteCoin->getPositionY()));
}

void BalloonResultDialog::setAnalysisData(const BalloonAnalysisData *pData)
{
    m_pAnalysisData = pData;
}
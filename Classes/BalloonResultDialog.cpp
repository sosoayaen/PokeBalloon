#include "BalloonResultDialog.h"
#include "bailinUtil.h"

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
	// TODO:
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
                                CCEaseExponentialIn::create(CCMoveTo::create(0.5f, posUp)));
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
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    
    /*/ 分享界面不停摇动
    CCMoveBy* pMoveUp = CCMoveBy::create(2.0f, ccp(0, m_pMenuItemReturn->getContentSize().height));
    
    CCMoveBy* pMoveRight = CCMoveBy::create(2.0f, ccp(m_pMenuItemReturn->getContentSize().height*0.3f, 0));
    
    m_pMenuShare->runAction(CCSpawn::create(
        CCRepeatForever::create(CCSequence::create(
            pMoveUp, pMoveUp->reverse(),
            NULL)),
        CCRepeatForever::create(CCSequence::create(
            pMoveRight, pMoveRight->reverse(),
            NULL)),
            NULL));
    //*/
    m_pMenuItemShare->stopAllActions();
    m_pMenuItemShare->setPosition(m_posShareItemOri);
    m_pMenuItemShare->setRotation(m_fShareItemAngleOri);
    
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.5f, 30.0f), CCRotateBy::create(1.5f, -30.0f), NULL)));
    m_pMenuItemShare->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(2.7f, ccp(0, m_pMenuItemShare->getContentSize().height*0.3f)), CCMoveBy::create(2.5f, ccp(0, -m_pMenuItemShare->getContentSize().height*0.3f)), NULL)));
}

void BalloonResultDialog::onExit()
{
	DialogLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
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
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontCurrentScore", CCLabelBMFont*, this->m_pLabelBMFontCurrentScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontHighestScore", CCLabelBMFont*, this->m_pLabelBMFontHighestScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuShare", CCMenu*, this->m_pMenuShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemPlayAgain", CCMenuItem*, this->m_pMenuItemPlayAgain);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemReturn", CCMenuItem*, this->m_pMenuItemReturn);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemShare", CCMenuItem*, this->m_pMenuItemShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemGotoShop", CCMenuItem*, this->m_pMenuItemGotoShop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuResult", CCMenu*, this->m_pMenuResult);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteNewFlag", CCSprite*, this->m_pSpriteNewFlag);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteResultBoard", CCSprite*, this->m_pSpriteResultBoard);

	return true;
}

SEL_CCControlHandler BalloonResultDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonResultDialog::setScore(long lScore)
{
    m_pLabelBMFontCurrentScore->setCString(CCString::createWithFormat("%ld", lScore)->getCString());
}

void BalloonResultDialog::setHighScore(long lScore)
{
    m_pLabelBMFontHighestScore->setCString(CCString::createWithFormat("%ld", lScore)->getCString());
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
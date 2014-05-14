#include "BalloonOptionsDialog.h"
#include "bailinUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

BalloonOptionsDialog::~BalloonOptionsDialog()
{
    CC_SAFE_RELEASE_NULL(m_pSpriteResultBoard);
}

bool BalloonOptionsDialog::init()
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
		std::string strCCBFileName = "BalloonOptionsDialog.ccbi";

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
        
        // 默认隐藏新记录标志
        setNewFlagVisible(false);
        
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonOptionsDialog::onEnter()
{
	DialogLayer::onEnter();
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
}

void BalloonOptionsDialog::onExit()
{
	DialogLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
}

SEL_CallFuncN BalloonOptionsDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonOptionsDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonOptionsDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontCurrentScore", CCLabelBMFont*, this->m_pLabelBMFontCurrentScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelBMFontHighestScore", CCLabelBMFont*, this->m_pLabelBMFontHighestScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuShare", CCMenu*, this->m_pMenuShare);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemPlayAgain", CCMenuItem*, this->m_pMenuItemPlayAgain);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemReturn", CCMenuItem*, this->m_pMenuItemReturn);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemGotoShop", CCMenuItem*, this->m_pMenuItemGotoShop);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuResult", CCMenu*, this->m_pMenuResult);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteNewFlag", CCSprite*, this->m_pSpriteNewFlag);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteResultBoard", CCSprite*, this->m_pSpriteResultBoard);

	return true;
}

SEL_CCControlHandler BalloonOptionsDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonOptionsDialog::setScore(long lScore)
{
    m_pLabelBMFontCurrentScore->setCString(CCString::createWithFormat("%ld", lScore)->getCString());
}

void BalloonOptionsDialog::setHighScore(long lScore)
{
    m_pLabelBMFontHighestScore->setCString(CCString::createWithFormat("%ld", lScore)->getCString());
}

void BalloonOptionsDialog::setNewFlagVisible(bool bShow)
{
    m_pSpriteNewFlag->setVisible(bShow);
}

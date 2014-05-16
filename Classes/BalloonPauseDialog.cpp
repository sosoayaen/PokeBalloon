#include "BalloonPauseDialog.h"
#include "bailinUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

BalloonPauseDialog::~BalloonPauseDialog()
{
}

bool BalloonPauseDialog::init()
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
		std::string strCCBFileName = "BalloonPauseDialog.ccbi";

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
		
        // 按钮按下后放大效果
        ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenu);
        
        pushMenu(m_pMenu);
        
        // 自动旋转
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonPauseDialog::onEnter()
{
	DialogLayer::onEnter();
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
}

void BalloonPauseDialog::onExit()
{
	DialogLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
}

SEL_CallFuncN BalloonPauseDialog::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonPauseDialog::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonPauseDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemAgain", CCMenuItem*, this->m_pMenuItemAgain);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemResume", CCMenuItem*, this->m_pMenuItemResume);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemReturn", CCMenuItem*, this->m_pMenuItemReturn);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenu", CCMenu*, this->m_pMenu);

	return true;
}

SEL_CCControlHandler BalloonPauseDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}










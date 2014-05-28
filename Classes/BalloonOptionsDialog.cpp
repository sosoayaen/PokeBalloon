#include "BalloonOptionsDialog.h"
#include "bailinUtil.h"
#include "SimpleAudioEngine.h"
#include "BalloonSoundManager.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;
using namespace CocosDenshion;

BalloonOptionsDialog::~BalloonOptionsDialog()
{
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
        
        initMenuItem();
        
        SetVisibleBoard(m_pSpriteBoard);
		
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
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuMusic", BalloonOptionsDialog::onPressMenuMusic);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressMenuEffect", BalloonOptionsDialog::onPressMenuEffect);

	return NULL;
}

bool BalloonOptionsDialog::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemMusic", CCMenuItemImage*, this->m_pMenuItemMusic);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuItemEffect", CCMenuItemImage*, this->m_pMenuItemEffect);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuMusic", CCMenu*, this->m_pMenuMusic);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuEffect", CCMenu*, this->m_pMenuEffect);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBoard", CCSprite*, this->m_pSpriteBoard);

	return true;
}

SEL_CCControlHandler BalloonOptionsDialog::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonOptionsDialog::saveOptionsData()
{
    CCUserDefault::sharedUserDefault()->setIntegerForKey(OPT_MUSIC_OFF, DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF));
    CCUserDefault::sharedUserDefault()->setIntegerForKey(OPT_SOUND_EFFECT_OFF, DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF));
}

void BalloonOptionsDialog::onPressMenuMusic(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCMenuItemImage* pMenuItem = dynamic_cast<CCMenuItemImage*>(pSender);
    if (pMenuItem)
    {
        bool bEnable = pMenuItem->getOpacity() == 0;
        setCheckBoxEnable(pMenuItem, bEnable);
        
        DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(OPT_MUSIC_OFF, bEnable ? 0 : 1);
        
        if (bEnable)
        {
            SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
        }
        else
        {
            SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
        }
    }
}

void BalloonOptionsDialog::onPressMenuEffect(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCMenuItemImage* pMenuItem = dynamic_cast<CCMenuItemImage*>(pSender);
    if (pMenuItem)
    {
        bool bEnable = pMenuItem->getOpacity() == 0;
        setCheckBoxEnable(pMenuItem, bEnable);
        
        DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(OPT_SOUND_EFFECT_OFF, bEnable ? 0 : 1);
    }
}

void BalloonOptionsDialog::initMenuItem()
{
    // ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuReturn);
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuMusic);
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuEffect);
    
    // pushMenu(m_pMenuReturn);
    pushMenu(m_pMenuMusic);
    pushMenu(m_pMenuEffect);
    
    m_pMenuItemEffect->setCascadeOpacityEnabled(true);
    m_pMenuItemMusic->setCascadeOpacityEnabled(true);
    
    // 根据全局数据设置按钮状态
    long lMusicOFF = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF);
    if (lMusicOFF != 0)
    {
        if (SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
        {
            // 停止背景音乐
            SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
        }
        
        setCheckBoxEnable(m_pMenuItemMusic, false);
    }
    else
    {
        if (!SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
        {
            // 停止背景音乐
            SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
        }
        
        setCheckBoxEnable(m_pMenuItemMusic, true);
    }
   
    long lSoundEffectOFF = DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF);
    
    setCheckBoxEnable(m_pMenuItemEffect, lSoundEffectOFF == 0);
}

void BalloonOptionsDialog::setCheckBoxEnable(CCMenuItemImage* pMenuItem, bool bEnable)
{
    if (pMenuItem)
    {
        if (bEnable)
            pMenuItem->setOpacity(255);
        else
            pMenuItem->setOpacity(0);
    }
}


bool BalloonOptionsDialog::endDialog(cocos2d::CCActionInterval* pAction /* = NULL */, float fDuration /* = 1.0f */)
{
    saveOptionsData();
    
    return DialogLayer::endDialog(pAction, fDuration);
}
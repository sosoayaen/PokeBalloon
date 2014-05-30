#include "BalloonFirstPage.h"
#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "UMSocial2DX.h"
#include "bailinUtil.h"

#include "BalloonOptionsDialog.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameKitHelper2dx.h"
#endif

#define OPT_MUSIC_OFF "opt_MusicOFF"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_BAILIN_UTIL;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

BalloonFirstPage::~BalloonFirstPage()
{
}

CCScene* BalloonFirstPage::scene()
{
    CCScene* pScene = CCScene::create();
    
    BalloonFirstPage* pLayer = BalloonFirstPage::create();
    
    pScene->addChild(pLayer);
    
    return pScene;
}

bool BalloonFirstPage::init()
{
	// TODO:
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!CCLayer::init());
		// 加载ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// 对应ccbi文件
		std::string strCCBFileName = "BalloonFirstPage.ccbi";

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
        
        // 初始化按钮
        initMenu();
        
        // 背景拉满屏幕
        if (m_pSpriteBackground)
        {
            m_pSpriteBackground->setScaleX(this->getContentSize().width/m_pSpriteBackground->getContentSize().width);
            m_pSpriteBackground->setScaleY(this->getContentSize().height/m_pSpriteBackground->getContentSize().height);
        }
		
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonFirstPage::onEnter()
{
	CCLayer::onEnter();
	// TODO: 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    
    if (!BalloonSoundManager::sharedBalloonSoundManager()->isBackgroundMusicPlaying())
    {
        BalloonSoundManager::sharedBalloonSoundManager()->playBackgroundMusic(SOUND_BACKGROUND_AFTER_SCHOOL);
    }
    
    if (DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF))
    {
        SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    }
    
    m_pMenuMain->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.8f, ccpMult(ccpFromSize(getContentSize()), 0.5f))));
}

void BalloonFirstPage::onExit()
{
	CCLayer::onExit();
	// TODO: 退出场景，取消CCNotificationCenter可以放在这里做，但是对应在onEnter的时候要重新注册
    m_pMenuMain->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*1.5f));
}

SEL_CallFuncN BalloonFirstPage::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonFirstPage::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonFirstPage::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pMenuMain", CCMenu*, this->m_pMenuMain);

	return true;
}

SEL_CCControlHandler BalloonFirstPage::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

void BalloonFirstPage::initMenu()
{
    // 加载按钮纹理
    CCSpriteFrameCache* pSpriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    pSpriteFrameCache->addSpriteFramesWithFile("texture/menuItems/menuItems.plist");
    
    // CCMenuItemImage* pMenuItemStart = CCMenuItemImage::create("texture/menuItems/menu_item_start.png", "texture/menuItems/menu_item_start.png", this, menu_selector(BalloonFirstPage::onPressMenuStartGame));
    
    CCMenuItemImage* pMenuItemStart = CCMenuItemImage::create();
    pMenuItemStart->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_start.png"));
    pMenuItemStart->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuStartGame));
    
    // CCMenuItemImage* pMenuItemShop = CCMenuItemImage::create("texture/menuItems/menu_item_shop.png", "texture/menuItems/menu_item_shop.png", this, menu_selector(BalloonFirstPage::onPressMenuShop));
    
    CCMenuItemImage* pMenuItemShop = CCMenuItemImage::create();
    pMenuItemShop->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_shop.png"));
    pMenuItemShop->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuShop));
    
    // CCMenuItemImage* pMenuItemOptions = CCMenuItemImage::create("texture/menuItems/menu_item_options.png", "texture/menuItems/menu_item_options.png", this, menu_selector(BalloonFirstPage::onPressMenuOptions));
    
    CCMenuItemImage* pMenuItemOptions = CCMenuItemImage::create();
    pMenuItemOptions->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_options.png"));
    pMenuItemOptions->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuOptions));
    
    pMenuItemStart->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemShop->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemOptions->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    
    m_pMenuMain->addChild(pMenuItemStart);
    m_pMenuMain->addChild(pMenuItemShop);
    m_pMenuMain->addChild(pMenuItemOptions);
    
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuMain);
    
    m_pMenuMain->alignItemsVerticallyWithPadding(pMenuItemOptions->getContentSize().height);
    
    m_pMenuMain->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*1.5f));
}

void BalloonFirstPage::onPressMenuStartGame(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCScene* pScene = BalloonScene::scene();
    CCDirector::sharedDirector()->pushScene(pScene);
}

void BalloonFirstPage::onPressMenuShop(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    GameKitHelper2dx::showLeaderboard();
#endif
}

void BalloonFirstPage::onPressMenuOptions(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    addChild(BalloonOptionsDialog::create());
}
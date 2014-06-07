#include "BalloonFirstPage.h"
#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "BalloonHandBookScene.h"
#include "UMSocial2DX.h"
#include "bailinUtil.h"
#include "Balloon_macro.h"

#include "BalloonOptionsDialog.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameKitHelper2dx.h"
#endif

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
        
        // 顶部有Fireworks的粒子效果
        if (!m_pFireworks)
        {
            m_pFireworks = CCParticleSystemQuad::create("particles/fireworksMainBoard.plist");
            CCSize size = CCDirector::sharedDirector()->getWinSize();
            m_pFireworks->setPosition(ccp(size.width*0.25f, size.height * 1.05f));
            m_pMenuMain->getParent()->addChild(m_pFireworks);
        }
        
        // 顶部有Fireworks的粒子效果
        if (!m_pFireworks2)
        {
            m_pFireworks2 = CCParticleSystemQuad::create("particles/fireworksMainBoard.plist");
            CCSize size = CCDirector::sharedDirector()->getWinSize();
            m_pFireworks2->setPosition(ccp(size.width*0.75f, size.height * 1.05f));
            m_pMenuMain->getParent()->addChild(m_pFireworks2);
        }
        
        // 初始化按钮
        initMenu();
        
        // 背景拉满屏幕
        if (m_pSpriteBackground)
        {
            m_pSpriteBackground->setScaleX(this->getContentSize().width/m_pSpriteBackground->getContentSize().width);
            m_pSpriteBackground->setScaleY(this->getContentSize().height/m_pSpriteBackground->getContentSize().height);
        }
        
        // 标题动起来
        if (m_pSpriteTitle)
        {
            float fDuration = 5.0f;
            CCPoint pos = m_pSpriteTitle->getPosition();
            CCPoint pos2 = ccpAdd(pos, ccp(0, m_pSpriteTitle->getContentSize().height*0.3f));
            m_pSpriteTitle->runAction(CCRepeatForever::create(CCSequence::create(
                                 CCSpawn::createWithTwoActions(CCMoveTo::create(fDuration, pos2), CCScaleTo::create(fDuration, 1.05f)),
                                 CCSpawn::createWithTwoActions(CCMoveTo::create(fDuration, pos), CCScaleTo::create(fDuration, 1.0f)), NULL)));
            m_pSpriteTitle->setZOrder(1);
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
    
    // 移动到屏幕偏下的位置，给标题留位置
    m_pMenuMain->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.8f, ccp(getContentSize().width*0.5f, getContentSize().height*0.3f))));
    
    m_pFireworks->stopSystem();
    m_pFireworks->resetSystem();
    m_pFireworks2->stopSystem();
    m_pFireworks2->resetSystem();
    
    // 判断下当前的语言版本，替换主题界面
    if (CCApplication::sharedApplication()->getCurrentLanguage() == kLanguageChinese)
    {
        CCSprite* pSprite = CCSprite::create("texture/mainboard/main_title_zh.png");
        m_pSpriteTitle->setDisplayFrame(pSprite->displayFrame());
    }
}

void BalloonFirstPage::onExit()
{
	CCLayer::onExit();
    // 退出场景时，把菜单拎到屏幕外，进场时直接落下即可
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
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteTitle", CCSprite*, this->m_pSpriteTitle);

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
    
    CCMenuItemImage* pMenuItemStart = CCMenuItemImage::create();
    pMenuItemStart->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_start.png"));
    pMenuItemStart->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuStartGame));
    
    CCMenuItemImage* pMenuItemOptions = CCMenuItemImage::create();
    pMenuItemOptions->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_options.png"));
    pMenuItemOptions->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuOptions));
    
    CCMenuItemImage* pMenuItemHandbook = CCMenuItemImage::create();
    pMenuItemHandbook->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_handbook.png"));
    pMenuItemHandbook->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuHandbook));
    
    pMenuItemStart->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemOptions->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemHandbook->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    
    m_pMenuMain->addChild(pMenuItemStart);
    m_pMenuMain->addChild(pMenuItemHandbook);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCMenuItemImage* pMenuItemRankBoard = CCMenuItemImage::create();
    pMenuItemRankBoard->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_rank.png"));
    pMenuItemRankBoard->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuRankBoard));
    pMenuItemRankBoard->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    m_pMenuMain->addChild(pMenuItemRankBoard);
#endif
    
    m_pMenuMain->addChild(pMenuItemOptions);
    
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuMain);
    
    m_pMenuMain->alignItemsVerticallyWithPadding(pMenuItemOptions->getContentSize().height*0.5f);
    
    m_pMenuMain->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*1.5f));
    
    m_pMenuMain->setZOrder(100);
}

void BalloonFirstPage::onPressMenuStartGame(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    CCScene* pScene = BalloonScene::scene();
    CCDirector::sharedDirector()->pushScene(pScene);
}

void BalloonFirstPage::onPressMenuRankBoard(cocos2d::CCObject *pSender)
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

void BalloonFirstPage::onPressMenuHandbook(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    // 显示图鉴场景
    CCScene* pScene = BalloonHandBookScene::scene();
    CCDirector::sharedDirector()->pushScene(pScene);
}
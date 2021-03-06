#include "BalloonFirstPage.h"
#include "BalloonScene.h"
#include "BalloonSoundManager.h"
#include "BalloonHandBookScene.h"
#include "UMSocial2DX.h"
#include "bailinUtil.h"
#include "Balloon_macro.h"

#include "BalloonOptionsDialog.h"
#include "BalloonRankDialog.h"
#include "BalloonShopScene.h"
#include "BalloonUserInfoDialog.h"
#include "CCJSONConverter.h"
#include "BalloonShopDialog.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#   include "GameKitHelper2dx.h"
#endif

#ifdef ENABLE_UMENG_DATA
#   include "MobClickCpp.h"
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
		// CC_BREAK_IF(!CCLayer::init());
		CC_BREAK_IF(!AutoTextureManagerLayer::init());
        
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
        
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        // 顶部有Fireworks的粒子效果
        if (!m_pFireworks)
        {
            m_pFireworks = CCParticleSystemQuad::create("particles/fireworksMainBoard.plist");
            m_pFireworks->setPosition(ccp(size.width*0.25f, size.height * 1.05f));
            m_pMenuMain->getParent()->addChild(m_pFireworks);
        }
        
        // 顶部有Fireworks的粒子效果
        if (!m_pFireworks2)
        {
            m_pFireworks2 = CCParticleSystemQuad::create("particles/fireworksMainBoard.plist");
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
        
        // 放上版本号
        if (!m_pLabelTTFVersion)
        {
			CCString* pStringVersion = CCString::createWithFormat("v%s", DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataString("versionName"));
            m_pLabelTTFVersion = CCLabelTTF::create(pStringVersion->getCString(), "", size.width/25);
            m_pLabelTTFVersion->setAnchorPoint(ccp(1.0f, 0));
            m_pLabelTTFVersion->setPosition(ccp(size.width, 0));
            addChild(m_pLabelTTFVersion);
        }
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonFirstPage::onEnter()
{
	// CCLayer::onEnter();
    AutoTextureManagerLayer::onEnter();
	// 这里可以定义进入场景的初始化，比如控件的初始位置，初始状态等
    
    if (!BalloonSoundManager::sharedBalloonSoundManager()->isBackgroundMusicPlaying())
    {
        BalloonSoundManager::sharedBalloonSoundManager()->playBackgroundMusic(SOUND_BACKGROUND_AFTER_SCHOOL);
    }
    
    if (DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF))
    {
        SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    }
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    // 移动到屏幕偏下的位置，给标题留位置
    m_pMenuMain->runAction(CCEaseBounceOut::create(CCMoveTo::create(0.8f, ccp(size.width*0.5f, size.height*0.3f))));
    
    m_pFireworks->stopSystem();
    m_pFireworks->resetSystem();
    m_pFireworks2->stopSystem();
    m_pFireworks2->resetSystem();
    
    ccLanguageType language = CCApplication::sharedApplication()->getCurrentLanguage();
    // 判断下当前的语言版本，替换主题界面
    if ( language == kLanguageChinese)
    {
        CCSprite* pSprite = CCSprite::create("texture/mainboard/main_title_zh.png");
        m_pSpriteTitle->setDisplayFrame(pSprite->displayFrame());
    }
    
    
#ifdef ENABLE_UMENG_DATA
    // 得到友盟配置的公告数据，跑马灯形式在上面滚动显示
    const char* pszSuffix = "_en";
    if (language == kLanguageChinese)
        pszSuffix = "_zh";
    
    CCString* pStrNoticeKeyName = CCString::createWithFormat("FirstPageNotice%s", pszSuffix);
    std::string strNotice = MobClickCpp::getConfigParams(pStrNoticeKeyName->getCString());
    if (!strNotice.empty())
    {
        
        if (!m_pLabelTTFNotice)
        {
            // 滚动显示公告
            m_pLabelTTFNotice = CCLabelTTF::create(strNotice.c_str(), "", size.width/18);
            m_pLabelTTFNotice->setAnchorPoint(ccp(0, 0.5f));
            addChild(m_pLabelTTFNotice);
            // 把字体放到背景之上
            m_pLabelTTFNotice->setZOrder(10);
        }
        
        // 背景展开
        if (!m_pSpriteNoticeBackground)
        {
            m_pSpriteNoticeBackground = CCSprite::create("texture/mainboard/firstPageNoticeBackground.png");
            m_pSpriteNoticeBackground->setOpacity(190);
            addChild(m_pSpriteNoticeBackground);
        }
        
        float fSpeed = 640/6.0f;
        float fDuration = m_pLabelTTFNotice->getContentSize().width/fSpeed;
        
        m_pSpriteNoticeBackground->stopAllActions();
        m_pSpriteNoticeBackground->setScaleY(0);
        m_pSpriteNoticeBackground->setPosition(ccp(size.width*0.5f, size.height*0.85f));
        m_pSpriteNoticeBackground->runAction(CCSequence::create(CCScaleTo::create(1.0f, 1.0f, 1.0f), CCDelayTime::create(fDuration),
                                                                CCScaleTo::create(0.5f, 1.0f, 0), NULL));
        
        m_pLabelTTFNotice->stopAllActions();
        m_pLabelTTFNotice->setPosition(ccp(size.width, size.height*0.85f));
        m_pLabelTTFNotice->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCMoveTo::create(fDuration, ccp(-m_pLabelTTFNotice->getContentSize().width, m_pLabelTTFNotice->getPositionY())), NULL));
    }
#endif
    
    // 删除下未用到的纹理缓存
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    
}

void BalloonFirstPage::onExit()
{
	// CCLayer::onExit();
    // 退出场景时，把菜单拎到屏幕外，进场时直接落下即可
    m_pMenuMain->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*1.5f));
    
    AutoTextureManagerLayer::onExit();
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
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pMenuMain", CCMenu*, this->m_pMenuMain);
	CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpriteTitle", CCSprite*, this->m_pSpriteTitle);

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
    
    CCMenuItemImage* pMenuItemShop = CCMenuItemImage::create();
    pMenuItemShop->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_shop.png"));
    pMenuItemShop->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuShop));
    
    CCMenuItemImage* pMenuItemUserinfo = CCMenuItemImage::create();
    pMenuItemUserinfo->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_userinfo.png"));
    pMenuItemUserinfo->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuUserinfo));
    
    pMenuItemStart->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemShop->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    pMenuItemUserinfo->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    
    m_pMenuMain->addChild(pMenuItemStart);
    m_pMenuMain->addChild(pMenuItemUserinfo);

#if 1 // (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCMenuItemImage* pMenuItemRankBoard = CCMenuItemImage::create();
    pMenuItemRankBoard->setNormalSpriteFrame(pSpriteFrameCache->spriteFrameByName("menu_item_rank.png"));
    pMenuItemRankBoard->setTarget(this, menu_selector(BalloonFirstPage::onPressMenuRankBoard));
    pMenuItemRankBoard->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, 10.0f), CCDelayTime::create(rand()%6+1), CCRotateTo::create(2.0f, -10.0f), NULL)));
    m_pMenuMain->addChild(pMenuItemRankBoard);
#endif
    
    m_pMenuMain->addChild(pMenuItemShop);
    
    ControlUtil::sharedControlUtil()->SetMenuItemSelectedImageWithNormalImage(m_pMenuMain);
    
    m_pMenuMain->alignItemsVerticallyWithPadding(pMenuItemShop->getContentSize().height*0.5f);
    
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

    addChild(BalloonRankDialog::create());
}

void BalloonFirstPage::onPressMenuShop(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    /*
    CCScene* pScene = BalloonShopScene::scene();
    CCDirector::sharedDirector()->pushScene(CCTransitionCrossFade::create(0.5f, pScene));
    */
    addChild(BalloonShopDialog::create());
}

void BalloonFirstPage::onPressMenuUserinfo(cocos2d::CCObject *pSender)
{
    BalloonSoundManager::sharedBalloonSoundManager()->playEffectPushBalloon();
    
    // 用户个人信息对话框
    BalloonUserInfoDialog* pDialog = BalloonUserInfoDialog::create();
    addChild(pDialog);
    
    /*
    // 把得分上传到服务器
    CCDictionary* pDict = CCDictionary::create();
    pDict->setObject(ccs("12345678901234567890123456789012"), "uid");
    pDict->setObject(ccs("JasonTou"), "uname");
    
    const char* pszJSON = CCJSONConverter::sharedConverter()->strFrom(pDict);
    // 请求Http数据
    HttpCenter::sharedHttpCenter()->request("http://121.40.76.13/score_rank.php", NULL, CCHttpRequest::kHttpPost, pszJSON);
    
    free((void*)pszJSON);
    */
}

bool BalloonFirstPage::setResourceString()
{
//    m_vTexturesString.push_back("texture/menuItems/menuItems.plist");
    m_vTexturesString.push_back("texture/balloon/balloon.plist");
    m_vTexturesString.push_back("texture/items/items.plist");
	m_vTexturesString.push_back("texture/mainboard/firstPageNoticeBackground.png");
	m_vTexturesString.push_back("texture/mainboard/main_background.png");
	m_vTexturesString.push_back("texture/mainboard/main_title_en.png");
	m_vTexturesString.push_back("texture/mainboard/main_title_zh.png");
    return true;
}

#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "CompanyPage.h"
#include "BalloonFirstPage.h"
#include "UMSocial2DX.h"
#include "GAdMob2DX.h"
#include "bailinUtil.h"

#ifdef ENABLE_UMENG_DATA
#   include "MobClickCpp.h"
#endif

USING_NS_CC;
USING_NS_BAILIN_UTIL;

#define JNI_CLASS "com/wardrums/pokeballoon/PokeBalloon"

#define OPT_MUSIC_OFF           "opt_MusicOFF"
#define OPT_SOUND_EFFECT_OFF    "opt_SoundEffectOFF"

using namespace CocosDenshion;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

// ��ʼ����������
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)

	// ����������ʵ��
    GAdMob2DX* pGADInstance = GAdMob2DX::sharedGAdMob2DX();

#   if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("5352425256240b09f407dee2");
#       endif
    UMSocial2DX::setAppKey("5352425256240b09f407dee2");

	// ����iOS���ID
    pGADInstance->init("ca-app-pub-9727130637201516/8034810581");

#	elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("535242b756240b0a0506ca56");
#       endif
    UMSocial2DX::setAppKey("535242b756240b0a0506ca56", JNI_CLASS);
	
	// ����Android���ID�Լ�Android��Ӧ�İ���
    pGADInstance->init("ca-app-pub-9727130637201516/7662720589", JNI_CLASS);
#	endif

#   ifdef ENABLE_UMENG_DATA
#       if COCOS2D_DEBUG > 0
	MobClickCpp::setLogEnabled(true);
#       endif

    // ���汾����
    MobClickCpp::checkUpdate();
    
    // ����������������
    MobClickCpp::updateOnlineConfig();
    
#   endif
    
    
#endif
    
    // ��ȡ������������
    setLocalConfigData();

    pDirector->setOpenGLView(pEGLView);
    
    CCSize screenSize = CCEGLView::sharedOpenGLView()->getFrameSize();

    CCSize resSize = CCSizeMake(640, 960);
    
    float fScaleFactor = 1.0f;
    if (screenSize.height/screenSize.width > resSize.height/resSize.width)
    {
        // ��Ļ�Ŀ�߱ȴ����趨�Ŀ�߱ȣ���ʶ��Ҫ���ո߶ȵı�׼����
        fScaleFactor = resSize.width / screenSize.width;
    }
    else
    {
        fScaleFactor = resSize.height / screenSize.height;
    }
    
    pDirector->setContentScaleFactor(fScaleFactor);
    pEGLView->setDesignResolutionSize(screenSize.width, screenSize.height, kResolutionShowAll);
    
    // ��ʼ�����������
    srand(time(NULL));
    

#if COCOS2D_DEBUG > 0
    // turn on display FPS
    pDirector->setDisplayStats(true);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	CCScene *pScene = CompanyPage::scene();
#else
    CCScene *pScene = BalloonFirstPage::scene();
#endif

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    
    // ����ͳ�ƽ����̨
    MobClickCpp::applicationDidEnterBackground();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // Ԥ������ѡ��
    if (!DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong("opt_MusicOFF"))
    {
        // if you use SimpleAudioEngine, it must resume here
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    }
    
    // ����ͳ�Ʒ����ص�ǰ̨
    MobClickCpp::applicationWillEnterForeground();
}

void AppDelegate::setLocalConfigData()
{
    DataManagerUtil* pDMU = DataManagerUtil::sharedDataManagerUtil();
    
    CCAssert(pDMU, "DataManagerUtil instance create failed!");
    
    if (pDMU)
    {
        // ��ȡ��Ч����������
        int nValue = CCUserDefault::sharedUserDefault()->getIntegerForKey(OPT_MUSIC_OFF, 0);
        pDMU->SetGlobalDataLong(OPT_MUSIC_OFF, nValue);
        nValue = CCUserDefault::sharedUserDefault()->getIntegerForKey(OPT_SOUND_EFFECT_OFF, 0);
        pDMU->SetGlobalDataLong(OPT_SOUND_EFFECT_OFF, nValue);
    }
}
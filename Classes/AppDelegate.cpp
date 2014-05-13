#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "BalloonFirstPage.h"
#include "UMSocial2DX.h"
#include "GAdMob2DX.h"
#include "MobClickCpp.h"

USING_NS_CC;

#define JNI_CLASS "com/wardrums/pokeballoon/PokeBalloon"

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

// 初始化友盟数据
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)

#   if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    MobClickCpp::startWithAppkey("5352425256240b09f407dee2");
    UMSocial2DX::setAppKey("5352425256240b09f407dee2");

#	elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    MobClickCpp::startWithAppkey("535242b756240b0a0506ca56");
    UMSocial2DX::setAppKey("535242b756240b0a0506ca56", JNI_CLASS);
	
#	endif

#	if COCOS2D_DEBUG > 0
	MobClickCpp::setLogEnabled(true);
#	endif

    MobClickCpp::checkUpdate();
    
    GAdMob2DX* pGADInstance = GAdMob2DX::sharedGAdMob2DX();
    
	// 设置广告ID以及Android对应的包名
    pGADInstance->init("ca-app-pub-9727130637201516/8034810581", JNI_CLASS);
#endif

    pDirector->setOpenGLView(pEGLView);
    
    CCSize screenSize = pDirector->getWinSize();
    CCSize resSize = CCSizeMake(960, 640);
    
    float fScaleFactor = 1.0f;
    if (screenSize.height/screenSize.width > resSize.height/resSize.width)
    {
        // 屏幕的宽高比大于设定的宽高比，标识需要按照高度的标准拉伸
        fScaleFactor = screenSize.width / resSize.width;
    }
    else
    {
        fScaleFactor = screenSize.height / resSize.height;
    }
    
    pDirector->setContentScaleFactor(fScaleFactor);
    pEGLView->setDesignResolutionSize(screenSize.width, screenSize.height, kResolutionShowAll);
    
    // 初始化随机数种子
    srand(time(NULL));
    

#if COCOS2D_DEBUG > 0
    // turn on display FPS
    pDirector->setDisplayStats(true);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = BalloonFirstPage::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    
    // 多盟统计进入后台
    MobClickCpp::applicationDidEnterBackground();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    
    // 多盟统计分析回到前台
    MobClickCpp::applicationWillEnterForeground();
}

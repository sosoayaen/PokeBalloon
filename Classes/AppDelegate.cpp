#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "BalloonSoundManager.h"
#include "CompanyPage.h"
#include "BalloonFirstPage.h"
#include "UMSocial2DX.h"
#include "GAdMob2DX.h"
#include "bailinUtil.h"
#include "Balloon_macro.h"
#include "BalloonAnalysis.h"
#include "AppInfo.h"
#include "NDKBridge.h"
#include "UserData.h"

#ifdef ENABLE_UMENG_DATA
#   include "MobClickCpp.h"
#endif

USING_NS_CC;
USING_NS_BAILIN_UTIL;

#define JNI_CLASS "com/wardrums/lib/WDBaseActivity"

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

	// 创建广告控制实例
    GAdMob2DX* pGADInstance = GAdMob2DX::sharedGAdMob2DX();

#   if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("5352425256240b09f407dee2");
#       endif
    UMSocial2DX::setAppKey("5352425256240b09f407dee2");

	// 设置iOS广告ID
    pGADInstance->init("ca-app-pub-4946557086550003/5403608979");

#	elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("535242b756240b0a0506ca56");
#       endif
    UMSocial2DX::setAppKey("535242b756240b0a0506ca56", JNI_CLASS);
	
	// 设置Android广告ID以及Android对应的包名
    pGADInstance->init("ca-app-pub-4946557086550003/8357075374", JNI_CLASS);
#	endif

#   ifdef ENABLE_UMENG_DATA
#       if COCOS2D_DEBUG > 0
	MobClickCpp::setLogEnabled(true);
#       endif

    // 检查版本更新
    MobClickCpp::checkUpdate();
    
    // 更新在线配置数据
    MobClickCpp::updateOnlineConfig();
    
#   endif
    
    
#endif

	// 得到程序的版本号以及编译版本值
	std::string strVersion = AppInfo::sharedAppInfo()->getVersionName();
	int nVersionCode = AppInfo::sharedAppInfo()->getVersionCode();

	// 把对应的值存入全局数据中
	DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataString("versionName", strVersion.c_str());
	DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong("versionCode", nVersionCode);
    
    // 读取本地配置数据
    setLocalConfigData();

    pDirector->setOpenGLView(pEGLView);
    
    CCSize screenSize = CCEGLView::sharedOpenGLView()->getFrameSize();

    CCSize resSize = CCSizeMake(640, 960);
    
    float fScaleFactor = 1.0f;
    if (screenSize.height/screenSize.width > resSize.height/resSize.width)
    {
        // 屏幕的宽高比大于设定的宽高比，标识需要按照高度的标准拉伸
        fScaleFactor = resSize.width / screenSize.width;
    }
    else
    {
        fScaleFactor = resSize.height / screenSize.height;
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	CCScene *pScene = CompanyPage::scene();
#else
    CCScene *pScene = BalloonFirstPage::scene();
#endif

    // run
    pDirector->runWithScene(pScene);

    // 初始化载入音效文件
    BalloonSoundManager::sharedBalloonSoundManager();
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
#ifdef ENABLE_UMENG_DATA
    // 多盟统计进入后台
    MobClickCpp::applicationDidEnterBackground();
#endif
    // 先把音效文件清理掉
    BalloonSoundManager::sharedBalloonSoundManager()->unloadAllEffect();
    
    CCNotificationCenter::sharedNotificationCenter()->postNotification(NOTIFY_PAUSE);
    
    // 保存本地数据
    UserDataManager::sharedUserDataManager()->saveData();
    
    // 设置推送消息
    CCDictionary* dict = CCDictionary::create();
    dict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("notification_section", "localNotificationText")), "notificationText");
    dict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("notification_section", "localNotificationName")), "notificationName");
#if COCOS2D_DEBUG > 0
    // 测试数据15秒发送
    dict->setObject(ccs("15"), "timeinterval");
#endif
    NDKBridge::sharedNDKBridge()->setNotification(dict);
    dict->release();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // 再次加载音效
    BalloonSoundManager::sharedBalloonSoundManager()->preloadAllEffect();
    
    // 预读音乐选项
    if (!DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_MUSIC_OFF))
    {
        // if you use SimpleAudioEngine, it must resume here
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    }
    
#ifdef ENABLE_UMENG_DATA
    // 多盟统计分析回到前台
    MobClickCpp::applicationWillEnterForeground();
#endif
    
    CCNotificationCenter::sharedNotificationCenter()->postNotification(NOTIFY_RESUME);
    
    // 取消当前推送通知
    CCDictionary* dict = CCDictionary::create();
    dict->setObject(ccs(DataManagerUtil::sharedDataManagerUtil()->GetUTF8StringInDictionary("notification_section", "localNotificationName")), "notificationName");
    NDKBridge::sharedNDKBridge()->cancelNotification(dict);
    dict->release();
}

void AppDelegate::setLocalConfigData()
{
    DataManagerUtil* pDMU = DataManagerUtil::sharedDataManagerUtil();
    
    CCAssert(pDMU, "DataManagerUtil instance create failed!");
    
    if (pDMU)
    {
        // 读取音效和音乐配置
        int nValue = CCUserDefault::sharedUserDefault()->getIntegerForKey(OPT_MUSIC_OFF, 0);
        pDMU->SetGlobalDataLong(OPT_MUSIC_OFF, nValue);
        nValue = CCUserDefault::sharedUserDefault()->getIntegerForKey(OPT_SOUND_EFFECT_OFF, 0);
        pDMU->SetGlobalDataLong(OPT_SOUND_EFFECT_OFF, nValue);
    }
    
    // 加载存在本地的用户数据
    UserDataManager::sharedUserDataManager()->loadData();
}

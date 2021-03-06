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
#include "BalloonMission.h"
#include "BalloonArchivement.h"

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
    
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // 启动IAP
    CCArray* pArrayProducts = CCArray::create();
    pArrayProducts->addObject(CCString::create("com.wardrums.pokeballoon.removead"));
    pArrayProducts->addObject(CCString::create("com.wardrums.pokeballoon.buyCoinsOne"));
    pArrayProducts->addObject(CCString::create("com.wardrums.pokeballoon.buyCoinsTwo"));
    pArrayProducts->addObject(CCString::create("com.wardrums.pokeballoon.buyCoinsThree"));
    NDKBridge::sharedNDKBridge()->initIAP(pArrayProducts, "76cf42e581eb48819b8456a8dcdc8129");
    NDKBridge::sharedNDKBridge()->setADProductID("com.wardrums.pokeballoon.removead");
    
#   if COCOS2D_DEBUG > 0
    NDKBridge::sharedNDKBridge()->clearSavedPurchasedProducts();
    // NDKBridge::sharedNDKBridge()->restoreIAPProducts();
#   endif
#endif

// 初始化友盟数据
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)

#   if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("5352425256240b09f407dee2");
#       endif
    UMSocial2DX::setAppKey("5352425256240b09f407dee2");

    // 这里判断下是否有去处广告的配置，有就不进行初始化
    NDKBridge::sharedNDKBridge()->initAdMob("ca-app-pub-4946557086550003/5403608979");

#	elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#       ifdef ENABLE_UMENG_DATA
    MobClickCpp::startWithAppkey("535242b756240b0a0506ca56");
#       endif
    UMSocial2DX::setAppKey("535242b756240b0a0506ca56", JNI_CLASS);
	
    // 初始化广告控件
    NDKBridge::sharedNDKBridge()->initAdMob("ca-app-pub-4946557086550003/8357075374", JNI_CLASS);
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
    

#if COCOS2D_DEBUG > 0 && !defined(FOR_APPSTORE_CAPTURE)
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
    // dict->setObject(ccs("15"), "timeinterval");
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
    
    // 配置的压缩文件包
    std::string strConfigZipFileName = "config";
    std::string strConfigurationFileName = "configuration.plist";
    // 可写目录中的配置文件
    std::string strConfiguration = CCFileUtils::sharedFileUtils()->getWritablePath() + strConfigurationFileName;
    
    // 加载本地配置数据，从对应的plist读取，比如游戏前购买道具的配置，图鉴的配置，还有技能的配置等
    do
    {
        // 判断配置文件压缩包是否存在（每次都释放覆盖）
        CC_BREAK_IF(!CCFileUtils::sharedFileUtils()->isFileExist(strConfigZipFileName.c_str()));
        std::string strZipFilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(strConfigZipFileName.c_str());
        // 把配置文件释放出来
        unsigned long len = 0;
        unsigned char* pszConfiguration = CCFileUtils::sharedFileUtils()->getFileDataFromZip(strZipFilePath.c_str(), "configuration/configuration.plist", &len);
        CC_BREAK_IF(len <= 0);
        FILE* file = fopen(strConfiguration.c_str(), "wb");
        CC_BREAK_IF(!file);
        if (file)
        {
            fwrite(pszConfiguration, 1, len, file);
            fclose(file);
        }
        delete pszConfiguration;
    } while (0);
    
    CCAssert(CCFileUtils::sharedFileUtils()->isFileExist(strConfiguration.c_str()), "configuration file not exist!");
    CCDictionary* pDictConfiguration = CCDictionary::createWithContentsOfFileThreadSafe(strConfiguration.c_str());
    // 放到全局数据中
    if (pDictConfiguration)
        DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataObject("configuration", pDictConfiguration);
    
    // 加载任务数据
    BalloonMission::sharedBalloonMission()->loadAllMissionData();
    
    // 加载成就标准数据
    BalloonArchivement::sharedBalloonArchivement()->loadData();
}

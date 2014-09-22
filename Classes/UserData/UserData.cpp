/*
 * =====================================================================================
 *
 *       Filename:  UserData.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2014 14:18:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "UserData.h"
#include "Ability.h"
#include "bailinUtil.h"
#include <string>
#include "cocos2d.h"
#include "BalloonArchivement.h"
#include "NDKBridge.h"

USING_NS_CC;
USING_NS_BAILIN_UTIL;

#define KEY_HIGHEST_SCORE "HighestScore"    // 配置文件中用户的最高分key

#define KEY_NICKNAME "Nickname" // 配置文件中用户保存昵称的key

#define CHECK_GOLDEN_COINS "checkGoldenCoins"  // 金币校验key
#define CHECK_GAME_COUNTS   "checkGameCounts"  // 游戏盘数校验key

// 用作校验的字符串
static const char* g_pszItemExCheckCodeKeyArray[] =
{
    "checkItemExPump",
    "checkItemExPreTime",
    "checkItemExTimeAdd",
    "checkItemExGiant",
    "checkItemExSafeGuard",
};

static const char* g_pszAbilityCheckCodeKeyArray[] =
{
    "checkAbilityNormalRange",    // 普通气球的分数随机上限等级
    "checkAbilityBoomGuard",      // 拆弹小队等级
    "checkAbilityTimeSlash",      // 时间老人恩赐，时间暴击概率
    "checkAbilityPumpCount",      // 气球可点击的次数上下限次数
    "checkAbilityMultiCritical",  // 乘分气球暴击概率
    "checkAbilityReverse",        // 点击逆转只影响负分转正分的概率
    "checkAbilityGiantMulti",     // 巨人气球点爆后50%概率得到多倍分数的等级
};

#define KEY_CONFIG_GOLDEN_COINS "GoldenCoins" // 配置文件中的金币的key
#define KEY_CONFIG_GAME_COUNTS "GameCounts"   // 配置文件中游戏盘数的key

// 用作保存和读取配置的字符串的key
static const char* g_pszItemExConfigKeyArray[] =
{
    "ItemExPump",
    "ItemExPreTime",
    "ItemExTimeAdd",
    "ItemExGiant",
    "ItemExSafeGuard",
};

static const char* g_pszAbilityConfigKeyArray[] =
{
    "AbilityNormalRange",    // 普通气球的分数随机上限等级
    "AbilityBoomGuard",      // 拆弹小队等级
    "AbilityTimeSlash",      // 时间老人恩赐，时间暴击概率
    "AbilityPumpCount",      // 气球可点击的次数上下限次数
    "AbilityMultiCritical",  // 乘分气球暴击概率
    "AbilityReverse",        // 点击逆转只影响负分转正分的概率
    "AbilityGiantMulti",     // 巨人气球点爆后50%概率得到多倍分数的等级
};

UserDataManager::UserDataManager()
{
	// 初始化重置用户数据
	memset(&m_UserData, 0, sizeof(UserData));
    
    m_GlobalAnalysisData.loadData();
    
    m_pArrayArchivementData = NULL;
    
    // 这里顺便初始化下用户等级描述数据
    AbilityManager::sharedAbilityManager();
}

UserDataManager::~UserDataManager()
{
    CC_SAFE_RELEASE(m_pArrayArchivementData);
}

static UserDataManager* g_sUserDataManager = NULL;

UserDataManager* UserDataManager::sharedUserDataManager()
{
	if (NULL == g_sUserDataManager)
	{
		g_sUserDataManager = new UserDataManager;
	}

	return g_sUserDataManager;
}

bool UserDataManager::verifyGoldenCoins()
{
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(CHECK_GOLDEN_COINS, m_UserData.llGoldenCoins);
    CCAssert(bRet, "Golden Coins verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setGoldenCoinsCheckCode()
{
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(CHECK_GOLDEN_COINS, crypto::Crc32(&m_UserData.llGoldenCoins, sizeof(m_UserData.llGoldenCoins)));
}

bool UserDataManager::verifyGameCounts()
{
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(CHECK_GAME_COUNTS, m_UserData.llGameCounts);
    CCAssert(bRet, "Game Counts verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setGameCountsCheckCode()
{
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(CHECK_GAME_COUNTS, crypto::Crc32(&m_UserData.llGameCounts, sizeof(m_UserData.llGameCounts)));
}

void UserDataManager::loadData()
{
    // 得到UDID，并且MD5一下，这样就可以统一UDID在不同平台下的长度
    m_strUDID = crypto::MD5(NDKBridge::sharedNDKBridge()->getDeviceUDID().c_str());
    
    // 从配置文件中获取用户名
    setNickName(CCUserDefault::sharedUserDefault()->getStringForKey(KEY_NICKNAME, ""));
    if (getNickName().empty())
    {
        setNickName(NDKBridge::sharedNDKBridge()->getDeviceName());
        if (getNickName().empty())
        {
            setNickName(CCString::createWithFormat("Burst Man%03d", rand()%100)->m_sString);
        }
    }
    
    // 得到统计数据
    m_GlobalAnalysisData.loadData();
    
    // 得到金币数据
    std::string strCoins = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_GOLDEN_COINS);
    // 转化到long long
    m_UserData.llGoldenCoins = atoll(strCoins.c_str());
    
#if COCOS2D_DEBUG > 0
    // 不足1万，每次加1万
    if (m_UserData.llGoldenCoins < 10000)
        m_UserData.llGoldenCoins += 10000;
#endif
    // 设置金币初始校验值
    setGoldenCoinsCheckCode();
    
    // 得到游戏盘数
    std::string strGameCounts = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_GAME_COUNTS);
    m_UserData.llGameCounts = atoll(strGameCounts.c_str());
    // 设置游戏盘数的初始校验值
    setGameCountsCheckCode();
   
    // 道具的个数
    unsigned int nItemCounts = sizeof(ItemExtend)/sizeof(long);
    for (unsigned int i = 0; i < nItemCounts; ++i)
    {
        ItemExType eType = (ItemExType)i;
        // 得到道具的数值
        std::string strItemName = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(g_pszItemExConfigKeyArray[i]);
        long* pItemData = (long*)&m_UserData.itemEx + i;
        *pItemData = atol(strItemName.c_str());
        // 设置对应道具的初始校验值
        setItemExCheckCodeByID(eType);
    }
    
    // 用户技能级别
    unsigned int nAbilityLvlCounts = sizeof(UserAbilityLevel)/sizeof(long);
    for (unsigned int i = 0; i < nAbilityLvlCounts; ++i)
    {
        UserAbilityLevelType eType = (UserAbilityLevelType)i;
        // 得到级别
        std::string strAbilityLvlName = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(g_pszAbilityConfigKeyArray[i]);
        long* pAbilityLvlData = (long*)&m_AbilityLevel + i;
        *pAbilityLvlData = atol(strAbilityLvlName.c_str());
        // 设定对应的初始校验值
        setAbilityCheckCodeByID(eType);
    }
    
}

void UserDataManager::saveData()
{
    // 保存用户的昵称数据
    CCUserDefault::sharedUserDefault()->setStringForKey(KEY_NICKNAME, getNickName());
    
    // 保存数据
    m_GlobalAnalysisData.saveData();
    
    // 保存金币数据
    if (!verifyGoldenCoins())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.llGoldenCoins = 0;
        setGoldenCoinsCheckCode();
    }
    
    // 保存游戏金币
    std::stringstream stream;
    stream << m_UserData.llGoldenCoins;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(KEY_CONFIG_GOLDEN_COINS, stream.str().c_str());
    
    if (!verifyGameCounts())
    {
        m_UserData.llGameCounts = 0;
        setGameCountsCheckCode();
    }
    
    // 保存游戏盘数
    stream.str("");
    stream << m_UserData.llGameCounts;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(KEY_CONFIG_GAME_COUNTS, stream.str().c_str());
    
    // 保存道具数目
    // 道具的个数
    unsigned int nItemCounts = sizeof(ItemExtend)/sizeof(long);
    for (unsigned int i = 0; i < nItemCounts; ++i)
    {
        ItemExType eType = (ItemExType)i;
        long* pItemData = (long*)&m_UserData.itemEx + i;
        if (!verifyItemExByID(eType))
        {
            *pItemData = 0;
            setItemExCheckCodeByID(eType);
        }
        stream.str("");
        stream << *pItemData;
        DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(g_pszItemExConfigKeyArray[i], stream.str().c_str());
    }
    
    // 保存技能级别
    unsigned int nAbilityCounts = sizeof(UserAbilityLevel)/sizeof(long);
    for (unsigned int i = 0; i < nAbilityCounts; ++i)
    {
        UserAbilityLevelType eType = (UserAbilityLevelType)i;
        long* pAbilityLvlData = (long*)&m_AbilityLevel + i;
        if (!verifyAbilityByID(eType))
        {
            *pAbilityLvlData = 0;
            setAbilityCheckCodeByID(eType);
        }
        stream.str("");
        stream << *pAbilityLvlData;
        DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(g_pszAbilityConfigKeyArray[i], stream.str().c_str());
    }
}

BalloonAnalysis* UserDataManager::getAnalysisDataRef()
{
    return &m_AnalysisData;
}

BalloonGlobalAnalysis* UserDataManager::getGlobalAnalysisDataRef()
{
    return &m_GlobalAnalysisData;
}

long long UserDataManager::getGoldenCoins()
{
    // 这里先校验下金币数量和之前的金币数量是否匹配
    if (!verifyGoldenCoins())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.llGoldenCoins = 0;
        setGoldenCoinsCheckCode();
    }
    return m_UserData.llGoldenCoins;
}

void UserDataManager::addGoldenCoins(long long llCoins)
{
    // 校验原始数据是否被修改
    if (verifyGoldenCoins())
    {
        m_UserData.llGoldenCoins += llCoins;
        // 设置新的校验和
        setGoldenCoinsCheckCode();
    }
}

bool UserDataManager::subGoldenCoins(long long llCoins)
{
    bool bRet = isGoldenEnough(llCoins);
    
    if (bRet)
    {
        if (!verifyGoldenCoins())
        {
            m_UserData.llGoldenCoins = 0;
            setGoldenCoinsCheckCode();
            return false;
        }
        
        // 减去对应的数据
        m_UserData.llGoldenCoins -= llCoins;
        setGoldenCoinsCheckCode();
    }
    
    return bRet;
}

bool UserDataManager::isGoldenEnough(long long llCoins)
{
    bool bRet = false;
    if (m_UserData.llGoldenCoins >= llCoins)
    {
        bRet = true;
    }
    return bRet;
}

long long UserDataManager::getGameCounts()
{
    // 这里先校验下游戏盘数数量和之前的游戏盘数数量是否匹配
    if (!verifyGameCounts())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.llGameCounts = 0;
        setGameCountsCheckCode();
    }
    return m_UserData.llGameCounts;
}

void UserDataManager::addOneGameCount()
{
    // 这里先校验下游戏盘数数量和之前的游戏盘数数量是否匹配
    if (!verifyGameCounts())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.llGameCounts = 0;
        setGameCountsCheckCode();
    }
    else
    {
        m_UserData.llGameCounts++;
        setGameCountsCheckCode();
    }
}

long UserDataManager::getItemExCountsByID(ItemExType type)
{
    int nIdx = int(type);
    // 定位到数据结构的初始位置
    long* pItemData = (long*)&m_UserData.itemEx + nIdx;
    // 校验数据
    if (!verifyItemExByID(type))
    {
        // 设置对应值为0
        *pItemData = 0;
        setItemExCheckCodeByID(type);
    }
    
    return *pItemData;
    
}

long UserDataManager::getAbilityLvlByID(UserAbilityLevelType eType)
{
    
    int nIdx = int(eType);
    // 定位到数据结构的初始位置
    long* pItemData = (long*)&m_AbilityLevel + nIdx;
    // 校验数据
    if (!verifyAbilityByID(eType))
    {
        // 设置对应值为0
        *pItemData = 0;
        setAbilityCheckCodeByID(eType);
    }
    
    return *pItemData;
}

long UserDataManager::addItemExCountsByID(ItemExType eType, long lValue)
{
    int nIdx = int(eType);
    long* plData = (long*)&m_UserData.itemEx + nIdx;
    
    if (!verifyItemExByID(eType))
    {
        // 设置值为0
        *plData = 0;
    }
    else
    {
        // 加上对应值
        *plData += lValue;
    }
    setItemExCheckCodeByID(eType);
    
    return *plData;
    
}

long UserDataManager::addAbilityLvlByID(UserAbilityLevelType eType, long lValue)
{
    int nIdx = int(eType);
    long* plData = (long*)&m_AbilityLevel + nIdx;
    
    if (!verifyAbilityByID(eType))
    {
        // 设置值为0
        *plData = 0;
    }
    else
    {
        // 加上对应值
        *plData += lValue;
    }
    setAbilityCheckCodeByID(eType);
    
    return *plData;
}

long UserDataManager::addItemExOneCountByID(ItemExType type)
{
    return addItemExCountsByID(type, 1);
}

long UserDataManager::addAbilityLvlOneByID(UserAbilityLevelType eType)
{
    return addAbilityLvlByID(eType, 1);
}

bool UserDataManager::verifyItemExByID(ItemExType eType)
{
    int nIdx = int(eType);
    const long* plData = (const long*)&m_UserData.itemEx + nIdx;
    long lValue = *plData;
    
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(g_pszItemExCheckCodeKeyArray[nIdx], lValue);
    CCAssert(bRet, "Item counts verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setItemExCheckCodeByID(ItemExType eType)
{
    int nIdx = int(eType);
    const long* plData = (const long*)&m_UserData.itemEx + nIdx;
    long lValue = *plData;
    unsigned long code = crypto::Crc32(&lValue, sizeof(lValue));
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(g_pszItemExCheckCodeKeyArray[nIdx], code);
    // CCLOG("ItemEx:%lu, value:%ld", code, lValue);
}

bool UserDataManager::verifyAbilityByID(UserAbilityLevelType eType)
{
    int nIdx = int(eType);
    const long* plData = (const long*)&m_AbilityLevel + nIdx;
    long lValue = *plData;
    
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(g_pszAbilityCheckCodeKeyArray[nIdx], lValue);
    CCAssert(bRet, "Ability verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setAbilityCheckCodeByID(UserAbilityLevelType eType)
{
    int nIdx = int(eType);
    const long* plData = (const long*)&m_AbilityLevel + nIdx;
    long lValue = *plData;
    unsigned long code = crypto::Crc32(&lValue, sizeof(lValue));
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(g_pszAbilityCheckCodeKeyArray[nIdx], code);
}

const std::string& UserDataManager::getNickName() const
{
    return m_strNickName;
}

void UserDataManager::setNickName(const std::string &strNickName)
{
    m_strNickName = strNickName;
}

long long UserDataManager::getHighestScore() const
{
    std::string strHighScore = DataManagerUtil::sharedDataManagerUtil()->ReadDataFromLocal(KEY_HIGHEST_SCORE);
    return atoll(strHighScore.c_str());
}

void UserDataManager::setHighestScore(long long llHighestScore)
{
    const std::string strHighestScore = CCString::createWithFormat("%lld", llHighestScore)->m_sString;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataToLocal(KEY_HIGHEST_SCORE, strHighestScore);
}

long UserDataManager::getArchivmentValue()
{
    // 计算所有成就值并返回
    CCArray* pArrayArchivement = getArchivementData(true);
    
    long lValue = 0;
    
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(pArrayArchivement, pObj)
    {
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
        // 先看是否unlock了
        if (pDict->valueForKey("unlocked")->boolValue())
        {
            // 每档成就增加100分
            lValue += pDict->valueForKey("level")->intValue()*100;
        }
    }
    return lValue;
}

const std::string& UserDataManager::getDeviceUDID() const
{
    return m_strUDID;
}

CCArray* UserDataManager::getArchivementData(bool bUseCache)
{
    // 如果没有数据，则先计算生成一下
    if (!m_pArrayArchivementData || !bUseCache)
    {
        updateArchivementData();
    }
    return m_pArrayArchivementData;
}

void UserDataManager::updateArchivementData()
{
    // 保存老的数据以供比对
    CCArray* pOldArray = m_pArrayArchivementData;
    // 给一个新的数据
    m_pArrayArchivementData = CCArray::create();
    CC_SAFE_RETAIN(m_pArrayArchivementData);

    // 根据初始化的数据，计算用户是否已经完成相应的成就
    const ArchDataByIDMap& archDataMap = BalloonArchivement::sharedBalloonArchivement()->getArchDataMap();
    ArchDataByIDMap::const_iterator iter = archDataMap.begin();
    for (int idx = 0; iter != archDataMap.end(); ++iter, ++idx)
    {
        CCDictionary* pDict = CCDictionary::create();
        // 默认锁住
        bool bUnlock = false;
        // 用于比较的值
        long long llData = 0;
        // 根据分类来处理不同的分支数据，得到一个比较值即可
        const ArchData* pArchData = iter->second;
        switch (pArchData->indexType)
        {
            case 0: // 自己处理
                switch (pArchData->indexId)
                {
                    case 0: // 气球总数
                        llData = m_GlobalAnalysisData.getTotalBalloonCounts();
                        break;
                    case 1: // 道具气球个数
                        llData = m_GlobalAnalysisData.getItemBalloonCounts();
                        break;
                    case 2: // 普通气球个数
                        llData = m_GlobalAnalysisData.getNormalBalloonCounts();
                        break;
                }
                break;
            case 1: // 颜色气球
                llData = *((long long*)&m_GlobalAnalysisData.getAnalysisData().colorData + pArchData->indexId);
                break;
            case 2: // 普通气球
                llData = *((long long*)&m_GlobalAnalysisData.getAnalysisData().normalData + pArchData->indexId);
                break;
            case 3: // 道具气球
                llData = *((long long*)&m_GlobalAnalysisData.getAnalysisData().itemData + pArchData->indexId);
                break;
            case 4: // 道具使用
                break;
            default:
                break;
        }
        
        // 处理数据，得到进度百分比，
        long long llLastAim = 0;
        float fProgress = 0;
        unsigned int i = 0;
        for (; i < pArchData->milestoteArray.size(); ++i)
        {
            long long llAim = pArchData->milestoteArray[i];
            if (llAim > llData)
            {
                // 就在这档以下，得到进度
                fProgress = (llData - llLastAim)*1.0f/llAim;
                // 计算结束
                break;
            }
            llLastAim = llAim;
        }
        
        // 是否解锁标志
        bUnlock = i > 0;
        // 设置加解锁标志
        pDict->setObject(ccs(bUnlock ? "true" : "false"), "unlocked");
        // 目标当前进度
        pDict->setObject(CCString::createWithFormat("%.04f", fProgress), "progress");
        // 目标成就标题
        pDict->setObject(CCString::createWithFormat("%s", pArchData->titleArray[bUnlock ? i-1 : i].c_str()), "title");
        // 当前目标成就描述
        pDict->setObject(CCString::createWithFormat("%s", pArchData->descArray[bUnlock ? i-1 : i].c_str()), "desc");
        // 目标里程碑的值
        pDict->setObject(CCString::createWithFormat("%lld", pArchData->milestoteArray[i]), "milestone");
        // 当前积累的值
        pDict->setObject(CCString::createWithFormat("%lld", llData), "curr");
        // 当前成就级别
        pDict->setObject(CCString::createWithFormat("%d", i), "level");
        // 最高等级
        pDict->setObject(CCString::createWithFormat("%lu", pArchData->milestoteArray.size()), "maxLevel");
        
        // 数据进入数组
        m_pArrayArchivementData->addObject(pDict);
        
        // TODO: 这里还可以比对新老数据，把更新的数据放入一个队列中，然后通知外部哪些数据更新了
    }
    // 释放老的数据
    CC_SAFE_RELEASE(pOldArray);
}
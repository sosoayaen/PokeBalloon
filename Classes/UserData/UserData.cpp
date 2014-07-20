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
#include "bailinUtil.h"
#include <string>

USING_NS_BAILIN_UTIL;

#define CHECK_GOLDEN_COINS "checkGoldenCoins"  // 金币校验key
#define CHECK_GAME_COUNTS   "checkGameCounts"  // 游戏盘数校验key

// 用作校验的字符串
static const char* g_pszItemExCheckCodeKeyArray[] =
{
    "checkItemExPump",
    "checkItemExPreTime",
    "checkItemExTimeAdd",
    "checkItemExGiant",
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
};

UserDataManager::UserDataManager()
{
	// 初始化重置用户数据
	memset(&m_UserData, 0, sizeof(UserData));
    
    m_GlobalAnalysisData.loadData();
}

UserDataManager::~UserDataManager()
{

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
    // 得到统计数据
    m_GlobalAnalysisData.loadData();
    
    // 得到金币数据
    std::string strCoins = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_GOLDEN_COINS);
    // 转化到无符号长整形
    m_UserData.llGoldenCoins = atoll(strCoins.c_str());
    
#if COCOS2D_DEBUG > 0
    // 每次加1万
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
        *pItemData = atoll(strItemName.c_str());
        // 设置对应道具的初始校验值
        setItemExCheckCodeByID(eType);
    }
    
}

void UserDataManager::saveData()
{
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

long UserDataManager::addItemExOneCountByID(ItemExType type)
{
    return addItemExCountsByID(type, 1);
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
    CCLOG("ItemEx:%lu, value:%ld", code, lValue);
}
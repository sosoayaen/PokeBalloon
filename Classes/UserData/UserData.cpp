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
#include "string"

USING_NS_BAILIN_UTIL;

#define CHECK_GOLDEN_COINS "checkGoldenCoins"  // 金币校验key
#define CHECK_GAME_COUNTS   "checkGameCounts"  // 游戏盘数校验key
#define CHECK_ITEM_EX_PUMP   "checkItemExPump"  // 打气筒道具
#define CHECK_ITEM_EX_PRE_TIME   "checkItemExPreTime"  // 开始时间增加道具

#define KEY_CONFIG_GOLDEN_COINS "GoldenCoins" // 配置文件中的金币的key
#define KEY_CONFIG_GAME_COUNTS "GameCounts"   // 配置文件中游戏盘数的key
#define KEY_CONFIG_ITEM_EX_PUMP "ItemExPump"   // 打气筒扩展道具个数
#define KEY_CONFIG_ITEM_EX_PRE_TIME "ItemExPreTime"   // 开始时间增加道具

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

bool UserDataManager::verifyItemExPump()
{
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(CHECK_ITEM_EX_PUMP, m_UserData.itemEx.lPump);
    CCAssert(bRet, "Item pump counts verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setItemExPumpCheckCode()
{
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(CHECK_ITEM_EX_PUMP, crypto::Crc32(&m_UserData.itemEx.lPump, sizeof(m_UserData.itemEx.lPump)));
}

bool UserDataManager::verifyItemExPreTime()
{
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(CHECK_ITEM_EX_PRE_TIME, m_UserData.itemEx.lPreTime);
    CCAssert(bRet, "Item PreTime counts verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setItemExPreTimeCheckCode()
{
    unsigned long code = crypto::Crc32(&m_UserData.itemEx.lPreTime, sizeof(m_UserData.itemEx.lPreTime));
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(CHECK_ITEM_EX_PRE_TIME, code);
    CCLOG("preTimeCheckCode:%lu, value:%ld", code, m_UserData.itemEx.lPreTime);
}

void UserDataManager::loadData()
{
    // 得到统计数据
    m_GlobalAnalysisData.loadData();
    
    // 得到金币数据
    std::string strCoins = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_GOLDEN_COINS);
    // 转化到无符号长整形
    // m_UserData.llGoldenCoins = strtoll(strCoins.c_str(), NULL, 0);
    m_UserData.llGoldenCoins = atoll(strCoins.c_str());
    // 设置金币初始校验值
    setGoldenCoinsCheckCode();
    
    // 得到游戏盘数
    std::string strGameCounts = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_GAME_COUNTS);
    m_UserData.llGameCounts = atoll(strGameCounts.c_str());
    // 设置游戏盘数的初始校验值
    setGameCountsCheckCode();
    
    // 得到道具的数值
    std::string strItemPumps = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_ITEM_EX_PUMP);
    m_UserData.itemEx.lPump = atoll(strItemPumps.c_str());
    // 设置对应道具的初始校验值
    setItemExPumpCheckCode();
    
    // 得到道具的数值
    std::string strItemPreTime = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_CONFIG_ITEM_EX_PRE_TIME);
    m_UserData.itemEx.lPreTime = atoll(strItemPumps.c_str());
    // 设置对应道具的初始校验值
    setItemExPreTimeCheckCode();
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
    if (!verifyItemExPump())
    {
        m_UserData.itemEx.lPump = 0;
        setItemExPumpCheckCode();
    }
    stream.str("");
    stream << m_UserData.itemEx.lPump;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(KEY_CONFIG_ITEM_EX_PUMP, stream.str().c_str());
    
    if (!verifyItemExPreTime())
    {
        m_UserData.itemEx.lPreTime = 0;
        setItemExPreTimeCheckCode();
    }
    stream.clear();
    stream << m_UserData.itemEx.lPreTime;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(KEY_CONFIG_ITEM_EX_PRE_TIME, stream.str().c_str());
    
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

long UserDataManager::getItemExPumpCounts()
{
    if (!verifyItemExPump())
    {
        m_UserData.itemEx.lPump = 0;
        setItemExPumpCheckCode();
    }
    return m_UserData.itemEx.lPump;
}

long UserDataManager::getItemExPreTimeCounts()
{
    if (!verifyItemExPreTime())
    {
        m_UserData.itemEx.lPreTime = 0;
        setItemExPreTimeCheckCode();
    }
    return m_UserData.itemEx.lPreTime;
}

void UserDataManager::addItemExPumpCount(long lCnts)
{
    // 这里先校验下游戏盘数数量和之前的游戏盘数数量是否匹配
    if (!verifyItemExPump())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.itemEx.lPump = 0;
        setItemExPumpCheckCode();
    }
    else
    {
        m_UserData.itemEx.lPump += lCnts;
        setItemExPumpCheckCode();
    }
}

void UserDataManager::addOneItemExPump()
{
    // 这里先校验下游戏盘数数量和之前的游戏盘数数量是否匹配
    addItemExPumpCount(1);
}

void UserDataManager::addItemExPreTimeCount(long lCnts)
{
    // 这里先校验下游戏盘数数量和之前的游戏盘数数量是否匹配
    if (!verifyItemExPreTime())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.itemEx.lPreTime = 0;
        setItemExPreTimeCheckCode();
    }
    else
    {
        m_UserData.itemEx.lPreTime += lCnts;
        setItemExPreTimeCheckCode();
    }
}

void UserDataManager::addOneItemExPreTime()
{
    addItemExPreTimeCount(1);
}

long UserDataManager::getItemExCountsByID(ItemExType type)
{
    long lRet = 0;
    switch (type) {
        case kCCItemExTypePump:
            lRet = getItemExPumpCounts();
            break;
        case kCCItemExTypePreTime:
            lRet = getItemExPreTimeCounts();
            break;
        default:
            break;
    }
    return lRet;
}

long UserDataManager::addItemExOneCountByID(ItemExType type)
{
    long lRet = 0;
    switch (type) {
        case kCCItemExTypePump:
            addOneItemExPump();
            lRet = m_UserData.itemEx.lPump;
            break;
        case kCCItemExTypePreTime:
            addOneItemExPreTime();
            lRet = m_UserData.itemEx.lPreTime;
            break;
        default:
            break;
    }
    return lRet;
}
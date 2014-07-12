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

#define CHECK_GOLDEN_COINS "goldenCoins"  // 金币校验key
#define KEY_DATA_GOLDEN_COINS "GoldenCoins" // 配置文件中的金币的key

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
    bool bRet = DataManagerUtil::sharedDataManagerUtil()->CheckSecurityData(CHECK_GOLDEN_COINS, m_UserData.ulGoldenCoins);
    CCAssert(bRet, "Golden Coins verify failed ... cheating!!!");
    return bRet;
}

void UserDataManager::setGoldenCoinsCheckCode()
{
    DataManagerUtil::sharedDataManagerUtil()->SetSecurityCode(CHECK_GOLDEN_COINS, crypto::Crc32(&m_UserData.ulGoldenCoins, sizeof(m_UserData.ulGoldenCoins)));
}

void UserDataManager::loadData()
{
    // 得到统计数据
    m_GlobalAnalysisData.loadData();
    
    // 得到金币数据
    std::string strCoins = DataManagerUtil::sharedDataManagerUtil()->ReadDataWithChecksum(KEY_DATA_GOLDEN_COINS);
    // 转化到无符号长整形
    m_UserData.ulGoldenCoins = strtoul(strCoins.c_str(), NULL, 0);
    // 设置初始值
    setGoldenCoinsCheckCode();
}

void UserDataManager::saveData()
{
    // 保存数据
    m_GlobalAnalysisData.saveData();
    
    // 保存金币数据
    if (!verifyGoldenCoins())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.ulGoldenCoins = 0;
        setGoldenCoinsCheckCode();
    }
    
    std::stringstream stream;
    stream << m_UserData.ulGoldenCoins;
    DataManagerUtil::sharedDataManagerUtil()->WriteDataWithChecksum(KEY_DATA_GOLDEN_COINS, stream.str().c_str());
    
}

BalloonAnalysis* UserDataManager::getAnalysisDataRef()
{
    return &m_AnalysisData;
}

BalloonGlobalAnalysis* UserDataManager::getGlobalAnalysisDataRef()
{
    return &m_GlobalAnalysisData;
}

unsigned long UserDataManager::getGoldenCoins()
{
    // 这里先校验下金币数量和之前的金币数量是否匹配
    if (!verifyGoldenCoins())
    {
        // 如果不匹配直接清零或者还原
        m_UserData.ulGoldenCoins = 0;
        setGoldenCoinsCheckCode();
    }
    return m_UserData.ulGoldenCoins;
}

void UserDataManager::addGoldenCoins(unsigned long ulCoins)
{
    // 校验原始数据是否被修改
    if (verifyGoldenCoins())
    {
        m_UserData.ulGoldenCoins += ulCoins;
        // 设置新的校验和
        setGoldenCoinsCheckCode();
    }
}

bool UserDataManager::subGoldenCoins(unsigned long ulCoins)
{
    bool bRet = isGoldenEnough(ulCoins);
    
    if (bRet)
    {
        if (!verifyGoldenCoins())
        {
            m_UserData.ulGoldenCoins = 0;
            setGoldenCoinsCheckCode();
            return false;
        }
        
        // 减去对应的数据
        m_UserData.ulGoldenCoins -= ulCoins;
        setGoldenCoinsCheckCode();
    }
    
    return bRet;
}

bool UserDataManager::isGoldenEnough(unsigned long ulCoins)
{
    bool bRet = false;
    if (m_UserData.ulGoldenCoins >= ulCoins)
    {
        bRet = true;
    }
    return bRet;
}
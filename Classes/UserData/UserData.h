/*
 * =====================================================================================
 *
 *       Filename:  UserData.h
 *
 *    Description:  用户数据类
 *
 *        Version:  1.0
 *        Created:  07/12/2014 14:10:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOON_USER_DATA_H__
#define __BALLOON_USER_DATA_H__

// 气球统计数据
#include "BalloonAnalysis.h"

/**
 * 用户数据结构体
 * */
typedef struct tagUserData
{
	unsigned long ulGoldenCoins;	// 金币
    unsigned long ulDiamond;        // 钻石数量
    // 用户成就数据
    // 用户任务数据
} UserData;

class UserDataManager
{
public:
	UserDataManager();
	virtual ~UserDataManager();

	static UserDataManager* sharedUserDataManager();
    
    // 保存数据到本地库中
    void saveData();
    
    // 从本地库中读取数据
    void loadData();
    
    /**
     * @brief 得到全局统计对象指针，可以外部修改
     */
    BalloonGlobalAnalysis* getGlobalAnalysisDataRef();
    
    /**
     * @brief 得到单局游戏统计对象指针，可以外部修改
     */
    BalloonAnalysis* getAnalysisDataRef();
    
    // 得到用户的金币数量
    unsigned long getGoldenCoins();
    
    // 增加用户的金币数量
    void addGoldenCoins(unsigned long ulCoins);
    
    // 消费金币
    bool subGoldenCoins(unsigned long ulCoins);
    
    // 判断金币是否够支付
    bool isGoldenEnough(unsigned long ulCoins);
private:
    // 校验当前金币值是否合法
    bool verifyGoldenCoins();
    
    // 设置当前金币的校验值
    void setGoldenCoinsCheckCode();
	
private:
	// 用户数据
	UserData m_UserData;
    
    // 全局用户分析数据
    BalloonGlobalAnalysis m_GlobalAnalysisData;
    
    // 当前用户游戏的分析数据
    BalloonAnalysis m_AnalysisData;
};

#endif // __BALLOON_USER_DATA_H__

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

enum ItemExType
{
//    kCCItemExTypeNone = 0,
    kCCItemExTypePump = 0,
    kCCItemExTypePreTime,
    kCCItemExTypeTimeAdd,   // 按下时间道具后添加时间
    kCCItemExTypeGiant,     // 巨人气球分数乘以2
};

typedef struct tagItemExtend
{
    long lPump;     // 加强版的打气筒
    long lPreTime;  // 预置时间
    long lTimeAdd;
    long lGiant;
} ItemExtend;
/**
 * 用户数据结构体
 * */
typedef struct tagUserData
{
	long long llGoldenCoins;	// 金币
    long long llGameCounts;     // 游戏盘数
    ItemExtend itemEx;
    // long long llDiamond;        // 钻石数量
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
    long long getGoldenCoins();
    
    // 增加用户的金币数量
    void addGoldenCoins(long long llCoins);
    
    // 消费金币
    bool subGoldenCoins(long long llCoins);
    
    // 判断金币是否够支付
    bool isGoldenEnough(long long llCoins);
    
    // 获得游戏盘数
    long long getGameCounts();
    
    // 增加一盘游戏数据
    void addOneGameCount();
    
    /*
    // 得到打气筒数目
    long getItemExPumpCounts();
    
    void addItemExPumpCount(long lCnts);
    // 增加一个打气筒
    void addOneItemExPump();
    
    // 得到打气筒数目
    long getItemExPreTimeCounts();
    
    void addItemExPreTimeCount(long lCnts);
    // 增加一个打气筒
    void addOneItemExPreTime();
    //*/
    
    // 根据道具的类型得到道具的保有量
    long getItemExCountsByID(ItemExType type);
    
    long addItemExCountsByID(ItemExType eType, long lValue);
    
    long addItemExOneCountByID(ItemExType type);
    
private:
    // 校验当前金币值是否合法
    bool verifyGoldenCoins();
    
    // 设置当前金币的校验值
    void setGoldenCoinsCheckCode();
    
    // 校验游戏盘数值是否合法
    bool verifyGameCounts();
    
    // 设置游戏盘数验证码
    void setGameCountsCheckCode();
    
    /*
    // 校验当前打气筒
    bool verifyItemExPump();
    
    // 设置当前打气筒
    void setItemExPumpCheckCode();
    
    // 校验当前游戏总时间
    bool verifyItemExPreTime();
    
    // 设置当前游戏总时间
    void setItemExPreTimeCheckCode();
    
    // 校验当前游戏总时间
    bool verifyItemExTimeAdd();
    
    // 设置当前游戏总时间
    void setItemExTimeAddCheckCode();
    //*/
    
    // 校验对应额外应用道具的数值是否合法
    bool verifyItemExByID(ItemExType eType);
    
    // 设置对应额外道具数值的最新校验值
    void setItemExCheckCodeByID(ItemExType eType);
    
private:
	// 用户数据
	UserData m_UserData;
    
    // 全局用户分析数据
    BalloonGlobalAnalysis m_GlobalAnalysisData;
    
    // 当前用户游戏的分析数据
    BalloonAnalysis m_AnalysisData;
};

#endif // __BALLOON_USER_DATA_H__

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
    kCCItemExTypeGiant,     // 巨人气球点击一次直接获得1分
    kCCItemExTypeSafeGuard, // 返还一半的分数
};

// 一次性道具
typedef struct tagItemExtend
{
    long lPump;     // 加强版的打气筒
    long lPreTime;  // 预置时间
    long lTimeAdd;
    long lGiant;
    long lSafeGuard;
} ItemExtend;

// 用户可以升级的能力对应的等级
typedef struct tagUserAbilityLevel
{
    unsigned char ucNormalRange;    // 普通气球的分数随机上限等级
    unsigned char ucBoomGuard;      // 拆弹小队等级
    unsigned char ucTimeSlash;      // 时间老人恩赐，时间暴击概率
    unsigned char ucPumpCount;      // 气球可点击的次数上下限次数
    unsigned char ucMultiCritical;  // 乘分气球暴击概率
    unsigned char ucReverse;        // 点击逆转只影响负分转正分的概率
    unsigned char ucGiantMulti;     // 巨人气球点爆后50%概率得到多倍分数的等级
} UserAbilityLevel;

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

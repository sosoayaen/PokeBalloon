/*
 * =====================================================================================
 *
 *       Filename:  BalloonMission.h
 *
 *    Description:  任务类头文件
 *
 *        Version:  1.0
 *        Created:  07/22/2014 21:48:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOONTASK_H__
#define __BALLOONTASK_H__

#include "BalloonAnalysis.h"
#include <string>
#include <vector>

// 任务数据结构体，用来比对是否完成任务
typedef struct tagMissionData
{
    long long score;    // 游戏的分数
    BalloonAnalysisData analysisData;
} MissionData;

// 任务奖励枚举类（目前最多255个）
enum MissionRewardType
{
    /* 金币 */
    kCCMissionRewardTypeGoldenCoins = 1,
    /* 钻石 */
    kCCMissionRewardTypeDiamond,
    /* 一次性物品奖励，额外打气筒 */
    kCCMissionRewardTypeExItemPump,
    /* 一次性物品奖励，总额外时间 */
    kCCMissionRewardTypeExItemPreTime,
    /* 一次性物品奖励，增强时间 */
    kCCMissionRewardTypeExItemExTime,
    /* 一次性物品奖励，巨人收割 */
    kCCMissionRewardTypeExItemGiant,
    /* 一次性物品奖励，炸弹保险 */
    kCCMissionRewardTypeExItemSafeGuard,
};

// 任务结构体
class Mission
{
public:
	Mission()
	{
		nMissionID = 0;
		strMissionName = "";
		strMissionDesc = "";
        cbRewardType = kCCMissionRewardTypeGoldenCoins;
        nReward = 0;
		memset(&data, 0, sizeof(MissionData));
	}

	unsigned int nMissionID;	// 任务ID
    unsigned char cbRewardType; // 奖励的类型
    int nReward;                // 奖励个数
	std::string strMissionName;	// 任务名称
	std::string strMissionDesc;	// 任务描述
	MissionData data;	// 气球分析数据
    
public:
	/**
	 * @brief 判断当前任务是否完成
	 * @param data 完成的一局游戏的统计数据
	 */
	bool isMissionComplete(const MissionData& data) const;

protected:

};

class BalloonMission
{
public:
	BalloonMission();
	virtual ~BalloonMission();

	// 全局静态对象
	static BalloonMission* sharedBalloonMission();

	// 判断是否需要一个新的任务
	bool isNeedNewMission() const
	{
		return m_bNeedNewMission;
	}

	// 设置新任务模式，当一个任务完成后，一般会设置这个标志
	// 表示下一次获取随机任务的时候来一个新的
	void setNeedNewMission(bool bEnable)
	{
		m_bNeedNewMission = bEnable;
	}

	// 得到一个随机任务
	const Mission* getRandomMission();

	// 载入配置文件中的任务数据 
	bool loadAllMissionData();

protected:
	// 当前随机任务是否过期标志
	bool m_bNeedNewMission;

private:
	// 任务数据数组
	std::vector<Mission*> m_vMissionArray;

	// 上次生成的任务
	Mission* m_pLastMission;

};

#endif // _N_BALLOONTASK_H__

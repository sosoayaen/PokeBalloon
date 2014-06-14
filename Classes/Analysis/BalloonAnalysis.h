/*
 * =====================================================================================
 *
 *       Filename:  BalloonAnalysis.h
 *
 *    Description:  气球数据统计类
 *
 *        Version:  1.0
 *        Created:  06/13/2014 00:38:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "Balloon.h"

// 颜色气球统计数据
struct tagBalloonColorAnalysisData
{
	long long green;
	long long black;
	long long orange;
	long long red;
	long long blue;
	long long yellow;
	long long brown;
	long long pink;
};

// 普通气球统计
struct tagBalloonNormalAnalysisData
{
	long long normal_positive;	// 正数
	long long normal_negative;	// 负数
	long long normal_zero;		// 零
};

// 道具气球统计
struct tagBalloonItemAnalysisData
{
	long long boom;          // 炸弹
	long long time;          // 增加时间
	long long pump;          // 打气筒
	long long multi;         // 乘倍
	long long reverse;       // 正负逆转
	long long reverse_ex;    // 负数逆转
	long long giant;         // 巨人气球
	long long frozen;        // 冰冻气球
};

// 结果数据存储结构体
typedef struct tagBalloonAnalysisData
{
	long long                           total;       // 总气球破坏数量
	struct tagBalloonColorAnalysisData  colorData;   // 颜色统计 
	struct tagBalloonNormalAnalysisData normalData;  // 普通气球个数统计
	struct tagBalloonItemAnalysisData   itemData;    // 道具气球个数统计
		
} BalloonAnalysisData;

class BalloonAnalysis
{
public:
	BalloonAnalysis(){}
	~BalloonAnalysis(){}

	/**
	 * @brief 初始化数据
	 */
	void initData();

	/**
	 * @brief 添加一个气球，加入统计数据中
	 * @param pBalloon 对应的气球对象
	 */
	void countWithBalloonObject(Balloon* pBalloon);

	/**
	 * @brief 获取统计分析数据
	 * @return BalloonAnalysisData&
	 */
	const BalloonAnalysisData& getAnalysisData() const;
    
    /**
     * @brief 得到点破气球总数
     * @return long long
     */
    long long getTotalBalloonCounts() const;
    
    /**
     * @brief 得到点破道具气球总数
     * @return long long
     */
    long long getItemBalloonCounts() const;
    
    /**
     * @brief 得到点破普通气球总数
     * @return long long
     */
    long long getNormalBalloonCounts() const;
    
	/**
	 * @brief 输出统计信息
	 */
	const char* dumpDebugInfo() const;

protected:

	/**
	 * @brief 处理颜色
	 */
	void handleColorType(Balloon* pBalloon);

	/**
	 * @brief 处理普通气球得分
	 */
	void handleNormalData(Balloon* pBalloon);

	/**
	 * @brief 处理道具气球
	 */
	void handleItemData(Balloon* pBalloon);

	/**
	 * @brief 处理颜色之外的数据
	 */
	void handleBalloonData(Balloon* pBalloon);

private:
	// 分析结构体
	BalloonAnalysisData m_AnalysisData;

};

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
#ifndef __BALLOON_ANALYSIS_H__
#define __BALLOON_ANALYSIS_H__
#include "Balloon.h"
#include "cocos2d.h"

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
	virtual ~BalloonAnalysis(){}

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
     * @brief 把统计数据合并，注意，数据结构必须时long long型的
     * @param analysisData
     */
    void merge(const BalloonAnalysis& analysisData);
    
	/**
	 * @brief 输出统计信息
	 */
    std::string dumpDebugInfo() const;

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

protected:
	// 分析结构体
	BalloonAnalysisData m_AnalysisData;

};

/**
 * @class 全局分析数据
 */
class BalloonGlobalAnalysis : public BalloonAnalysis
{
public:
    BalloonGlobalAnalysis(){}
    ~BalloonGlobalAnalysis(){}
    
    // 得到全局静态的记录数据
    static BalloonGlobalAnalysis* sharedGlobalAnalysis();
    
    /**
     * @brief 释放全局记录对象
     */
    void purgeGlobalAnalysis();
    
    /**
     * @brief 从磁盘加载记录数据，一般只有全局数据允许调用此接口
     */
    void loadData();
    
    /**
     * @brief 保存数据到磁盘，一般只允许全局数据调用此接口
     */
    bool saveData();
    
private:
    // 从对象数据转化为字典
    cocos2d::CCDictionary* dictionayFromData();
    
    // 从字典数据填充对象
    void setDataWithDictionary(cocos2d::CCDictionary* pDict);
    
private:
    // 是否明文模式用于存储和读取数据
    bool m_bPlainMode;
};

#endif // __BALLOON_ANALYSIS_H__
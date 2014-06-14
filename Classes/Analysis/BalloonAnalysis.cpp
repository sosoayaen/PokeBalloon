/*
 * =====================================================================================
 *
 *       Filename:  BalloonAnalysis.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/2014 01:09:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonAnalysis.h"

void BalloonAnalysis::initData()
{
	memset(&m_AnalysisData, 0, sizeof(BalloonAnalysisData));
}

void BalloonAnalysis::handleColorType(Balloon* pBalloon)
{
	struct tagBalloonColorAnalysisData& colorData = m_AnalysisData.colorData;
	switch(pBalloon->getBalloonColor())
	{
		case kBalloonColorGreen:
			colorData.green++;
			break;
		case kBalloonColorBlack:
			colorData.black++;
			break;
		case kBalloonColorOrange:
			colorData.orange++;
			break;
		case kBalloonColorRed:
			colorData.red++;
			break;
		case kBalloonColorBlue:
			colorData.blue++;
			break;
		case kBalloonColorYellow:
			colorData.yellow++;
			break;
		case kBalloonColorBrown:
			colorData.brown++;
			break;
		case kBalloonColorPink:
			colorData.pink++;
			break;
		default:
			break;
	}
}

void BalloonAnalysis::handleNormalData(Balloon* pBalloon)
{
	struct tagBalloonNormalAnalysisData& normalData = m_AnalysisData.normalData;
	// 判断对应的正负分数
	if (pBalloon->getBalloonScore() > 0)
		normalData.normal_positive++;
	else if (pBalloon->getBalloonScore() < 0)
		normalData.normal_negative++;
	else
		normalData.normal_zero++;

}

void BalloonAnalysis::handleItemData(Balloon* pBalloon)
{
	struct tagBalloonItemAnalysisData& itemData = m_AnalysisData.itemData;
	// 根据道具类型分配
	switch(pBalloon->getBalloonType())
	{
		case kBalloonTypeMulti:
			itemData.multi++;
			break;          
		case kBalloonTypeBoom:
			itemData.boom++;
			break;           
		case kBalloonTypeAddTime:
			itemData.time++;
			break;        
		case kBalloonTypePump:
			itemData.pump++;
			break;
		case kBalloonTypeAddBalloon:
			break;     
		case kBalloonTypeFrozen:
			itemData.frozen++;
			break;         
		case kBalloonTypeGiant:
			itemData.giant++;
			break;          
		case kBalloonTypeHurricane:
			break;      
		case kBalloonTypeReverse:
			itemData.reverse++;
			break;        	
		default:
			break;
	}
}

void BalloonAnalysis::handleBalloonData(Balloon* pBalloon)
{
	// 先判断是道具还是普通气球
	BalloonType eType = pBalloon->getBalloonType();
	if (eType == kBalloonTypeNormal)
	{
		handleNormalData(pBalloon);
	}
	else
	{
		handleItemData(pBalloon);
	}
}

void BalloonAnalysis::countWithBalloonObject(Balloon* pBalloon)
{
	// 先区分颜色
	handleColorType(pBalloon);

	// 处理普通气球或者道具
	handleBalloonData(pBalloon);

	// 增体请求个数+1
	m_AnalysisData.total++;
}

const BalloonAnalysisData& BalloonAnalysis::getAnalysisData() const
{
	return m_AnalysisData;
}

long long BalloonAnalysis::getTotalBalloonCounts() const
{
    return m_AnalysisData.total;
}

long long BalloonAnalysis::getNormalBalloonCounts() const
{
    const struct tagBalloonNormalAnalysisData normalData = m_AnalysisData.normalData;
    long long llSumData = 0;
    const long long* pData = (long long*)&normalData;
    
    int nLen = sizeof(normalData)/sizeof(long long);
    for (int idx = 0; idx < nLen; ++idx)
    {
        llSumData += *pData;
        pData++;
    }
    return llSumData;
}

long long BalloonAnalysis::getItemBalloonCounts() const
{
    const struct tagBalloonItemAnalysisData itemData = m_AnalysisData.itemData;
    long long llSumData = 0;
    const long long* pData = (long long*)&itemData;
    
    int nLen = sizeof(itemData)/sizeof(long long);
    for (int idx = 0; idx < nLen; ++idx)
    {
        llSumData += *pData;
        pData++;
    }
    return llSumData;
}

const char* BalloonAnalysis::dumpDebugInfo() const
{
	// 输出统计调试信息
	std::stringstream stream;
	stream << "=======>debug info<=======\n";
	// 总点破气球数目
	stream << "total break: " << m_AnalysisData.total << "\n";
    // 各个颜色的统计
    stream << "color red: " << m_AnalysisData.colorData.red << "\n";
    stream << "color brown: " << m_AnalysisData.colorData.brown << "\n";
    stream << "color black: " << m_AnalysisData.colorData.black << "\n";
    stream << "color orange: " << m_AnalysisData.colorData.orange << "\n";
    stream << "color blue: " << m_AnalysisData.colorData.blue << "\n";
    stream << "color yellow: " << m_AnalysisData.colorData.yellow << "\n";
    stream << "color pink: " << m_AnalysisData.colorData.pink << "\n";
    stream << "color green: " << m_AnalysisData.colorData.green << "\n";
    // 普通气球总数
    stream << "+ normal total: " << getNormalBalloonCounts() << "\n";
	// 正分气球点破个数
	stream << "- normal positive: " << m_AnalysisData.normalData.normal_positive << "\n";
	// 负分气球点破个数
	stream << "- normal negative: " << m_AnalysisData.normalData.normal_negative << "\n";
	// 零分气球个数
	stream << "- normal zero: " << m_AnalysisData.normalData.normal_zero << "\n";
    // 道具气球总数
    stream << "+ item total: " << getItemBalloonCounts() << "\n";
    // 乘2气球数目
    stream << "- multi: " << m_AnalysisData.itemData.multi << "\n";
    // 炸弹气球数目
    stream << "- boom: " << m_AnalysisData.itemData.boom << "\n";
    // 翻转气球数目
    stream << "- reverse: " << m_AnalysisData.itemData.reverse << "\n";
    // 巨人气球数目
    stream << "- giant: " << m_AnalysisData.itemData.giant << "\n";
    // 霜冻球数目
    stream << "- frozen: " << m_AnalysisData.itemData.frozen << "\n";
    // 增加时间气球数目
    stream << "- time: " << m_AnalysisData.itemData.time << "\n";

	return stream.str().c_str();
}

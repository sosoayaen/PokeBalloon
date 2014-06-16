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
#include "CCJSONConverter.h"


USING_NS_CC;

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

void BalloonAnalysis::merge(const BalloonAnalysis &analysisData)
{
    const long long* pData = (long long*)&analysisData.getAnalysisData();
    long long* pDestData = (long long*)&m_AnalysisData;
    
    // 得到数据结构内数据个数
    int nLen = sizeof(BalloonAnalysisData)/sizeof(long long);
    while (nLen-- > 0)
    {
        // 逐个累加数据
        *pDestData++ = *pData++;
    }
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

//////////////////////////////////////////////////////////////////////////////////
// 全局数据类实现

#define KEY_ITEM_DATA   "itemData"      // 数据字典中道具数据的key
#define KEY_COLOR_DATA  "colorData"     // 数据字典中颜色数据的key
#define KEY_NORMAL_DATA "normalData"    // 数据字典中普通计分数据的key
#define ARCHIVEMENT_FILE_NAME "archivement" // 存放数据的文件名

BalloonGlobalAnalysis* g_sharedGlobalAnalysis = NULL;

BalloonGlobalAnalysis* BalloonGlobalAnalysis::sharedGlobalAnalysis()
{
    if (!g_sharedGlobalAnalysis)
    {
        g_sharedGlobalAnalysis = new BalloonGlobalAnalysis;
        CCAssert(g_sharedGlobalAnalysis, "g_sharedGlobalAnalysis malloc failed!");
        g_sharedGlobalAnalysis->initData();
        g_sharedGlobalAnalysis->loadData();
    }
    return g_sharedGlobalAnalysis;
}

void BalloonGlobalAnalysis::purgeGlobalAnalysis()
{
    if (g_sharedGlobalAnalysis)
    {
        delete g_sharedGlobalAnalysis;
        g_sharedGlobalAnalysis = NULL;
    }
}

// 加速书写过程的宏定义
#define getColorData(color) m_AnalysisData.colorData.color = atoll(pDictColor->valueForKey(#color)->getCString())
#define getNormalData(data) m_AnalysisData.normalData.data = atoll(pDictNormal->valueForKey(#data)->getCString())
#define getItemData(data) m_AnalysisData.itemData.data = atoll(pDictItem->valueForKey(#data)->getCString())

void BalloonGlobalAnalysis::setDataWithDictionary(cocos2d::CCDictionary *pDict)
{
    // 得到总数
    m_AnalysisData.total = atoll(pDict->valueForKey("total")->getCString());
    
    // 设置颜色数据
    CCDictionary* pDictColor = dynamic_cast<CCDictionary*>(pDict->objectForKey(KEY_COLOR_DATA));
    if (pDictColor)
    {
        // m_AnalysisData.colorData.red = atoll(pDictColor->valueForKey("red")->getCString());
        getColorData(red);
        getColorData(black);
        getColorData(yellow);
        getColorData(green);
        getColorData(pink);
        getColorData(blue);
        getColorData(brown);
        getColorData(orange);
    }
    
    // 设置普通气球数据
    CCDictionary* pDictNormal = dynamic_cast<CCDictionary*>(pDict->objectForKey(KEY_NORMAL_DATA));
    if (pDictNormal)
    {
        getNormalData(normal_positive);
        getNormalData(normal_negative);
        getNormalData(normal_zero);
    }
    
    // 设置道具气球数据
    CCDictionary* pDictItem = dynamic_cast<CCDictionary*>(pDict->objectForKey(KEY_ITEM_DATA));
    if (pDictItem)
    {
        getItemData(boom);
        getItemData(frozen);
        getItemData(giant);
        getItemData(multi);
        getItemData(reverse_ex);
        getItemData(reverse);
        getItemData(pump);
        getItemData(time);
    }
}

void BalloonGlobalAnalysis::loadData()
{
    // 从磁盘中把数据读入，如果没有，则跳过
    std::string strPath = CCFileUtils::sharedFileUtils()->getWritablePath() + ARCHIVEMENT_FILE_NAME;
    if (CCFileUtils::sharedFileUtils()->isFileExist(strPath))
    {
        // 载入数据
        unsigned long nSize = 0;
        const unsigned char* pData = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(), "rb", &nSize);
        if (pData)
        {
            const char* pszJSONData = (const char*)pData;
            CCDictionary* pDict = CCJSONConverter::sharedConverter()->dictionaryFrom(pszJSONData);
            // 转换数据到结构体
            setDataWithDictionary(pDict);
            // 释放数据
            delete pData;
        }
    }
}

// 加速书写过程的宏定义
#define setColorData(color) pDictColor->setObject(CCString::createWithFormat("%lld", m_AnalysisData.colorData.color), #color)
#define setNormalData(data) pDictNormal->setObject(CCString::createWithFormat("%lld", m_AnalysisData.normalData.data), #data)
#define setItemData(data) pDictItem->setObject(CCString::createWithFormat("%lld", m_AnalysisData.itemData.data), #data)

CCDictionary* BalloonGlobalAnalysis::dictionayFromData()
{
    CCDictionary* pDict = CCDictionary::create();
    pDict->removeAllObjects();
    
    // 从数据中得到总数
    pDict->setObject(CCString::createWithFormat("%lld", m_AnalysisData.total), "total");
    
    // 处理颜色数据
    CCDictionary* pDictColor = CCDictionary::create();
    // pDictColor->setObject(CCString::createWithFormat("%lld", colorData.red), "red");
    setColorData(red);
    setColorData(blue);
    setColorData(black);
    setColorData(orange);
    setColorData(pink);
    setColorData(green);
    setColorData(brown);
    setColorData(yellow);
    pDict->setObject(pDictColor, KEY_COLOR_DATA);
    
    // 处理分数气球
    CCDictionary* pDictNormal = CCDictionary::create();
    // pDictNormal->setObject(CCString::createWithFormat("%lld", m_AnalysisData.normalData.normal_positive), "normal_positive");
    setNormalData(normal_positive);
    setNormalData(normal_negative);
    setNormalData(normal_zero);
    pDict->setObject(pDictNormal, KEY_NORMAL_DATA);
    
    // 处理道具气球
    CCDictionary* pDictItem = CCDictionary::create();
    setItemData(pump);
    setItemData(boom);
    setItemData(giant);
    setItemData(multi);
    setItemData(reverse);
    setItemData(reverse_ex);
    setItemData(frozen);
    setItemData(time);
    pDict->setObject(pDictItem, KEY_ITEM_DATA);
    
    return pDict;
}

bool BalloonGlobalAnalysis::saveData()
{
    // 把数据结构转换成CCDictionary对象
    CCDictionary* pDict = dictionayFromData();
    
    if (!pDict) return false;
    // 转换成JSON数据
    std::string strJSON = CCJSONConverter::sharedConverter()->strFrom(pDict);
    // 保存数据到磁盘
    std::string strPath = CCFileUtils::sharedFileUtils()->getWritablePath() + ARCHIVEMENT_FILE_NAME;
    FILE* file = fopen(strPath.c_str(), "wb");
    CCAssert(file, "archivement file open failed...");
    if (file)
    {
        fwrite(strJSON.c_str(), 1, strJSON.length(), file);
        fflush(file);
        fclose(file);
    }
    
    return true;
}

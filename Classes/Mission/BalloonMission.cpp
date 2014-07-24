/*
 * =====================================================================================
 *
 *       Filename:  BalloonMission.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/2014 21:49:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonMission.h"
#include "CCJSONConverter.h"

bool Mission::isMissionComplete(const MissionData& data) const
{
	bool bRet = true;

	// TODO: 为了减少内存消耗，这里可以当判断任务是否完成的时候再把对应的任务数据读取到内存中
	const long long* plSrcData = (const long long*)&this->data;
	const long long* plData = (const long long*)&data;

	// 得到数据的个数，这里要保证数据的类型都是一致的
	unsigned int nSize = sizeof(MissionData)/sizeof(long long);
	
	// 循环判断数值是否达标
	for (unsigned int idx = 0; idx < nSize; ++idx)
	{
		if (*plData++ < *plSrcData++)
		{
			bRet = false;
			break;
		}
	}

	// 设置当前任务完成
	if (bRet)
		BalloonMission::sharedBalloonMission()->setNeedNewMission(true);

	return bRet;
}

BalloonMission::BalloonMission()
{
    // 默认会生成一个新的任务
	m_bNeedNewMission = true;
	m_pLastMission = NULL;
}

BalloonMission::~BalloonMission()
{
	// 清除动态申请的数据
	std::vector<Mission* >::iterator iter = m_vMissionArray.begin();
	for (; iter != m_vMissionArray.end(); ++iter)
	{
		Mission* pData = *iter;
		delete pData;
	}
}

static BalloonMission* g_sBalloonMission = NULL;

BalloonMission* BalloonMission::sharedBalloonMission()
{
	if (g_sBalloonMission == NULL)
	{
		g_sBalloonMission = new BalloonMission;
	}

	return g_sBalloonMission;
}

#define SetMissionColorData(keyName) pMission->data.analysisData.colorData.keyName = ((CCNumber*)pDict->objectForKey(#keyName))->getIntValue();
#define SetMissionNormalData(keyName) pMission->data.analysisData.normalData.keyName = ((CCNumber*)pDict->objectForKey(#keyName))->getIntValue();
#define SetMissionItemData(keyName) pMission->data.analysisData.itemData.keyName = ((CCNumber*)pDict->objectForKey(#keyName))->getIntValue();
bool BalloonMission::loadAllMissionData()
{
	// TODO: 从配置文件中读取对应的任务列表并生成任务结构体，放入队列中
	
	const char* pszJSONData = NULL;
    unsigned long len = 0;
#if COCOS2D_DEBUG > 0
	// Debug直接从文件中读取
    pszJSONData = (const char*)CCFileUtils::sharedFileUtils()->getFileData("configuration/Mission.json", "r", &len);
#else
	// Release版本从压缩包中读取数据
    pszJSONData = (const char*)CCFileUtils::sharedFileUtils()->getFileDataFromZip("config", "Mission.json", &len);
#endif
	
	if (!pszJSONData) return false;

	CCDictionary* pDictData = CCJSONConverter::sharedConverter()->dictionaryFrom(pszJSONData);
    
    // 释放临时创建的文件内存
    delete pszJSONData;
    
    if (!pDictData)
        return false;
    
    CCArray* pArrayData = dynamic_cast<CCArray*>(pDictData->objectForKey("data"));
    
    if (!pArrayData)
        return false;
    
    //*
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(pArrayData, pObj)
    {
        CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
        if (!pDict) continue;
        
        Mission* pMission = new Mission;
        
        // 设置总数
        pMission->nMissionID = ((CCNumber*)pDict->objectForKey("ID"))->getIntValue();
        pMission->strMissionName = pDict->valueForKey("name")->getCString();
        pMission->strMissionDesc = pDict->valueForKey("description")->getCString();
        // 设置颜色气球完成度
        SetMissionColorData(black);
        SetMissionColorData(red);
        SetMissionColorData(blue);
        SetMissionColorData(yellow);
        SetMissionColorData(brown);
        SetMissionColorData(pink);
        SetMissionColorData(orange);
        SetMissionColorData(green);
        // 设置普通得分气球完成度
        SetMissionNormalData(normal_negative);
        SetMissionNormalData(normal_positive);
        SetMissionNormalData(normal_zero);
        // 设置道具气球完成度
        SetMissionItemData(boom);
        SetMissionItemData(time);
        SetMissionItemData(pump);
        SetMissionItemData(multi);
        SetMissionItemData(reverse);
        SetMissionItemData(reverse_ex);
        SetMissionItemData(giant);
        SetMissionItemData(frozen);
        // 把数据塞到列表中
        m_vMissionArray.push_back(pMission);
    }
    
    // */

	return true;
}

const Mission* BalloonMission::getRandomMission()
{
	// 先判断是否需要生成新的任务
	if (m_pLastMission != NULL && !isNeedNewMission())
	{
		return m_pLastMission;
	}

	// 生成新的随机任务
	Mission* pMission = NULL;

	unsigned int nMissionSize = m_vMissionArray.size();

	if (nMissionSize > 0)
	{
		// 取随机一个任务
		pMission = m_vMissionArray[rand()%nMissionSize];
        // 生成一个任务后，完成前就把新任务标志关闭
        setNeedNewMission(false);
	}

	// 设置最后的任务
	m_pLastMission = pMission;

	return pMission;
}

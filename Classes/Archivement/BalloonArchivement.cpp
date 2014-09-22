/*
 * =====================================================================================
 *
 *       Filename:  BalloonArchivement.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/30/2014 22:16:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "BalloonArchivement.h"
#include "cocos2d.h"
#include "bailinUtil.h"
#include "CCJSONConverter.h"

USING_NS_CC;
USING_NS_BAILIN_UTIL;

BalloonArchivement::BalloonArchivement()
{
    m_bLoadData = false;
}

BalloonArchivement::~BalloonArchivement()
{
	ArchDataByIDMap::iterator iter = m_archDataMap.begin();
	ArchDataByIDMap::iterator iterEnd = m_archDataMap.end();
	for (; iter != iterEnd; ++iter)
	{
		// 删除动态创建的数据
		delete iter->second;
	}
}

static BalloonArchivement* g_sSharedBalloonArchivement = NULL;

BalloonArchivement* BalloonArchivement::sharedBalloonArchivement()
{
	if (!g_sSharedBalloonArchivement)
	{
		g_sSharedBalloonArchivement = new BalloonArchivement;
	}
	return g_sSharedBalloonArchivement;
}

bool BalloonArchivement::loadData()
{
	bool bRet = false;

	// 从配置中加载成就数据
    unsigned long size = 0;
    std::string zipFile = CCFileUtils::sharedFileUtils()->fullPathForFilename("config");
    const unsigned char* pszArchData = CCFileUtils::sharedFileUtils()->getFileDataFromZip(zipFile.c_str(), "configuration/arch.json", &size);
    if (size > 0)
    {
        // 把数据读入内存中
        CCDictionary* pDictArch = CCJSONConverter::sharedConverter()->dictionaryFrom((const char*)pszArchData);
        CC_SAFE_DELETE_ARRAY(pszArchData);
        
        if (pDictArch)
        {
            // 填充内部数据
            CCArray* pArray = dynamic_cast<CCArray*>(pDictArch->objectForKey("data"));
            if (pArray)
            {
                CCObject* pObj = NULL;
                CCARRAY_FOREACH(pArray, pObj)
                {
                    CCDictionary* pDict = dynamic_cast<CCDictionary*>(pObj);
                    
                    ArchData* pArchData = new ArchData;
                    // 获得成就ID
                    pArchData->nID = pDict->valueForKey("id")->uintValue();
                    // 获得描述数组
                    CCArray* pArrayTitle = dynamic_cast<CCArray*>(pDict->objectForKey("title"));
                    CCAssert(pArrayTitle, "archivement title array is null");
                    CCArray* pArrayMilestone = dynamic_cast<CCArray*>(pDict->objectForKey("milestone"));
                    CCAssert(pArrayMilestone, "archivement milestone array is null");
                    CCString* pStringDesc = dynamic_cast<CCString*>(pDict->objectForKey("desc"));
                    CCAssert(pStringDesc, "archivement desc data is null");
                    // 大分类
                    pArchData->indexType = pDict->valueForKey("indexType")->intValue();
                    // 下标位置
                    pArchData->indexId = pDict->valueForKey("indexId")->intValue();
                    // 里程碑个数
                    pArchData->nSize = pArrayMilestone->count();
                    CCObject* pObjDetail = NULL;
                    // 得到成就名称
                    CCARRAY_FOREACH(pArrayTitle, pObjDetail)
                    {
                        CCString* pString = dynamic_cast<CCString*>(pObjDetail);
                        pArchData->titleArray.push_back(pString->m_sString);
                    }
                    // 填上里程碑和对应的描述
                    CCARRAY_FOREACH(pArrayMilestone, pObjDetail)
                    {
                        CCString* pString = dynamic_cast<CCString*>(pObjDetail);
                        long long milestone = atoll(pString->getCString());
                        pArchData->milestoteArray.push_back(milestone);
                        pArchData->descArray.push_back(CCString::createWithFormat(pStringDesc->getCString(), milestone)->m_sString);
                    }
                    m_archDataMap[pArchData->nID] = pArchData;
                }
                // 数据加载完成标志
                m_bLoadData = true;
            }
            //
        }
    }
    CC_SAFE_DELETE_ARRAY(pszArchData);

	return bRet;
}

const ArchData* BalloonArchivement::getArchDataByID(long nID) const
{
	const ArchData* pRet = NULL;
	ArchDataByIDMap::const_iterator iter = m_archDataMap.find(nID);
	if (iter != m_archDataMap.end())
	{
		pRet = iter->second;
	}

	return pRet;
}

const ArchDataByIDMap& BalloonArchivement::getArchDataMap() const
{
    return m_archDataMap;
}
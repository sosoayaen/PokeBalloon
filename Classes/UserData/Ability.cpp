/*
 * =====================================================================================
 *
 *       Filename:  Ability.cpp
 *
 *    Description:  用户技能类实现文件
 *
 *        Version:  1.0
 *        Created:  07/31/2014 23:52:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "Ability.h"
#include "cocos2d.h"

USING_NS_CC;

AbilityManager::AbilityManager()
{

}

AbilityManager::~AbilityManager()
{
	// 清空数据表
	AbilityDescMap::iterator iterMap = m_mapAbilityDesc.begin();
	for (; iterMap != m_mapAbilityDesc.end(); ++iterMap)
	{
		// 得到其中的数组对象
		AbilityDescArray& vArray = iterMap->second;
		// 循环迭代释放堆上的数据
		AbilityDescArray::iterator iterDesc = vArray.begin();
		for (; iterDesc != vArray.end(); ++iterDesc)
		{
			// 释放对象
			delete *iterDesc;
		}
	}
}

static AbilityManager* g_sharedAbilityManager = NULL;

AbilityManager* AbilityManager::sharedAbilityManager()
{
	if (!g_sharedAbilityManager)
	{
		g_sharedAbilityManager = new AbilityManager;
        
        g_sharedAbilityManager->loadData();
	}

	return g_sharedAbilityManager;
} 

#define SetAbilityDescString(key) pAbilityDesc->key = pDictDesc->valueForKey(#key)->getCString();
#define SetAbilityDescInt(key) pAbilityDesc->key = pDictDesc->valueForKey(#key)->intValue();
bool AbilityManager::loadData()
{
	bool bRet = false;

	// 从配置文件中读取所有的技能数据，并且生成的内存数据
    CCDictionary* pData = CCDictionary::createWithContentsOfFile("configuration/ability.plist");
    if (pData)
    {
        CCArray* pArrayAbilities = dynamic_cast<CCArray*>(pData->objectForKey("abilityArray"));
        if (pArrayAbilities)
        {
            CCObject* pObjDictAbility = NULL;
            CCARRAY_FOREACH(pArrayAbilities, pObjDictAbility)
            {
                CCDictionary* pDictAbility = (CCDictionary*)pObjDictAbility;
                // 得到对应的类型
                int nID = pDictAbility->valueForKey("nID")->intValue();
                // 创建描述数组
                AbilityDescArray vAbilities;
                CCArray* pArrayAbilityDesc = (CCArray*)pDictAbility->objectForKey("descArray");
                CCObject* pObjDictDesc = NULL;
                CCARRAY_FOREACH(pArrayAbilityDesc, pObjDictDesc)
                {
                    CCDictionary* pDictDesc = (CCDictionary*)pObjDictDesc;
                    // 得到内部对应的等级的结构体
                    AbilityDesc* pAbilityDesc = new AbilityDesc;
                    pAbilityDesc->nID = nID;
                    
                    // 得到对应的标题，描述，耗费等
                    SetAbilityDescInt(nCost);
                    SetAbilityDescInt(nLvl);
                    
                    // TODO: 这里的数据都可以从语言文件中实时获取，这里仅存配置数据
                    SetAbilityDescString(strTitle);
                    SetAbilityDescString(strDescription);
                    
                    // 添加到数组中
                    vAbilities.push_back(pAbilityDesc);
                }
                // 把数据设置到哈希表中
                m_mapAbilityDesc[nID] = vAbilities;
            }
        }
    }

	return bRet;
}

const AbilityDesc* AbilityManager::getAbilityDescByLvl(int nID, int nLvl) const
{
	const AbilityDesc* pRet = NULL;

	const AbilityDescArray* pArray = getAbilityDescArray(nID);
	if (pArray && pArray->size() >= nLvl)
	{
		// 根据级别获取对应的结构体
		pRet = pArray->at(nLvl);
	}

	return pRet;
}

const AbilityDescArray* AbilityManager::getAbilityDescArray(int nID) const
{
	const AbilityDescArray* pRet = NULL;
	
	AbilityDescMap::const_iterator iterFind = m_mapAbilityDesc.find(nID);
	if (iterFind != m_mapAbilityDesc.end())
	{
		// 直接指向真实数据地址
		pRet = &(iterFind->second);
	}

	return pRet;
}

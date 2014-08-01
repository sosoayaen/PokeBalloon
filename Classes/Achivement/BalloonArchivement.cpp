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

BalloonArchivement::BalloonArchivement()
{

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

	return bRet;
}

const ArchData* BalloonArchivement::getArchDataByID(long nID)
{
	const ArchData* pRet = NULL;
	ArchDataByIDMap::const_iterator iter = m_archDataMap.find(nID);
	if (iter != m_archDataMap.end())
	{
		pRet = iter->second;
	}

	return pRet;
}

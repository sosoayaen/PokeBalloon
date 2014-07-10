/*
 * =====================================================================================
 *
 *       Filename:  Cloud.cpp
 *
 *    Description:  气球实现类
 *
 *        Version:  1.0
 *        Created:  05/09/2014 23:37:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "Cloud.h"

USING_NS_CC;

Cloud::Cloud() : m_fSpeedX(0), m_fSpeedY(0)
{

}

Cloud::~Cloud()
{

}

Cloud* Cloud::create(const char* pszFileName)
{
	Cloud* pobCloud = new Cloud();
	if (pobCloud && pobCloud->initWithFile(pszFileName))
	{
		pobCloud->autorelease();
		return pobCloud;
	}
	CC_SAFE_DELETE(pobCloud);
	
	return NULL;
}

Cloud* Cloud::create(const char* pszFileName, const CCRect& rect)
{
	Cloud* pobCloud = new Cloud();
	if (pobCloud && pobCloud->initWithFile(pszFileName, rect))
	{
		pobCloud->autorelease();
		return pobCloud;
	}
	CC_SAFE_DELETE(pobCloud);
	
	return NULL;
}

Cloud* Cloud::createWithSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame)
{
	Cloud* pobCloud = new Cloud();
	if (pSpriteFrame && pobCloud && pobCloud->initWithSpriteFrame(pSpriteFrame))
	{
		pobCloud->autorelease();
		return pobCloud;
	}
	CC_SAFE_DELETE(pobCloud);
	
	return NULL;
}

Cloud* Cloud::createWithSpriteFrameName(const char* pszSpriteFrameName)
{
	CCSpriteFrame* pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);

#if COCOS2D_DEBUG > 0
	char msg[256] = {0};
	sprintf(msg, "Invalid spriteFrameName: %s", pszSpriteFrameName);
	CCAssert(pFrame != NULL, msg);
#endif	

	return createWithSpriteFrame(pFrame);
}

void Cloud::updatePosition()
{
    // 根据帧数调整位置
    float fPosX = m_fSpeedX + getPositionX();
    float fPosY = m_fSpeedY + getPositionY();
    setPosition(ccp(fPosX, fPosY));
}

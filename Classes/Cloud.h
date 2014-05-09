/*
 * =====================================================================================
 *
 *       Filename:  Cloud.h
 *
 *    Description:  云朵对象
 *
 *        Version:  1.0
 *        Created:  05/09/2014 23:35:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "cocos2d.h"

class Cloud : public cocos2d::CCSprite
{
	// 云的飞行速度，分X和Y两个方向
	CC_SYNTHESIZE(float, m_fSpeedX, SpeedX);
	CC_SYNTHESIZE(float, m_fSpeedY, SpeedY);

public:
	Cloud();
	~Cloud();

	// 创建函数
	static Cloud* create(const char* pszFileName);
    
	static Cloud* create(const char* pszFileName, const cocos2d::CCRect& rect);

	static Cloud* createWithSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame);

	static Cloud* createWithSpriteFrameName(const char* pszSpriteFrameName);

	/**
	 * @brief 更新云朵的位置
	 */
	void updatePosition();

protected:

private:

};

#endif // __CLOUD_H__

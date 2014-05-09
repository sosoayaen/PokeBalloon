/*
 * =====================================================================================
 *
 *       Filename:  CloudManager.h
 *
 *    Description:  云朵的管理类，负责生成云朵，添加到对应的层中，并且设定大小、速度等
 *
 *        Version:  1.0
 *        Created:  05/10/2014 00:02:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __CLOUD_MANAGER_H__
#define __CLOUD_MANAGER_H__

#include "cocos2d.h"

class CloudManager
{
    // 管理对象是否初始化的标志
    CC_PROPERTY_READONLY(bool, m_bInited, InitFlag);

public:
	CloudManager();
	~CloudManager();

	/**
	 * @brief 初始化云朵管理对象，设置放置云朵的层
	 */
	void init(cocos2d::CCLayer* pLayerCouldContainer);

	/**
	 * @brief 更新云朵的位置
	 */
	void updatePosition();

	/**
	 * @brief 随机在层上增加一个云朵	
	 */
	void addRandomCloud();

protected:
	// 放置云朵的层
	cocos2d::CCLayer* m_pLayerCloudContainer;

private:

	cocos2d::CCSize m_sizeScreen;
};

#endif // __CLOUD_MANAGER_H__

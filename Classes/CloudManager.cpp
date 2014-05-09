/*
 * =====================================================================================
 *
 *       Filename:  CloudManager.cpp
 *
 *    Description:  云管理类的实现文件
 *
 *        Version:  1.0
 *        Created:  05/10/2014 00:10:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "CloudManager.h"
#include "Cloud.h"

USING_NS_CC;

CloudManager::CloudManager() :
	m_pLayerCloudContainer(NULL),
	m_bInited(false)
{

}

CloudManager::~CloudManager()
{

}

void CloudManager::init(CCLayer* pLayerCouldContainer)
{
	m_pLayerCloudContainer = pLayerCouldContainer;

	m_sizeScreen = CCDirector::sharedDirector()->getWinSize();

	m_bInited = true;
}

bool CloudManager::getInitFlag()
{
	return m_bInited;
}

void CloudManager::updatePosition()
{
    CCObject* pObj = NULL;
    CCARRAY_FOREACH_REVERSE(m_pLayerCloudContainer->getChildren(), pObj)
    {
        Cloud* pCloud = dynamic_cast<Cloud*>(pObj);
        
        if (!pCloud) continue;
        
		pCloud->updatePosition();

		// 判断如果云移出屏幕择直接消失
		if (pCloud->getPositionY() > m_sizeScreen.height + pCloud->getContentSize().height ||
				pCloud->getPositionX() > m_sizeScreen.width + pCloud->getContentSize().width)
		{
			// 从层中移除对象
			if (pCloud->getParent())
			{
				pCloud->removeFromParent();
			}
		}
	}
}

void CloudManager::addRandomCloud()
{
	if (!m_bInited) return;

	// 随机得到云朵的ID
	CCString* pStrCloudTextureName = CCString::createWithFormat("cloud/cloud_%d.png", rand()%8+1);

	// 生成云朵对象
	Cloud* pCloud = Cloud::create(pStrCloudTextureName->getCString());

	// 云朵的大小
    float fScale = (rand()%15 + 10) / 10.0f;

	pCloud->setScale(fScale);

	// 云朵的起始位置为屏幕左侧，这个和云朵本身的大小有关
	float fStartX = pCloud->boundingBox().size.width * -0.5f;

	// 设定云朵出现的位置为屏幕30%以上的区域
	float fStartY = m_sizeScreen.height * (rand()%700 + 300)/1000.0f;

	pCloud->setPosition(ccp(fStartX, fStartY));

	// 设定云朵的Y轴移动速度
	pCloud->setSpeedX(rand()%5 + 1);

	// 添加云朵
	m_pLayerCloudContainer->addChild(pCloud);

}

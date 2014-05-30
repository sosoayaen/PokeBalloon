/*
 * =====================================================================================
 *
 *       Filename:  BalloonItemManager.h
 *
 *    Description:  气球道具的管理类，也可以看成是一个工厂类
 *
 *        Version:  1.0
 *        Created:  05/23/2014 22:50:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOON_ITEM_MANAGER_H__
#define __BALLOON_ITEM_MANAGER_H__

// 道具类
#include "BalloonItem.h"

class BalloonItemManager
{
public:
	BalloonItemManager();
	~BalloonItemManager();
	/**
	 * @brief 初始化
	 * @param 
	 */
	bool init(BalloonItemDelegate* pDelegate, cocos2d::CCLayer* pLayerItemContainer);

	// 根据道具的类型，生成对应的道具对象
	// 如果成功，则会自动添加到场景中的道具层上
	// bool productItemWithItemType(BalloonItemType eType);

	/**
	 * @brief 在最后面添加一个道具
	 * @param pBalloonItem 外面创建好的道具对象
	 */
	bool appendBalloonItem(BalloonItem* pBalloonItem);

	bool appendBalloonItemWithItemId(BalloonItemId eID, const char* pszTextureFile, unsigned long nCnts);
    
	bool appendBalloonItemWithItemId(BalloonItemId eID, cocos2d::CCSpriteFrame* pSpriteFrame, unsigned long nCnts);

	// 更新道具的位置，有可能道具持续效果消失，道具会被重新排列位置
	// 一般每一帧都要调用，以确保位置的及时更新
	void updatePosition(float dt, unsigned long ulFrame);

    // 初始化标志
	bool getInitFlag()
	{
		return m_bInited;
	}
    
    // 清理道具层
    void clearItems()
    {
        if (m_pLayerItemContainer)
        {
            m_pLayerItemContainer->removeAllChildren();
        }
    }

	// 排列内部元素
	void alignItems();
protected:

private:
	bool m_bInited;
	// 放置道具的层容器
	cocos2d::CCLayer* m_pLayerItemContainer;
	// 外部回调类
	BalloonItemDelegate* m_pDelegate;
};

#endif // __BALLOON_ITEM_MANAGER_H__

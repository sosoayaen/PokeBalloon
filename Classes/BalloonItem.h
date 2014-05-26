/*
 * =====================================================================================
 *
 *       Filename:  BalloonItem.h
 *
 *    Description:  气球中的道具对象的基类
 *
 *        Version:  1.0
 *        Created:  05/22/2014 23:13:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __BALLOON_ITEM_H__
#define __BALLOON_ITEM_H__

#include "cocos2d.h"
#include "Balloon_macro.h"

// 定义一个道具的枚举列表，所有的道具都需要通过BalloonItemManager生成
enum BalloonItemId
{
	kBalloonItemId_Pumps = 1,	// 气筒，点击增加普通气球的分数
	kBalloonItemId_AddMore,	// 点击增加气球个数
};

enum BalloonItemType
{
	kBalloonItemType_Click = 1, // 点击型道具，有点击次数期限
	kBalloonItemType_Durative,	// 持续性道具，有时间期限
};

// 预先定义
class BalloonItem;

// 道具响应监听代理类
class BalloonItemDelegate
{
public:
	// 道具起效通知，一般为按钮按下后的效果
	virtual void onBalloonItemEffectTrigger(BalloonItem* pItem) = 0;

	// 道具消失通知
	virtual void onBalloonItemDisappear(BalloonItem* pItem) = 0;
};

/**
 * @class 此类用作气球中的道具在界面的显示，用作显示效果的持续时间
 *
 */
class BalloonItem : public cocos2d::CCNode
{
	// 道具类型
	CC_SYNTHESIZE(BalloonItemType, m_eType, ItemType);
	// 道具ID
	CC_SYNTHESIZE(BalloonItemId, m_eItemId, ItemId);
	// 监听回调对象
	CC_SYNTHESIZE(BalloonItemDelegate*, m_pBalloonItemDelegate, BalloonItemDelegate);
	
public:

	/*
	CREATE_FUNC(BalloonItem);

	static BalloonItem* create(BalloonItemType eType, BalloonItemId eId, cocos2d::CCSpriteFrame* pSpriteFrame, );
	*/

    BalloonItem():m_pBalloonItemDelegate(NULL){}
	/**
	 * @brief 根据帧数更新道具状态
	 * @param ulFrame 当前主场景帧数
	 */
	virtual void updateItemStatus(float dt, unsigned long ulFrame) = 0;

	/**
	 * @brief 暂停道具动画
	 *
	 */
	virtual void pause() = 0;

	// 恢复道具动画
	virtual void resume() = 0;
};

/**
 * @class 持续性道具类
 */
class BalloonItemDurative : public BalloonItem
{
	// 道具在屏幕出现的起始帧
	CC_SYNTHESIZE(unsigned long, m_ulStartFrame, StartFrame);
	// 道具当前帧
	CC_SYNTHESIZE(unsigned long, m_ulCurrentFrame, CurrentFrame);
	// 道具消失帧
	CC_SYNTHESIZE(unsigned long, m_ulDisappearFrame, DisappearFrame);
	// 道具持续时间
	CC_SYNTHESIZE(float, m_fItemDuration, ItemDuration);

public:
	// 构造函数
	BalloonItemDurative();

	virtual void updateItemStatus(float dt, unsigned long ulFrame);

	virtual void pause();

	virtual void resume();
};

/**
 * @class 触摸型道具类
 */
class BalloonItemClick : public BalloonItem
{
	// 可点击的次数
	CC_SYNTHESIZE(unsigned long, m_ulClickCounts, ClickCounts);
public:
	BalloonItemClick();
	~BalloonItemClick();

	/**
	 * @brief 根据按钮创建道具
	 * @param pSpriteFrame 按钮精灵帧
	 * @param ulCnts 道具可按下的次数
	 */
	// static BalloonItemClick* create(cocos2d::CCSpriteFrame* pSpriteFrame, unsigned long ulCnts);

	/**
	 * @brief 根据按钮创建道具
	 * @param pDelegate 外部回调对象
	 * @param pszTextureFile 按钮背景图片路径
	 * @param ulCnts 道具可按下的次数
	 */
	static BalloonItemClick* create(BalloonItemDelegate* pDelegate, const char* pszTextureFile, unsigned long ulCnts);

	virtual void updateItemStatus(float dt, unsigned long ulFrame);

	virtual void pause();

	virtual void resume();

public:
    void initClickMenu(cocos2d::CCSprite* pSpriteIcon);

protected:
	// 按下按钮的回调函数
	void onPressMenuClick(cocos2d::CCObject* pSender);

protected:
	// 按钮
	cocos2d::CCMenu* m_pMenuClick;

    // 按钮上的图片
    cocos2d::CCSprite* m_pSpriteIcon;
	
};

#endif // __BALLOON_ITEM_H__

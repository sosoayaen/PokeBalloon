/*
 * =====================================================================================
 *
 *       Filename:  BalloonManager.h
 *
 *    Description:  气球管理类，负责整个场景中所有气球的添加删除更新等操作
 *
 *        Version:  1.0
 *        Created:  2014/04/20 10时30分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __BALLOON_MANAGER_H__
#define __BALLOON_MANAGER_H__

#include "Balloon.h"

// 定义气球数组
typedef std::list<Balloon* > BalloonObjectList;

/**
 * @class BalloonDelegate类，主要负责气球的一些状态的回调
 *
 */
class BalloonDelegate
{
public:
	/**
	 * @brief 气球触碰检测成功回调
	 */
	virtual void balloonHitTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite) = 0;

	/**
	 * @brief 气球触摸检测成功回调
	 */
	virtual void balloonTouchTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite) = 0;
};

class BalloonManager
{
	// 实现回调外部对象指针
	CC_SYNTHESIZE(BalloonDelegate*, m_pBalloonDelegate, BalloonDelegate);

	// 装载气球的层，外部设置，在场景中
	CC_SYNTHESIZE(cocos2d::CCLayer*, m_pLayerBalloonContainer, BalloonLayerContainer);
    
    // 管理对象是否初始化的标志
    CC_PROPERTY_READONLY(bool, m_bInited, InitFlag);

public:
	BalloonManager();
	~BalloonManager();

	/**
	 * @brief 初始化管理类
	 * @param pDelegate 气球回调操作对象
	 * @param pLayerBalloonContainer 用来放置气球精灵的层
	 *
	 */
	void init(BalloonDelegate* pDelegate, cocos2d::CCLayer* pLayerBalloonContainer);
    
	/**
	 * @brief 更新气球的位置，会循环调用场景中的所有气球对象
	 */
	void updatePosition();

	/**
	 * @brief 碰撞检测，逆向迭代，要求气球数组尾部增加新对象，使后增加的气球层级较高
	 * @param rect 待检测的矩形
	 * @param pSprite 此参数默认留空，是用作hitTest碰撞检测是哪个对象，限定为CCSprite对象
	 *                主要BalloonDelegate操作对象的hitTest相关回调函数只有一个，需要知道
	 *                是哪个物体被碰撞了
	 *                需要注意对应对象是否忽略了m_bIgnoreAnchorPointForPosition
	 * @return bool 返回true表示碰撞检测成功，否则碰撞检测失败
	 *				这里的返回值作用不是特别大，因为碰撞检测成功或者失败后会有对应的回调
	 */
	bool hitTest(const cocos2d::CCRect& rect, cocos2d::CCSprite* pSprite = NULL); 

	/**
	 * @brief 触摸检测，逆向迭代
	 * @param point 世界坐标系下的触摸点
	 * @param pSprite 参数默认为空，是用作回调通知是哪个气球对象被点到了
	 * @return bool 返回true标识点击成功，否则没有气球被点到
	 */
	bool touchTest(const cocos2d::CCPoint point, cocos2d::CCSprite* pSprite = NULL);

	/**
	 * @brief 在屏幕上X轴随机增加一个随机气球
	 */
	void addRandomBalloon();
    
    /**
     * @brief 当前屏幕上所有普通气球分数乘以一个数
     * @param lValue 对应的乘数
     */
    void multipleBalloonScore(long nValue);

protected:
	// protected functions
	

protected:
	// 对应气球对象数组
	// BalloonObjectList m_BalloonObjectList;

	// 游戏屏幕大小
	cocos2d::CCSize m_sizeScreen;
    
};

#endif // __BALLOON_MANAGER_H__

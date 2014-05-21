/**
 * 气球对象
 * 设置气球的位置、加速度等等
 * 主要负责气球的位置更新，通常，位置更新由气球对象的管理类负责
 */

#ifndef __PokeBalloon__Balloon__
#define __PokeBalloon__Balloon__

#include "cocos2d.h"
#include <string>

#include "Balloon_macro.h"

enum BalloonType
{
	kBalloonTypeNormal = 0,         // 普通气球，分数即为点击后的得分，持续时间无效
    kBalloonTypeMulti,              // 乘分气球，分数即为乘数，持续时间无效
    kBalloonTypeDiv,                // 除分气球，分数为被除数，持续时间无效
    kBalloonTypeAddTime,            // 增加时间的球，分数为增加的时间，单位为秒，持续时间无效
    kBalloonTypeAddBalloonScore,    // 给全屏的气球加分，给全屏增加的分值，持续时间为秒
    kBalloonTypeAddBalloon,         // 增加屏幕气球个数，分数为给全屏一次性增加的气球个数，持续时间为秒
    kBalloonTypeFrozen,             // 冻结全屏，分数无效，持续时间为冻结的秒数
    kBalloonTypeGiant,              // 巨人气球，分数为点爆需要的次数，持续时间无效
    kBalloonTypeHurricane,          // 飓风气球，分数无效，直接影响界面云突然变多
    kBalloonTypeReverse,            // 正负号转换，分数和持续时间无效
};

class Balloon : public cocos2d::CCSprite
{
	// 气球的类型
	CC_SYNTHESIZE(BalloonType, m_eType, BalloonType);
	// 气球的飞行速度，分X和Y两个方向
	CC_SYNTHESIZE(float, m_fSpeedX, SpeedX);
	CC_SYNTHESIZE(float, m_fSpeedY, SpeedY);
	// 气球的加速度，分为X和Y两个方向
	CC_SYNTHESIZE(float, m_fAcceleratedSpeedX, AcceleratedSpeedX);
	CC_SYNTHESIZE(float, m_fAcceleratedSpeedY, AcceleratedSpeedY);
    // 气球的纹理名称，一般是放在plist中的文件名
    CC_SYNTHESIZE(std::string, m_strTextureName, TextureName);
	// 气球是否存活的标志
	CC_SYNTHESIZE_BOOL(m_bAlive, Alive);
	// 气球的分数
	CC_SYNTHESIZE(long, m_lScore, BalloonScore);
    // 设定气球的颜色，供后期使用，比如粒子效果
    CC_SYNTHESIZE(cocos2d::ccColor4F, m_pBalloonColor4F, BalloonColor4F);
	// 气球的效果的持续时间
	CC_SYNTHESIZE(long, m_lLastTime, BalloonLastTime);

public:
    Balloon();
    ~Balloon();
    
	// 创建函数
	static Balloon* create(const char* pszFileName);
    
    static Balloon* create(const char* pszFileName, long lScore, BalloonType nType = kBalloonTypeNormal);

	static Balloon* create(const char* pszFileName, const cocos2d::CCRect& rect);

	static Balloon* createWithSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame);

	static Balloon* createWithSpriteFrameName(const char* pszSpriteFrameName);
    
    // 更新分数
    void updateDisplayDesc();
    
    // 根据帧数跟新位置
    void updatePosition();

	// 气球爆炸效果
	void explosive();

	/**
	 * @brief 碰撞检测
	 * @param rect 传入的坐标必须是世界坐标系
	 */
	bool hitTest(const cocos2d::CCRect& rect);

	/**
	 * @brief 触摸检测
	 * @param point 传入的触摸点必须是世界坐标系
	 */
	bool touchTest(const cocos2d::CCPoint& point);
    
public:
    virtual void onEnter();

protected:
    
    // 气球的帧数，和场景帧数无关，初始化为0，之后每调用更新函数自动递增
    unsigned long m_ulFrame;
    
    // cocos2d::CCLabelTTF* m_pLabelDesc;
    cocos2d::CCLabelBMFont* m_pLabelBMFontDesc;
    
    std::string m_strDisplayDesc;
    
private:
    
};

#endif /* defined(__PokeBalloon__Balloon__) */

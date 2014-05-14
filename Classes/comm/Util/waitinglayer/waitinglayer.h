//
//  waitinglayer.h
//  KillBulls
//
//  Created by mac-zrb on 13-6-3.
//
//

#ifndef __KillBulls__waitinglayer__
#define __KillBulls__waitinglayer__

#include "cocos2d.h"
#include "cocos-ext.h"

//等待框层
class WaitingLayer :  public cocos2d::CCLayerColor
{
public:
    virtual bool init();
    
    virtual void onEnter();
    
    virtual void onExit();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual ~WaitingLayer();
    static WaitingLayer* shareWaitingLayer(); //单例
    //显示等待框
    void  showProgress(cocos2d::CCString* pTitle ,float lAutoCloseTime = 0);
    void  showProgress(std::string title ,float lAutoCloseTime = 0,bool bChangeScene = false);
    
    void addProgressToScene(float dt);
    //关闭等待框
    void  closeProgress();
    void autoCloseProgress(float dt);
    void createSprite();
    void destroySprite();
    //开始滚动动画
    void startAnimation();
    //停止等待动画
    void stopAnimation();
    CREATE_FUNC(WaitingLayer);
    
private:
    cocos2d::CCLabelTTF* m_pLabelTitle; //文字标题
    cocos2d::CCSprite* m_sprite;  //等待圈

private:
    bool m_bShowing;  //是否显示
    std::string m_title;
    float m_lAutoCloseTime;
};

#endif /* defined(__KillBulls__waitinglayer__) */

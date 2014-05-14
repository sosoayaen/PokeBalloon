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

//�ȴ����
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
    static WaitingLayer* shareWaitingLayer(); //����
    //��ʾ�ȴ���
    void  showProgress(cocos2d::CCString* pTitle ,float lAutoCloseTime = 0);
    void  showProgress(std::string title ,float lAutoCloseTime = 0,bool bChangeScene = false);
    
    void addProgressToScene(float dt);
    //�رյȴ���
    void  closeProgress();
    void autoCloseProgress(float dt);
    void createSprite();
    void destroySprite();
    //��ʼ��������
    void startAnimation();
    //ֹͣ�ȴ�����
    void stopAnimation();
    CREATE_FUNC(WaitingLayer);
    
private:
    cocos2d::CCLabelTTF* m_pLabelTitle; //���ֱ���
    cocos2d::CCSprite* m_sprite;  //�ȴ�Ȧ

private:
    bool m_bShowing;  //�Ƿ���ʾ
    std::string m_title;
    float m_lAutoCloseTime;
};

#endif /* defined(__KillBulls__waitinglayer__) */

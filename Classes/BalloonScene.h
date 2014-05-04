/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-04-19 18:53:29
*/

#ifndef __BALLOONSCENE__H__
#define __BALLOONSCENE__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "BalloonManager.h"

enum BalloonGameStatus
{
    GAME_STATUS_READY = 0,
    GAME_STATUS_RUNNING = 1,
    GAME_STATUS_STOP = 2
};

class BalloonScene:
	public cocos2d::CCLayer,
    public BalloonDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonScene()
	{
		m_pSpriteBalloonModel = NULL;
		m_pLabelTTFScore = NULL;
		m_pLabelTTFTime = NULL;
        m_pLayerBalloon = NULL;
        m_pSpriteBackground = NULL;
        // 注册监听对象
        m_BalloonManger.setBalloonDelegate(this);
        
        m_ulFrame = 0;
        m_lTotalScore = 0;
        m_ulTimeLeft = 0;
        
	}
	~BalloonScene();

	CREATE_FUNC(BalloonScene);

    static cocos2d::CCScene* scene();
    
	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void ccTouchCanceled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void update(float dt);
    
	virtual void balloonHitTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite);
    
	virtual void balloonTouchTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite);
	
private:
	// Attributes for CCB
	cocos2d::CCSprite* m_pSpriteBalloonModel;
	cocos2d::CCLabelTTF* m_pLabelTTFScore;
	cocos2d::CCLabelTTF* m_pLabelTTFTime;
    // 气球层
    cocos2d::CCLayer* m_pLayerBalloon;
    // 背景
    cocos2d::CCSprite* m_pSpriteBackground;


private:
    // 当前播放的帧
    unsigned long m_ulFrame;
    
    // 气球管理类
    BalloonManager m_BalloonManger;
    
    // 当前用户分数
    long m_lTotalScore;
    
    // 当前剩余时间
    unsigned long m_ulTimeLeft;
    
    // 游戏状态
    BalloonGameStatus m_eGameStatus;
    
private:
    // 重置数据
    void resetData();
    
    void updateScore();
    
    void updateTimeLeft();
    
    void showResultDialog();
    
    void onPressMenuRestartGame(cocos2d::CCObject* pSender);
    
    void timeCount(float dt);
    
    void startGame();
    
public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void keyBackClicked( void );
	virtual void keyMenuClicked( void );


	
public:
	// Funcitons





};

#endif // __BALLOONSCENE__H__
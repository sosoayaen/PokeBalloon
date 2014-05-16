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
#include "CloudManager.h"

#include "BalloonResultDialog.h"

enum BalloonGameStatus
{
    GAME_STATUS_READY = 0,  // 开始前的准备状态
    GAME_STATUS_RUNNING,    // 游戏进行状态
    GAME_STATUS_TIMES_UP,   // 准备结束状态，时间结束
    GAME_STATUS_STOP        // 游戏结束
    
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
		// m_pLabelTTFScore = NULL;
		// m_pLabelTTFTime = NULL;
        m_pLabelBMFontScore = NULL;
        m_pLabelBMFontTimeLeft = NULL;
        m_pLayerBalloon = NULL;
        m_pSpriteBackground = NULL;
        m_pResultDialog = NULL;
        
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
	// cocos2d::CCLabelTTF* m_pLabelTTFScore;
	// cocos2d::CCLabelTTF* m_pLabelTTFTime;
    cocos2d::CCLabelBMFont* m_pLabelBMFontTimeLeft;
    cocos2d::CCLabelBMFont* m_pLabelBMFontScore;
    
    cocos2d::CCSprite* m_pSpriteClock;
    // 气球层
    cocos2d::CCLayer* m_pLayerBalloon;
    // 背景
    cocos2d::CCSprite* m_pSpriteBackground;

    BalloonResultDialog* m_pResultDialog;

private:
    // 当前播放的帧
    unsigned long m_ulFrame;
    
    // 气球管理类
    BalloonManager m_BalloonManger;
    
    // 云朵管理对象
    CloudManager m_CloudManager;
    
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
    
    // 按钮回调
    void onPressMenuRestartGame(cocos2d::CCObject* pSender);
    void onPressMenuReturnMainMenu(cocos2d::CCObject* pSender);
    void onPressMenuShare(cocos2d::CCObject* pSender);
    // 对话框结束回调
    void onResultDialogEndCall(cocos2d::CCNode* pNode);
    
    void timeCount(float dt);
    
    void startGame();
    
    // 创建结算对话框
    void createResultDialog();
    
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
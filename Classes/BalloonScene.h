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
#include "BalloonItemManager.h"

#include "BalloonResultDialog.h"
#include "BalloonPauseDialog.h"

#include "BalloonAnalysis.h"

enum BalloonGameStatus
{
    GAME_STATUS_READY = 0,  // 开始前的准备状态
    GAME_STATUS_RUNNING,    // 游戏进行状态
    GAME_STATUS_PAUSE,      // 游戏暂停
    GAME_STATUS_TIMES_UP,   // 准备结束状态，时间结束
    GAME_STATUS_STOP        // 游戏结束
    
};

class BalloonScene:
	public cocos2d::CCLayer,
    public BalloonDelegate,
    public BalloonItemDelegate,
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
        m_pLayerItems = NULL;
        m_pSpriteBackground = NULL;
        m_pResultDialog = NULL;
        m_pPauseDialog = NULL;
        m_pMenuPause = NULL;
        m_pSpriteScoreBar = NULL;
        m_bCheated = false;
        
        m_nReadyTimeTime = 0.0;
        
        // 注册监听对象
        m_BalloonManager.setBalloonDelegate(this);
        
        m_ulFrame = 0;
        m_llTotalScore = 0;
        m_lTimeLeft = 0;
        
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
    
    /**
     * @brief 气球碰撞屏幕上的物体成功回调
     */
	virtual void balloonHitTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite);
    
    /**
     * @brief 气球被点击中后的回调
     */
	virtual void balloonTouchTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite);
    
    /**
     * @brief 气球移出屏幕后的回调
     */
    virtual void balloonMoveOutOfScreen(Balloon* pBalloon);
	
	// 道具起效通知，一般为按钮按下后的效果
	virtual void onBalloonItemEffectTrigger(BalloonItem* pItem);
    
	// 道具使用后消失通知
	virtual void onBalloonItemBeforeDisappear(BalloonItem* pItem);
	virtual void onBalloonItemAfterDisappear(BalloonItem* pItem);
    
private:
	// Attributes for CCB
	cocos2d::CCSprite* m_pSpriteBalloonModel;
    cocos2d::CCLabelBMFont* m_pLabelBMFontTimeLeft;
    cocos2d::CCLabelBMFont* m_pLabelBMFontScore;
    
    cocos2d::CCSprite* m_pSpriteClock;
    // 气球层
    cocos2d::CCLayer* m_pLayerBalloon;
    // 道具层
    cocos2d::CCLayer* m_pLayerItems;
    // 背景
    cocos2d::CCSprite* m_pSpriteBackground;
    
    cocos2d::CCMenu* m_pMenuPause;
    
    cocos2d::CCSprite* m_pSpriteScoreBar;

    BalloonResultDialog* m_pResultDialog;
    
    BalloonPauseDialog* m_pPauseDialog;

private:
    // 统计对象
    BalloonAnalysis m_BalloonAnalysis;
    
    // 开始前倒计时的变量
    int m_nReadyTimeTime;
    // 作弊标志
    bool m_bCheated;
    
    // 当前播放的帧
    unsigned long m_ulFrame;
    
    // 气球管理类
    BalloonManager m_BalloonManager;
    
    // 云朵管理对象
    CloudManager m_CloudManager;
    
    // 道具管理类
    BalloonItemManager m_BalloonItemManager;
    
    // 当前用户分数
    long long m_llTotalScore;
    
    // 当前剩余时间
    long m_lTimeLeft;
    
    // 游戏状态
    BalloonGameStatus m_eGameStatus;
    
private:
    // 重置数据
    void resetData();
    
    void updateScore();
    
    void updateTimeLeft();
    
    void showResultDialog();
    
    void showPauseDialog();
    
    // 按钮回调
    void onPressMenuRestartGame(cocos2d::CCObject* pSender);
    void onPressMenuReturnMainMenu(cocos2d::CCObject* pSender);
    void onPressMenuShare(cocos2d::CCObject* pSender);
    void onPressMenuResume(cocos2d::CCObject* pSender);
    void onPressMenuPause(cocos2d::CCObject* pSender);
    // 对话框结束回调
    void onResultDialogEndCall(cocos2d::CCNode* pNode);
    
    // 倒计时回调函数
    void timeCountCallback(CCNode* pNode);
    
    // 读秒倒计时
    void readReadySecond();
    
    // 游戏开始
    void startGame();
    
    // 创建结算对话框
    void createResultDialog();
    
    // 响应程序切换到后台
    void notifyEnterBackground(cocos2d::CCObject* pData);
    
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
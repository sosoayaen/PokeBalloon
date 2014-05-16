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
    GAME_STATUS_READY = 0,  // ��ʼǰ��׼��״̬
    GAME_STATUS_RUNNING,    // ��Ϸ����״̬
    GAME_STATUS_TIMES_UP,   // ׼������״̬��ʱ�����
    GAME_STATUS_STOP        // ��Ϸ����
    
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
        
        // ע���������
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
    // �����
    cocos2d::CCLayer* m_pLayerBalloon;
    // ����
    cocos2d::CCSprite* m_pSpriteBackground;

    BalloonResultDialog* m_pResultDialog;

private:
    // ��ǰ���ŵ�֡
    unsigned long m_ulFrame;
    
    // ���������
    BalloonManager m_BalloonManger;
    
    // �ƶ�������
    CloudManager m_CloudManager;
    
    // ��ǰ�û�����
    long m_lTotalScore;
    
    // ��ǰʣ��ʱ��
    unsigned long m_ulTimeLeft;
    
    // ��Ϸ״̬
    BalloonGameStatus m_eGameStatus;
    
private:
    // ��������
    void resetData();
    
    void updateScore();
    
    void updateTimeLeft();
    
    void showResultDialog();
    
    // ��ť�ص�
    void onPressMenuRestartGame(cocos2d::CCObject* pSender);
    void onPressMenuReturnMainMenu(cocos2d::CCObject* pSender);
    void onPressMenuShare(cocos2d::CCObject* pSender);
    // �Ի�������ص�
    void onResultDialogEndCall(cocos2d::CCNode* pNode);
    
    void timeCount(float dt);
    
    void startGame();
    
    // ��������Ի���
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
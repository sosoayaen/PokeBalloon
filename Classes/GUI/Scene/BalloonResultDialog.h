/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-05-14 23:42:32
*/

#ifndef __BALLOON_RESULT_DIALOG_H__
#define __BALLOON_RESULT_DIALOG_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"
#include "BalloonAnalysis.h"

class BalloonResultDialog:
	public DialogLayer,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonResultDialog()
	{
		m_pLabelBMFontCurrentScore = NULL;
		// m_pLabelBMFontHighestScore = NULL;
		m_pMenuShare = NULL;
        m_pMenuTop = NULL;
		m_pMenuItemPlayAgain = NULL;
		// m_pMenuItemReturn = NULL;
		// m_pMenuItemGotoShop = NULL;
        m_pMenuItemShare = NULL;
		// m_pMenuResult = NULL;
		m_pSpriteNewFlag = NULL;
		m_pSpriteResultBoard = NULL;
        m_pSpriteStar = NULL;
        m_pSpriteCoin = NULL;
        m_pSpriteCoinTop = NULL;
        m_pSpriteCoinReward = NULL;
        m_pSpriteMissionStatus = NULL;
        m_pLabelBMFontCoins = NULL;
        m_pLabelBMFontCoinQty = NULL;
        m_pLabelBMFontMissionReward = NULL;
        m_pLabelTTFMissionDesc = NULL;
        m_pSpriteSeperate = NULL;
        
        m_llTotalScore = 0;
        m_llHighestScore = 0;
        m_llUserCoins = 0;
        m_nCoinsReward = 0;
        m_bMissionCompleted = false;
        m_nCoinsAdd = 0;
        
        m_llScoreStep = 0;
        m_lCoinsWithScores = 0;
        m_lCoinsWithScoresCnt = 0;
	}
	~BalloonResultDialog();

	CREATE_FUNC(BalloonResultDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

    /**
     * @brief 获取分享的截图文件路径
     */
    std::string getSharedPictureFilePath();
    
public:
    // 这里的菜单都public，供外部调用
	cocos2d::CCMenuItem* m_pMenuItemPlayAgain;
    
private:
    // 控件原始位置
    cocos2d::CCPoint m_posOri;
    
    // 分享按钮初始位置
    cocos2d::CCPoint m_posShareItemOri;
    
    // 分享按钮初始角度
    float m_fShareItemAngleOri;
    
    // 当前总分和历史最高分
    long long m_llTotalScore;
    long long m_llHighestScore;
    // 奖励的金币数量
    int m_nCoinsReward;
    // 当局游戏可获得的金币数量
    int m_nCoinsAdd;
    // 用于显示的用户金币数量
    long long m_llUserCoins;
    
    bool m_bMissionCompleted;
    
private:
	// Attributes for CCB
	cocos2d::CCLabelBMFont* m_pLabelBMFontCurrentScore;
	// cocos2d::CCLabelBMFont* m_pLabelBMFontHighestScore;
    cocos2d::CCMenu* m_pMenuTop;
	cocos2d::CCMenu* m_pMenuShare;
    cocos2d::CCMenuItem* m_pMenuItemShare;
    // 分割对话框的档子
    cocos2d::CCSprite* m_pSpriteSeperate;
    cocos2d::CCLabelTTF* m_pLabelTTFMissionDesc;
    /*
	cocos2d::CCMenu* m_pMenuResult;
    */
	cocos2d::CCSprite* m_pSpriteNewFlag;
	cocos2d::CCSprite* m_pSpriteResultBoard;
    cocos2d::CCSprite* m_pSpriteMissionStatus;
    // 奖牌上的星星
    cocos2d::CCSprite* m_pSpriteStar;
    // 结算对话框上的金币
    cocos2d::CCSprite* m_pSpriteCoin;
    // 顶部的金币
    cocos2d::CCSprite* m_pSpriteCoinTop;
    cocos2d::CCSprite* m_pSpriteCoinReward;
    // 当次得到金币的数量
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;
    // 顶部金币数量
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoinQty;
    // 奖励的金币数量
    cocos2d::CCLabelBMFont* m_pLabelBMFontMissionReward;
    
public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

private:
    void initLabelTTF();
    
    void initMenuTop();
    // 初始化旋转的星星
    void initRotateStar();
    
    // 星星切换位置
    void changeStarPosition(cocos2d::CCNode* pNode);
    // 当前金币换算得到的金币数量
    long m_lCoinsWithScores;
    // 用在动画的时候计数用的
    long m_lCoinsWithScoresCnt;
    
    // 更新得分获得的金币
    void updateGameCoins(long long llCoins);
    long long m_llScoreStep;
    // 更新分数动画回调
    void timerCallbackUpdateScoreAndCoin(float dt);
    void startScoreAndCoinUpdateAnimation();
    
    // 更新金币数量动画
    void startCoinUpdateAnimation();
    
    // 更新顶部的金币数量
    void updateUserinfoCoins(long long llCoins);
    
    // 屏幕投放烟花
    // nFireworks 烟花个数，默认0表示随机得到3到10个烟花
    void addFireworks(int nFireworks = 0);
    void actionCallbackFireworksExplosive(cocos2d::CCNode* pNode);
    
    // 播放烟花发射音效
    void actionCallbackPlayFireworksTakeOffSound();
    
    // 添加金币的回调函数
    void actionCallbackAddCoins(cocos2d::CCNode* pNode, void* pData);
    
    void updateRewardCoins(int nCoins);
    
    void onPressMenuShare(cocos2d::CCObject* pSender);
    
    void onPressMenuReturnMain(cocos2d::CCObject* pSender);
    
public:
	// Funcitons
    
    // 设置结果分数
    void setScore(long long llScore);
    
    // 设置最高分
    void setHighScore(long long llScore);
    
    // 设置是否显示新的记录标志
    void setNewFlagVisible(bool bShow);
};

#endif // __BALLOON_RESULT_DIALOG_H__

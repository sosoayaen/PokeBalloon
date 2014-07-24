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
		m_pLabelBMFontHighestScore = NULL;
		m_pMenuShare = NULL;
		m_pMenuItemPlayAgain = NULL;
		m_pMenuItemReturn = NULL;
		m_pMenuItemGotoShop = NULL;
        m_pMenuItemShare = NULL;
		m_pMenuResult = NULL;
		m_pSpriteNewFlag = NULL;
		m_pSpriteResultBoard = NULL;
        m_pSpriteStar = NULL;
        m_pSpriteCoin = NULL;
        m_pLabelBMFontCoins = NULL;
        
        m_llTotalScore = 0;
        m_llHighestScore = 0;
        m_pAnalysisData = NULL;
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
	cocos2d::CCMenuItem* m_pMenuItemReturn;
	cocos2d::CCMenuItem* m_pMenuItemGotoShop;
    cocos2d::CCMenuItem* m_pMenuItemShare;
    
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
    
    const BalloonAnalysisData* m_pAnalysisData;
private:
	// Attributes for CCB
	cocos2d::CCLabelBMFont* m_pLabelBMFontCurrentScore;
	cocos2d::CCLabelBMFont* m_pLabelBMFontHighestScore;
	cocos2d::CCMenu* m_pMenuShare;
	cocos2d::CCMenu* m_pMenuResult;
	cocos2d::CCSprite* m_pSpriteNewFlag;
	cocos2d::CCSprite* m_pSpriteResultBoard;
    // 奖牌上的星星
    cocos2d::CCSprite* m_pSpriteStar;
    // 结算对话框上的金币
    cocos2d::CCSprite* m_pSpriteCoin;
    // 当次得到金币的数量
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;

    
public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

private:
    // 初始化旋转的星星
    void initRotateStar();
    
    // 星星切换位置
    void changeStarPosition(cocos2d::CCNode* pNode);
    
    // 更新
    void updateCoins(long long ulCoins);
    
public:
	// Funcitons
    
    // 设置结果分数
    void setScore(long long llScore);
    
    // 设置最高分
    void setHighScore(long long llScore);
    
    // 设置是否显示新的记录标志
    void setNewFlagVisible(bool bShow);
    
    // 设置分析数据的数据源
    void setAnalysisData(const BalloonAnalysisData* pData);

};

#endif // __BALLOON_RESULT_DIALOG_H__

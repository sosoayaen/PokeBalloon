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

	}
	~BalloonResultDialog();

	CREATE_FUNC(BalloonResultDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

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
    
private:
	// Attributes for CCB
	cocos2d::CCLabelBMFont* m_pLabelBMFontCurrentScore;
	cocos2d::CCLabelBMFont* m_pLabelBMFontHighestScore;
	cocos2d::CCMenu* m_pMenuShare;
	cocos2d::CCMenu* m_pMenuResult;
	cocos2d::CCSprite* m_pSpriteNewFlag;
	cocos2d::CCSprite* m_pSpriteResultBoard;

public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();


	
public:
	// Funcitons
    
    // 设置结果分数
    void setScore(long lScore);
    
    // 设置最高分
    void setHighScore(long lScore);
    
    // 设置是否显示新的记录标志
    void setNewFlagVisible(bool bShow);

};

#endif // __BALLOON_RESULT_DIALOG_H__
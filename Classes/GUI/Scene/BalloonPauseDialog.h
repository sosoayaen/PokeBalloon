/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-05-16 23:02:41
*/

#ifndef __BALLOONPAUSEDIALOG__H__
#define __BALLOONPAUSEDIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonPauseDialog:
	public DialogLayer,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonPauseDialog()
	{
		m_pMenuItemAgain = NULL;
		m_pMenuItemResume = NULL;
		m_pMenuItemReturn = NULL;
		m_pMenu = NULL;

	}
	~BalloonPauseDialog();

	CREATE_FUNC(BalloonPauseDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

	virtual bool onInitDialog()
	{
		return true;
	}

public:
	cocos2d::CCMenuItem* m_pMenuItemAgain;
	cocos2d::CCMenuItem* m_pMenuItemResume;
	cocos2d::CCMenuItem* m_pMenuItemReturn;
    
private:
	// Attributes for CCB
	cocos2d::CCMenu* m_pMenu;


public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();


	
public:
	// Funcitons





};

#endif // __BALLOONPAUSEDIALOG__H__
/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-05-19 00:21:44
*/

#ifndef __BALLOONOPTIONSDIALOG__H__
#define __BALLOONOPTIONSDIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonOptionsDialog:
	public DialogLayer,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonOptionsDialog()
	{
		m_pMenuItemMusic = NULL;
		m_pMenuItemEffect = NULL;
        m_pMenuReturn = NULL;
        m_pMenuMusic = NULL;
        m_pMenuEffect = NULL;

	}
	~BalloonOptionsDialog();

	CREATE_FUNC(BalloonOptionsDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

	virtual bool onInitDialog()
	{
		return true;
	}
	
private:
	// Attributes for CCB
	cocos2d::CCMenuItemImage* m_pMenuItemMusic;
	cocos2d::CCMenuItemImage* m_pMenuItemEffect;
    cocos2d::CCMenu* m_pMenuReturn;
    cocos2d::CCMenu* m_pMenuMusic;
    cocos2d::CCMenu* m_pMenuEffect;


private:
    
    void saveOptionsData();
    
	void onPressMenuReturn(cocos2d::CCObject* pSender);
    void onPressMenuMusic(cocos2d::CCObject* pSender);
    void onPressMenuEffect(cocos2d::CCObject* pSender);
    
    void initMenuItem();
    
    void setCheckBoxEnable(cocos2d::CCMenuItemImage* pMenuItem, bool bEnable);
    
public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();


	
public:
	// Funcitons





};

#endif // __BALLOONOPTIONSDIALOG__H__
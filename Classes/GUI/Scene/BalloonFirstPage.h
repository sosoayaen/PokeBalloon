/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-05-05 22:25:18
*/

#ifndef __BALLOONFIRSTPAGE__H__
#define __BALLOONFIRSTPAGE__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "AutoTextureManagerLayer.h"


class BalloonFirstPage:
	// public cocos2d::CCLayer,
    public AutoTextureManagerLayer,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonFirstPage()
	{
		m_pSpriteBackground = NULL;
		m_pMenuMain = NULL;
        m_pSpriteTitle = NULL;
        m_pFireworks = NULL;
        m_pFireworks2 = NULL;
        m_pLabelTTFNotice = NULL;
        m_pSpriteNoticeBackground = NULL;
        m_pLabelTTFVersion = NULL;

	}
	~BalloonFirstPage();

	CREATE_FUNC(BalloonFirstPage);
    
    static cocos2d::CCScene* scene();

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

	
private:
	// Attributes for CCB
	cocos2d::CCSprite* m_pSpriteBackground;
	cocos2d::CCMenu* m_pMenuMain;
    cocos2d::CCSprite* m_pSpriteTitle;
    cocos2d::CCLabelTTF* m_pLabelTTFNotice;
    cocos2d::CCSprite* m_pSpriteNoticeBackground;
    cocos2d::CCLabelTTF* m_pLabelTTFVersion;

private:
    cocos2d::CCParticleSystemQuad* m_pFireworks;
    cocos2d::CCParticleSystemQuad* m_pFireworks2;

public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();


	
public:
	// Funcitonsjj
    
protected:
    virtual bool setResourceString();

private:
    
    void initMenu();

    void onPressMenuStartGame(cocos2d::CCObject* pSender);
    void onPressMenuRankBoard(cocos2d::CCObject* pSender);
    void onPressMenuOptions(cocos2d::CCObject* pSender);
    void onPressMenuHandbook(cocos2d::CCObject* pSender);

};

#endif // __BALLOONFIRSTPAGE__H__
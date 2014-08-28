/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-08-27 23:09:44
*/

#ifndef __BALLOONRANKDIALOG__H__
#define __BALLOONRANKDIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonRankDialog:
	public DialogLayer,
	public cocos2d::extension::CCTableViewDataSource,
	public cocos2d::extension::CCTableViewDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonRankDialog()
	{
		m_pLayerTableViewContainer = NULL;
		m_pMenuTop = NULL;
		m_pSpriteContainer = NULL;
		m_pSpriteBoard = NULL;
        m_pSpriteScoreFire = NULL;
        
        m_pTableViewRank = NULL;

        m_pDictRankData = NULL;
	}
	~BalloonRankDialog();

	CREATE_FUNC(BalloonRankDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

	
private:
	// Attributes for CCB
	cocos2d::CCLayer* m_pLayerTableViewContainer;
	cocos2d::CCMenu* m_pMenuTop;
	cocos2d::CCSprite* m_pSpriteContainer;
	cocos2d::CCSprite* m_pSpriteBoard;
    cocos2d::CCSprite* m_pSpriteScoreFire;
    
    cocos2d::extension::CCTableView* m_pTableViewRank;

    // Http data object
    cocos2d::CCDictionary* m_pDictRankData;
    
private:
	bool initLabelTTF();
    
    void initTableView();
    
    void initMenu();
    
    void createCellForScoreRank(cocos2d::extension::CCTableViewCell* cell, cocos2d::extension::CCTableView* table, unsigned int idx);
    
private:
    // handle http data callback
    void requestRankData();
    void NotifyHttpRankDataCallback(cocos2d::CCObject* pData);
    cocos2d::CCArray* getRankDataArray();

public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void keyBackClicked( void );
	virtual void keyMenuClicked( void );

	//////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate virtual function
	//////////////////////////////////////////////////////////////////////////
	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);

	//////////////////////////////////////////////////////////////////////////
	// CCTableViewDelegate virtual function
	//////////////////////////////////////////////////////////////////////////

	virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);

	virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);

	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);

	virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

	
public:
	// Funcitons
	void onPressMenuShare(cocos2d::CCObject* pSender);
	void onPressMenuClose(cocos2d::CCObject* pSender);

};

#endif // __BALLOONRANKDIALOG__H__

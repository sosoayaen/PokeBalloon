/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-07-14 22:03:43
*/

#ifndef __BALLOONITEMSELECTDIALOG__H__
#define __BALLOONITEMSELECTDIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonItemSelectDialog:
	public DialogLayer,
	public cocos2d::extension::CCTableViewDataSource,
	public cocos2d::extension::CCTableViewDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonItemSelectDialog()
	{
		m_pScale9SpriteBuyBoard = NULL;
        m_pLabelBMFontCoins = NULL;
        m_pSpriteCoin = NULL;
        m_pMenu = NULL;
        
        m_pArrayItems = NULL;
        
	}
	~BalloonItemSelectDialog();

	CREATE_FUNC(BalloonItemSelectDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

private:
	// Attributes for CCB
	cocos2d::extension::CCScale9Sprite* m_pScale9SpriteBuyBoard;
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;
    cocos2d::CCSprite* m_pSpriteCoin;
    cocos2d::CCMenu* m_pMenu;
    
    cocos2d::extension::CCTableView* m_pTableView;

    cocos2d::CCArray* m_pArrayItems;
    
    // cocos2d::CCDictionary* m_pDictExtItemConfig;
    
public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

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
    
    void onPressMenuStart(cocos2d::CCObject* pSender);
    void onPressMenuReturnMain(cocos2d::CCObject* pSender);

	
public:
	// Funcitons

private:
    
    // 更新金币数目
    void updateCoins();




};

#endif // __BALLOONITEMSELECTDIALOG__H__
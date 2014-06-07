/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-06-05 22:47:24
*/

#ifndef __BALLOONHANDBOOKSCENE__H__
#define __BALLOONHANDBOOKSCENE__H__

#include "cocos2d.h"
#include "cocos-ext.h"

class BalloonHandBookScene:
    public cocos2d::CCLayer,
	public cocos2d::extension::CCTableViewDataSource,
	public cocos2d::extension::CCTableViewDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonHandBookScene()
	{
        m_pSpriteBackground = NULL;
        m_pLayerItemContainer = NULL;
        m_pMenu = NULL;
        m_pSpriteCoin = NULL;
        m_pLabelBMFontCoins = NULL;
        
        m_pTableViewHandbook = NULL;
        
        m_pDictHandbookData = NULL;
	}
	~BalloonHandBookScene();

	CREATE_FUNC(BalloonHandBookScene);
    
    static cocos2d::CCScene* scene();

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
    cocos2d::CCSprite* m_pSpriteBackground;
    cocos2d::CCLayer* m_pLayerItemContainer;
    cocos2d::CCMenu* m_pMenu;
    cocos2d::CCSprite* m_pSpriteCoin;
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;
    
private:
    // 图鉴列表
    cocos2d::extension::CCTableView* m_pTableViewHandbook;
    
    // 图鉴配置文件
    cocos2d::CCDictionary* m_pDictHandbookData;
    
    // 屏幕大小
    cocos2d::CCSize m_winSize;

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

private:
    // 根据配置文件初始化解锁列表
    void initItemTableView();
    
    void onPressMenuReturnMainBoard(cocos2d::CCObject* pSender);
    
    // 根据金币数目重新布局
    void reLayoutCoins();
	
public:
	// Funcitons





};

#endif // __BALLOONHANDBOOKSCENE__H__
/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-09-10 22:07:08
*/

#ifndef __BALLOONSHOPDIALOG__H__
#define __BALLOONSHOPDIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonShopDialog:
	public DialogLayer,
	public cocos2d::extension::CCTableViewDataSource,
	public cocos2d::extension::CCTableViewDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonShopDialog()
	{
		m_pLayerTableViewContainer = NULL;
		m_pMenuTop = NULL;
		m_pSpriteContainer = NULL;
        m_pSpriteItemTemplate = NULL;
        m_pSpriteCoin = NULL;
		m_pLabelBMFontCoins = NULL;
		m_pSpriteBoard = NULL;
        
        m_pTableView = NULL;
        
        m_pArrayItemData = NULL;

	}
	~BalloonShopDialog();

	CREATE_FUNC(BalloonShopDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

	
private:
	// Attributes for CCB
    
    // TableView容器
	cocos2d::CCLayer* m_pLayerTableViewContainer;
    // 顶部按钮
	cocos2d::CCMenu* m_pMenuTop;
    // 边框容器
	cocos2d::CCSprite* m_pSpriteContainer;
    // 商品模板
    cocos2d::CCSprite* m_pSpriteItemTemplate;
    // 金币图像
    cocos2d::CCSprite* m_pSpriteCoin;
    // 顶部金币数量
	cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;
    // 整个面板
	cocos2d::CCSprite* m_pSpriteBoard;

    cocos2d::extension::CCTableView* m_pTableView;
    
    // 存放iOS商店内商品的数组
    cocos2d::CCArray* m_pArrayItemData;
private:
	bool initLabelTTF();
    
    void initSysMenu();
    
    void initTableView();

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

	
private:
	// Funcitons
    void scheduleCallbackCheckProduct(float dt);
    void scheduleCallbackCheckPurchaseStatus(float dt);
    
	void onPressMenuShare(cocos2d::CCObject* pSender);
	void onPressMenuClose(cocos2d::CCObject* pSender);
    
    void updateCoins();
    
public:
    
};

#endif // __BALLOONSHOPDIALOG__H__

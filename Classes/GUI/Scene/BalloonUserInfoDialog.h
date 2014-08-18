/**
* Create by GenerateCppCodeFromCCBFile.lua
* All right received
* Author: Jason Tou
* Date: 2014-08-16 15:59:54
*/

#ifndef __BALLOONUSERINFODIALOG__H__
#define __BALLOONUSERINFODIALOG__H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DialogLayer.h"

class BalloonUserInfoDialog:
	public DialogLayer,
	public cocos2d::extension::CCTableViewDataSource,
	public cocos2d::extension::CCTableViewDelegate,
	public cocos2d::extension::CCBSelectorResolver,
	public cocos2d::extension::CCBMemberVariableAssigner
{
public:
	// Constructor
	BalloonUserInfoDialog()
	{
		m_pLabelTTFAchivementHolder = NULL;
		m_pLabelTTFAchivementValue = NULL;
		m_pLabelTTFHighScoreHolder = NULL;
        m_pLabelTTFHighestScoreValue = NULL;
		m_pSpriteScoreArea = NULL;
        m_pMenuItemMusic = NULL;
        m_pMenuItemSound = NULL;
		m_pMenuMusic = NULL;
		m_pMenuItem1 = NULL;
		m_pMenuItem2 = NULL;
		m_pMenuItem3 = NULL;
		m_pMenuTab = NULL;
		m_pSpriteTableViewBackground = NULL;
		m_pLabelBMFontCoins = NULL;
		m_pLabelTTFNickname = NULL;
		m_pSpriteBoard = NULL;

        m_pMenuItemLabelNickName = NULL;
        m_pMenuNickname = NULL;
        
        m_pTableViewDetail = NULL;
        
        m_pDictHandbookData = NULL;
        
        m_nTabIndex = 0;
	}
	~BalloonUserInfoDialog();

	CREATE_FUNC(BalloonUserInfoDialog);

	virtual cocos2d::SEL_CallFuncN onResolveCCBCCCallFuncSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector( cocos2d::CCObject * pTarget, const char* pSelectorName );

	virtual bool onAssignCCBMemberVariable( cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode );

private:
	// Attributes for CCB
	cocos2d::CCLabelTTF* m_pLabelTTFAchivementHolder;
	cocos2d::CCLabelTTF* m_pLabelTTFAchivementValue;
	cocos2d::CCLabelTTF* m_pLabelTTFHighScoreHolder;
    cocos2d::CCLabelTTF* m_pLabelTTFHighestScoreValue;
	cocos2d::CCSprite* m_pSpriteScoreArea;
    cocos2d::CCMenuItemImage* m_pMenuItemMusic;
    cocos2d::CCMenuItemImage* m_pMenuItemSound;
	cocos2d::CCMenu* m_pMenuMusic;
	cocos2d::CCMenuItem* m_pMenuItem1;
	cocos2d::CCMenuItem* m_pMenuItem2;
	cocos2d::CCMenuItem* m_pMenuItem3;
	cocos2d::CCMenu* m_pMenuTab;
	cocos2d::CCSprite* m_pSpriteTableViewBackground;
	cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;
	cocos2d::CCLabelTTF* m_pLabelTTFNickname;
	cocos2d::CCSprite* m_pSpriteBoard;
    
    cocos2d::CCMenuItemLabel* m_pMenuItemLabelNickName;
    cocos2d::CCMenu* m_pMenuNickname;
    
    cocos2d::extension::CCTableView* m_pTableViewDetail;
    
    // 图鉴配置文件
    cocos2d::CCDictionary* m_pDictHandbookData;
    
    // 当前选择的标签
    unsigned int m_nTabIndex;

private:
    // 初始化文字
    void initLabelTTF();
    // 初始化标签按钮上的文字
    void initMenuTabText();
    // 把文字添加到节点上，对应的CCLabelTTF是居中的
    void addLabelTTFToNode(cocos2d::CCNode* pNode, const char* pszText);
    // 初始化按下变大的按钮
    void initPressMenuEffect();
    // 把禁止标志添加到节点上，右下角
    void addForbiddenSpriteToNode(cocos2d::CCNode* pNode);
    // 设置对应节点的禁止标志是否显示，内部取对应的节点tagid
    void setForbiddenSpriteVisible(cocos2d::CCNode* pNode, bool bVisible);
    // 设置音乐音效按钮的状态是否启用（是否有禁止标志）
    void setMenuMusicBtnStatusEnable(cocos2d::CCMenuItemImage* pItem, bool bEnable);
    // 初始化表格
    void initTableView();
    // 保存当前页面的所有数据到磁盘
    void saveUserinfoData();
    // update the user relative label data
    void updateLabelData();
    // 创建道具升级的标签页单元内容，主要是修改第二个参数pCell
    void createTableCellLevelUp(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* pCell, unsigned int idx);

public:
	// Virtual Functions
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void keyBackClicked( void );
	virtual void keyMenuClicked( void );
	virtual bool endDialog(cocos2d::CCActionInterval* pAction = NULL, float fDuration = 1.0f);
    
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
    virtual cocos2d::CCSize tableCellSizeForIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
    virtual void tableCellUnhighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell);
    virtual void tableCellHighlight(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell);
	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

	
public:
	// Funcitons
	void onPressMenuMusic(cocos2d::CCObject* pSender);
	void onPressMenuSound(cocos2d::CCObject* pSender);
	void onPressMenuTabDetail(cocos2d::CCObject* pSender);
    void onPressMenuNickname(cocos2d::CCObject* pSender);

};

#endif // __BALLOONUSERINFODIALOG__H__
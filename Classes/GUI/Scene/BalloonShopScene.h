// =====================================================================================
//
//       Filename:  BalloonShopScene.h
//
//    Description:  商店界面
//
//        Version:  1.0
//        Created:  07/27/2014 02:09:00
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Jason Tou (), sosoayaen@gmail.com
//   Organization:  
//
// =====================================================================================

#ifndef __BALLOON_SHOP_H__
#define __BALLOON_SHOP_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class BalloonShopScene : 
    public cocos2d::CCLayer,
    public cocos2d::extension::CCTableViewDelegate,
    public cocos2d::extension::CCTableViewDataSource
{
public:
	BalloonShopScene();
	virtual ~BalloonShopScene();

	CREATE_FUNC(BalloonShopScene);
    static cocos2d::CCScene* scene();

public:
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

    virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);
    virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);
    virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
    virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);
    virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
    
protected:
    // 金币
    cocos2d::CCSprite* m_pSpriteCoin;
    
    // 金币显示值
    cocos2d::CCLabelBMFont* m_pLabelBMFontCoins;

private:
    // 定时器检查商品列表是否从苹果返回
	void checkProduct(float dt);
    
    // 定时器校验购买是否成功
    void checkPurchaseStatus(float dt);
    
    // 更新金币的值
    void updateCoins();
    
private:
    // AppStore返回过来的商品，后期也可以配置到服务器上
    cocos2d::extension::CCTableView* m_pTableViewItems;
    
    // 商品数据
    cocos2d::CCArray* m_pArrayItemData;
};

#endif

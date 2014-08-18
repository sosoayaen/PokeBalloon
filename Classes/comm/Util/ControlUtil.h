/************************************************************************/
/* ControlUtil.h                                                        */
/* Author: Jason Tou													*/
/* 针对cocos2dx的一些控件统一简便操作									*/
/************************************************************************/

#ifndef __CONTROL_UTIL__
#define __CONTROL_UTIL__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "bailinMacro.h"

NS_BAILIN_UTIL_BEGIN

class ControlUtil
{
public:
	ControlUtil();
	~ControlUtil();

	static ControlUtil* sharedControlUtil();

	static void purgeSharedControlUtil();

	/**
	* @brief 设定CCLabelTTF控件的字体大小和屏幕适应
	* @param pLabel 需要设定的LabelTTF对象指针
	*/
	void FitLabelTTFFontSize(cocos2d::CCLabelTTF* pLabel);

	/**
	* @brief 设定控件是否可见
	*/
	void SetNodeVisible(cocos2d::CCNode* pNode, bool bVisible);

	/**
	* @brief 设置按钮下的所有按钮都有按下后的缩放效果
	* @param pMenu 对应的按钮容器
	* @param fRate 对应的缩放比例，默认1.1倍，放大效果	
	*/
	void SetMenuItemSelectedImageWithNormalImage( cocos2d::CCMenu* pMenu, float fRate = 1.1f );

	/**
	* @brief 设置按钮按下效果为普通图片的缩放
	* @param pMenuItemImage 对应需要设置按下效果的菜单控件
	* @param fRate 对应的缩放比例，默认1.1倍，放大效果
	*/
	void SetMenuItemSelectedImageWithNormalImage( cocos2d::CCMenuItemImage* pMenuItemImage, float fRate = 1.1f );

	/**
	* @brief 设置按钮下的所有按钮都有按下后的缩放效果，附带把内部文字也一起折腾一下
	* @param pMenu 对应的按钮容器
	* @param fRate 对应的缩放比例，默认1.1倍，放大效果	
	*/
	void SetMenuItemSelectedImageWithNormalImageAndLabel( cocos2d::CCMenu* pMenu, float fRate = 1.1f );

	/**
	* @brief 设置按钮按下效果为普通图片的缩放
	* @param pMenuItemImage 对应需要设置按下效果的菜单控件
	* @param fRate 对应的缩放比例，默认1.1倍，放大效果
	*/
	void SetMenuItemSelectedImageWithNormalImageAndLabel( cocos2d::CCMenuItemImage* pMenuItemImage, float fRate = 1.1f );

	/**
	* @brief 弹出框，内容默认为UTF8，如果有变化会在实现中用宏定义隔开实现
	*        弹出框仅仅是cocos2dx默认的各个平台的消息框
	*/
	void ShowCocos2dxMessageBox(const char* pszContent, const char* pszTitle = NULL);
    
    /**
     * @brief 返回是否按钮可按下的
     * @param dTimeIntervalMillionSecound 按钮的按下间隔，毫秒级别，如果小于等于0则默认1.5秒
     */
    bool isButtonCanTouch(double dTimeIntervalMillionSecound = -1.0);
    
    /**
     * @brief 得到节点的中心坐标位置，坐标系在该节点下
     * @param pNode 对应取中点坐标的点
     * @return CCPoint
     */
    cocos2d::CCPoint getCenterPointWithNode(cocos2d::CCNode* pNode);

	/**
	* @brief 弹出一个加载等待的层
	*/
	void ShowLoadingWaitLayer();

	/**
	* @brief 隐藏记载等待的层
	*/
	void HideLoadingWaitLayer();
    
    bool AddEventListenToSprit( cocos2d::CCSprite* sprit ,cocos2d::CCObject *target, cocos2d::SEL_MenuHandler selector , int menuItemTag);

protected:
	
private:
	// 屏幕大小
	cocos2d::CCSize m_sizeScreen;
};

NS_BAILIN_UTIL_END
#endif // __CONTROL_UTIL__
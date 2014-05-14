#ifndef __COMPANY_PAGE_H__
#define __COMPANY_PAGE_H__

#include "cocos2d.h"

class CompanyPage : public cocos2d::CCLayerColor
{
public:
	CompanyPage();
	~CompanyPage();

	CREATE_FUNC(CompanyPage);

	static cocos2d::CCScene* scene();

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

private:

	// 进入游戏
	void showGameScene();

private:

	// 工作室名称
	cocos2d::CCSprite* m_pSpriteEnglishName;
	// 工作室LOGO
	cocos2d::CCSprite* m_pSpriteLogo;
	// 工作室中文名称
	cocos2d::CCSprite* m_pSpriteChineseName;
	// 遮罩层
	cocos2d::CCLayerColor* m_pLayerColorMask;

};

#endif // __COMPANY_PAGE_H__

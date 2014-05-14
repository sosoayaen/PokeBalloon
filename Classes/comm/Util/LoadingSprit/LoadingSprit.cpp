#include "LoadingSprit.h"

USING_NS_CC;

#define Tag_LoadingSprit  123001 
#define Tag_LoadingSprit_Loading_Sprit  123002


LoadingSprit::LoadingSprit()
{
	
}


LoadingSprit::~LoadingSprit()
{
	
}

bool LoadingSprit::init()
{
	CCLayer::init();

	return true;
}

void LoadingSprit::onEnter()
{
	CCLayer::onEnter();
	ShowLoading();
}

void LoadingSprit::ShowLoading()
{
	CCSprite* sprite = (CCSprite*)(getChildByTag(Tag_LoadingSprit_Loading_Sprit));
	if (sprite == NULL )
	{
		sprite = CCSprite::create("ccbResources/common/loading.png");
		if (sprite)
		{
			sprite->setTag(Tag_LoadingSprit_Loading_Sprit);
			sprite->setAnchorPoint(ccp(0.5f, 0.5f));
			this->setContentSize(sprite->getContentSize());
			sprite->setPosition( ccp(getContentSize().width/2, getContentSize().height/2 ) );
			this->addChild(sprite);
		}
	}

	this->setContentSize(sprite->getContentSize());


	//Ðý×ª
	CCRotateBy * rotateAction = CCRotateBy::create(1.0f/25, 45/2.0f);
	CCAction* pAction = CCRepeatForever::create(rotateAction);
	getChildByTag(Tag_LoadingSprit_Loading_Sprit)->runAction(pAction);
}

LoadingSprit* LoadingSprit::ShowLoading(cocos2d::CCNode* showingOnLayer, cocos2d::CCPoint positon , float wantSize)
{
	LoadingSprit* loadingLayer = (LoadingSprit*)(showingOnLayer->getChildByTag(Tag_LoadingSprit));
	if (loadingLayer != NULL)
	{
		loadingLayer->removeFromParent();
	}

	loadingLayer = LoadingSprit::create();
	loadingLayer->setTag(Tag_LoadingSprit);

	loadingLayer->ShowLoading();
	positon.x = positon.x - loadingLayer->getContentSize().width/2;
	positon.y = positon.y - loadingLayer->getContentSize().height/2;
	loadingLayer->setPosition(positon);


	if (wantSize != 0)
	{
		//Ëõ·Å
		float scaleTo = wantSize/loadingLayer->getContentSize().width;
		loadingLayer->setScale(scaleTo);
	}

	loadingLayer->setZOrder(100);

	showingOnLayer->addChild(loadingLayer);
//	loadingLayer->onEnter();

	return loadingLayer;
}

void LoadingSprit::HideLoading(cocos2d::CCNode* showingOnLayer)
{
	showingOnLayer->removeChildByTag(Tag_LoadingSprit);
}

#undef Tag_LoadingSprit
#undef Tag_LoadingSprit_Loading_Sprit
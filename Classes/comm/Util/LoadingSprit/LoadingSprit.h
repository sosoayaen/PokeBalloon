#ifndef _H_LoadingSprit_H
#define _H_LoadingSprit_H

#include <vector>
#include "cocos2d.h"

//等待框
class LoadingSprit :  public cocos2d::CCLayer 
{
public:
	LoadingSprit();
	~LoadingSprit();

	CREATE_FUNC(LoadingSprit);
	bool init();

	static LoadingSprit* ShowLoading(cocos2d::CCNode* showingOnLayer, cocos2d::CCPoint positon , float wantSize = 0);
	static void HideLoading(cocos2d::CCNode* showingOnLayer);

	void onEnter();
	void ShowLoading();

};


#endif//_H_LoadingSprit_H


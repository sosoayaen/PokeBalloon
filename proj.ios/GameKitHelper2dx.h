/*
 * =====================================================================================
 *
 *       Filename:  GameKitHelper2dx.h
 *
 *    Description:  iOS Game Center 的辅助类，提供接口给cocos2dx调用oc的功能
 *
 *        Version:  1.0
 *        Created:  05/28/2014 18:03:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __GAME_KIT_HELPER_2DX_H__
#define __GAME_KIT_HELPER_2DX_H__

class GameKitHelper2dx
{
public:
	// 认证GameCenter
	static void authGameCenter();

	// 发送得分
	static void uploadScore(long long llScore);
    
    // 显示排行榜
    static void showLeaderboard();
    
    // 弹出对应评分对话框
    static void showRateMessageBox();

};

#endif // __GAME_KIT_HELPER_2DX_H__

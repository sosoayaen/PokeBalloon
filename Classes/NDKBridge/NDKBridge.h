/*
 * =====================================================================================
 *
 *       Filename:  NDKBridge.h
 *
 *    Description:  到底层设备的桥接层
 *
 *        Version:  1.0
 *        Created:  2014/07/05 20时14分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __NDK_BRIDGE_H__
#define __NDK_BRIDGE_H__

#include "cocos2d.h"

class NDKBridge
{
public:
	NDKBridge();
	virtual ~NDKBridge();

	static NDKBridge* sharedNDKBridge();
    
    // 设置本地推送信息
    void setNotification(cocos2d::CCDictionary* pData);
    
    // 取消本地推送信息
    void cancelNotification(cocos2d::CCDictionary* pData);
};

#endif // __NDK_BRIDGE_H__

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
#include <string>

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
    
    // 向服务器获取内购商品数据
    void initIAP(cocos2d::CCArray* pArrayProductID, const char* pszShareSecret = NULL);
    
    // 设置广告产品对应的KEY名
    void setADProductID(const char* pszADProductID);
    
    // 购买IAP商店内的商品
    void buyIAPProduct(unsigned int productIdx);
    
    // 是否去处广告
    bool isADOff();
    
    // 恢复购买
    void restoreIAPProducts();
    
private:
    // IAP校验值
    std::string m_strIAPShareSecret;
    
    // IAP中广告产品的key
    std::string m_strIAPADProductID;
};

#endif // __NDK_BRIDGE_H__

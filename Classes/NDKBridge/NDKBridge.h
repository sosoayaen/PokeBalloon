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
#include "GAdMob2DX.h"
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
    
    // 初始化广告对象
    bool initAdMob(const char* pszUnitID, const char* pszClassName = NULL);
    
    // 设置广告对象是否可见
    void setAdMobVisible(bool bVisible);
    
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
    
    // 清除本机上的IAP数据
    void clearSavedPurchasedProducts();
    
    // 获得机器的UDID
    std::string getDeviceUDID();
    
private:
    // IAP校验值（目前只有在iOS上有用）
    std::string m_strIAPShareSecret;
    
    // IAP中广告产品的key（目前仅在iOS有效，直接送给AppStore）
    std::string m_strIAPADProductID;
};

#endif // __NDK_BRIDGE_H__

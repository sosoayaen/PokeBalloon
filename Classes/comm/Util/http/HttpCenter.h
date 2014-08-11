/*
 * =====================================================================================
 *
 *       Filename:  HttpCenter.h
 *
 *    Description:  处理http请求的文件
 *
 *        Version:  1.0
 *        Created:  08/09/2014 15:21:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __HTTP_CENTER_H__
#define __HTTP_CENTER_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "bailinMacro.h"

NS_BAILIN_UTIL_BEGIN

class HttpCenter : public cocos2d::CCObject
{
public:
	HttpCenter();
	virtual ~HttpCenter();

	static HttpCenter* sharedHttpCenter();
    
    /**
     * @brief 
     * @param pszUrl 请求的网址
     * @param pszNotifyName 回调到CCNotifactionCenter的名称，数据到了后会直接postNotification
     * @param eType 默认为kHttpGet
     * @param reqData 发送的数据
     */
    void request(const char* pszUrl, const char* pszNotifyName, cocos2d::extension::CCHttpRequest::HttpRequestType eType = cocos2d::extension::CCHttpRequest::kHttpGet, const char* reqData = NULL);
    
protected:
	void onHttpRequestCompleted(cocos2d::extension::CCHttpClient* pClient, cocos2d::extension::CCHttpResponse* pResponse);
};

NS_BAILIN_UTIL_END

#endif // __HTTP_CENTER_H__

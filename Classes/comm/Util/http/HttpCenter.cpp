/*
 * =====================================================================================
 *
 *       Filename:  HttpCenter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/09/2014 15:21:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "HttpCenter.h"
#include "support/zip_support/ZipUtils.h"
#include "MemBufferObject.h"

NS_BAILIN_UTIL_BEGIN

USING_NS_CC;
USING_NS_CC_EXT;

HttpCenter::HttpCenter()
{

}

HttpCenter::~HttpCenter()
{

}

static HttpCenter* g_sHttpCenter = NULL;

HttpCenter* HttpCenter::sharedHttpCenter()
{
	if (!g_sHttpCenter)
	{
		g_sHttpCenter = new HttpCenter;
	}

	return g_sHttpCenter;
}

void HttpCenter::onHttpRequestCompleted(CCHttpClient* pClient, CCHttpResponse* pResponse)
{
    if (!pResponse)
    {
        return;
    }
    
    if (pResponse->isSucceed())
    {
        CCHttpRequest* request = pResponse->getHttpRequest();
        
        std::vector<char> *vHeader = pResponse->getResponseHeader();
        std::vector<char> *vData = pResponse->getResponseData();
        
        std::string strResponseHeader(&vHeader->front(), vHeader->size());
        CCLOG("%s", strResponseHeader.c_str());
        CCLOG("Http response data len:%lu", vData->size());
        CCLOG("%s", &vData->front());
        
        MemBufferObject* pMemObj = NULL;
        // 这里要处理下是否要对返回的数据进行解码或者其他操作
        if (strResponseHeader.find("Content-Encoding: gzip") != std::string::npos)
        {
            // 解码
            unsigned char *deflated = NULL;
            int deflatedLen = ZipUtils::ccInflateMemory((unsigned char *)&vData->front(), vData->size(), &deflated  );
            if (deflatedLen > 0)
            {
                // 解析成功
                pMemObj = MemBufferObject::create((const char*)deflated, deflatedLen);
            }
            else
            {
                // 解析失败就直接放数据进去
                pMemObj = MemBufferObject::create(&vData->front(), vData->size());
            }
        }
        else
        {
            pMemObj = MemBufferObject::create(&vData->front(), vData->size());
        }
        
        if (strlen(request->getTag()) > 0)
        {
            // 向外抛出对应的数据
            CCNotificationCenter::sharedNotificationCenter()->postNotification(request->getTag(), pMemObj);
        }
    }
}

void HttpCenter::request(const char* pszUrl, const char* pszNotifyName, CCHttpRequest::HttpRequestType eType /* = CCHttpRequest::kHttpGet */, const char* reqData /* = NULL */)
{
    if (!pszUrl) return;
    
    CCHttpRequest* request = new CCHttpRequest;
    
    request->setRequestType(eType);
    request->setUrl(pszUrl);
    request->setResponseCallback(this, httpresponse_selector(HttpCenter::onHttpRequestCompleted));
    if (reqData)
        request->setRequestData(reqData, strlen(reqData));
    if (pszNotifyName)
        request->setTag(pszNotifyName);
    
    // Send HttpRequest
    CCHttpClient::getInstance()->send(request);
    
    request->release();
}

NS_BAILIN_UTIL_END
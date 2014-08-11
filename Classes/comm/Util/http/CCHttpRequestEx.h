/****************************************************************************
 @author greathqy@gmail.com
 @weibo http://weibo.com/u/1678467541
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#ifndef __CCHttpRequestEx_h
#define __CCHttpRequestEx_h

#include "cocos2d.h"
#include <curl/curl.h>
#include "MemBuffer.h"

USING_NS_CC;

NS_CC_BEGIN

//Http request type
typedef enum {
    kHttpRequestGet,
    kHttpRequestPost,
    kHttpRequestDownloadFile,
	kHttpRequestMemoryDownload,//内存块下载
	kHttpRequestFileUpLoad,//文件上传
} HttpRequestType;

//Request structure
typedef struct {
    HttpRequestType             reqType;
    std::string                 url;
    std::string                 reqData;
    std::vector<std::string>    files;
    std::string                 reqName;
	std::string                 reqUploadFileName;//上传的文件 文件名
	std::string					str_copyName;//key 栏的名字，文件名
	std::string					str_fileNickName;//文件名的别名
    CCObject                    *pTarget;
    SEL_CallFuncND              pSelector;
    int                         nTag;
	int							nRequestIndex;// 请求时的索引 id ，每次请求都会上涨。这样应用内就可以标示这个请求时不是哪个页面发送的
} HttpRequestPacket;

//Response structure
typedef struct {
    HttpRequestPacket       *request;
    std::string             responseData;
	std::string             responseHeadContent;
    int32_t                 responseCode;
    bool                    succeed;
	bailin::MemBuffer		memBuffer;

} HttpResponsePacket;

/** @brief Singleton that handles asynchrounous http requests
 * Once the request completed, a callback will issued in main thread when it provided during make request
 */
class CCHttpRequestEx : public CCObject
{
public:
    /** Return the shared instance **/
    static CCHttpRequestEx *sharedHttpRequest();
    
    /** Relase the shared instance **/
    static void purgeSharedHttpRequest();
    
    CCHttpRequestEx();
    virtual ~CCHttpRequestEx();
    
    /**
     * Add a get request to task queue
     * @param url url want to get
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return NULL
     */
    void addGetTask(std::string &url, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag=0);
    
    /**
     * Add a get request to task queue
     * @param url the post destination url
     * @param postData data want to post
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return request index
     */
    int addPostTask(std::string &url, std::string &postData, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag = 0);
    
    /**
     * Add a get request to task queue
     * @param url urls want to download
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return NULL
     */
    void addDownloadTask(std::vector<std::string> &urls, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag = 0);
    
	/**
     * 下载内存块
     */
    void addDataGetTask(std::string &url, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag = 0);
    
	/*上传文件*/
	void addPostFileTask(std::string url, std::string postData, CCObject *pTarget, SEL_CallFuncND pSelector, std::string filePath,int nTag = 0 , std::string copyName = "file");

public:
    /**
     * Change the connect timeout
     * @param timeout 
     * @return NULL
     */
    inline void setConnectTimeout(int32_t timeout) {m_connectTimeout = timeout;};
    
    /**
     * Change the download timeout
     * @param timeout
     * @return NULL
     */
    inline void setDownloadTimeout(int32_t timeout) {m_timeout = timeout;};
    
    /**
     * Get connect timeout
     * @return int
     */ 
    inline int32_t getConnectTimeout() {return m_connectTimeout;}
    
    /**
     * Get download timeout
     * @return int
     */
    inline int32_t getDownloadTimeout() {return m_timeout;}
    
private:
    /**
     * Init pthread mutex, semaphore, and create new thread for http requests
     * @return bool
     */
    bool lazyInitThreadSemphore();
    /** Poll function called from main thread to dispatch callbacks when http requests finished **/
    void httpRequestCallback(float delta);
    
private:
    int32_t m_connectTimeout;
    int32_t m_timeout;
    int32_t m_requestIndex;//
   
};

NS_CC_END

#endif //__CCHttpRequest_h

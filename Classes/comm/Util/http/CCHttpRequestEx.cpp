/**************************************************************************** 
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

#include "CCHttpRequestEx.h"
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "cocos2d.h"
#include "support/zip_support/ZipUtils.h"

NS_CC_BEGIN

static pthread_t        s_requestThread;
static pthread_mutex_t  s_requestQueueMutex;
static pthread_mutex_t  s_responseQueueMutex;
static sem_t *          s_pSem = NULL;
static unsigned long    s_asyncRequestCount = 0;
static std::string   semaphoreName ="ccHttpAsync";
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE 1
#else
#define CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE 0
#endif

#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
#define CC_ASYNC_HTTPREQUEST_SEMAPHORE "ccHttpAsync"
static sem_t s_sem;
#else
static sem_t s_sem;
#endif

static bool need_quit = false;

static std::queue<HttpRequestPacket *> *s_requestQueue = NULL;
static std::queue<HttpResponsePacket *> *s_responseQueue = NULL;

static CCHttpRequestEx *_g_singleton_httprequest = NULL;

static char errorBuffer[CURL_ERROR_SIZE];
typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

//callback function used by libcurl for save file
size_t writeFile(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    
    return written;
}

//callback function used by libcurl for collect response data
size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string *str = (std::string *)stream;
    
    size_t sizes = size * nmemb;
    str->append((char *)ptr, sizes);
    
    return sizes;
}

//callback function used by libcurl for collect response data
size_t writeHeadData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string *str = (std::string *)stream;

	size_t sizes = size * nmemb;
	str->append((char *)ptr, sizes);

	return sizes;
}

size_t writeDataMemory(void *ptr, size_t size, size_t nmemb, void *stream)
{
 	bailin::MemBuffer *memBuffer = (bailin::MemBuffer *)stream;
 
 	size_t sizes = size * nmemb;
 	memBuffer->Append((char *)ptr ,sizes );

	return sizes;
}


//Prototypes
void releaseRequestQueue();
bool configureCURL(CURL *handle);
int processGetTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);
int processPostTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode ,write_callback headCallback = NULL ,void *headStream = NULL );
int processPostFileTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);
int processDownloadTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);

void CCHttpRequestEx::purgeSharedHttpRequest()
{
    CC_SAFE_RELEASE_NULL(_g_singleton_httprequest);
	// Release curl global
	curl_global_cleanup();
}

CCHttpRequestEx *CCHttpRequestEx::sharedHttpRequest()
{
    if (_g_singleton_httprequest == NULL) {
        _g_singleton_httprequest = new CCHttpRequestEx();

		// initial curl global
		curl_global_init(CURL_GLOBAL_ALL);
    }
    
    return _g_singleton_httprequest;
}

//Worker thread
static void *requestThread(void *data)
{
    CCThread thread;
    thread.createAutoreleasePool();
    
    HttpRequestPacket *req = NULL;
    
    while (true) {
        //Wait for http request tasks from main thread
        int semWaitRet = sem_wait(s_pSem);
        if (semWaitRet < 0) {
            CCLog("HttpRequest async thread semaphore error: %s\n", strerror(errno));
            break;
        }
        
        std::queue<HttpRequestPacket *> *pQueue = s_requestQueue;
        pthread_mutex_lock(&s_requestQueueMutex); //Get request task from queue
        if (pQueue->empty()) {
            pthread_mutex_unlock(&s_requestQueueMutex);
            
            if (need_quit) {
                break;
            } else {
                continue;
            }
        } else {
            if (need_quit) {
                pthread_mutex_unlock(&s_requestQueueMutex);
                break;
            }
            
            req = pQueue->front();
            pQueue->pop();
            pthread_mutex_unlock(&s_requestQueueMutex);
        }
        
        //Create a response packet and assume it will successed
        HttpResponsePacket *responsePacket = new HttpResponsePacket();
        responsePacket->request = req;
        responsePacket->succeed = true;

        //Process the request
        if (req->reqType == kHttpRequestGet) { //Get Request
            int32_t ret = processGetTask(req, writeData, &responsePacket->responseData, &responsePacket->responseCode);
            if (ret != 0) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
		} 
		else if (req->reqType == kHttpRequestMemoryDownload) { //Get Request
			int32_t ret = processGetTask(req, writeDataMemory, &responsePacket->memBuffer, &responsePacket->responseCode);
			if (ret != 0) {
				responsePacket->succeed = false;
				responsePacket->responseData = errorBuffer;
			}
		} 
		else if (req->reqType == kHttpRequestPost) { //Post Request
            int32_t ret = processPostTask(req, writeDataMemory, &(responsePacket->memBuffer), &responsePacket->responseCode ,writeHeadData , &responsePacket->responseHeadContent );

			//把 memory 转到 string
			//1.先看文件头
			bool isZiped = false;
			if (responsePacket->responseHeadContent.length() > 0)
			{
				if (responsePacket->responseHeadContent.find("Content-Encoding: gzip") != std::string::npos)
				{
					//用 zip 去解压
					isZiped = true;
				}
			}
			
			if (isZiped)
			{
				unsigned char *deflated = NULL;
				int deflatedLen = cocos2d::ZipUtils::ccInflateMemory((unsigned char *)responsePacket->memBuffer.GetBuffer() ,responsePacket->memBuffer.GetBufferLen() , &deflated  );
				if (deflatedLen > 0)
				{
					//解压成功
					CCString* strResult = CCString::createWithData(deflated,deflatedLen );
					responsePacket->responseData = strResult->getCString();
				}
				else
				{
					//直接赋值
					responsePacket->responseData = responsePacket->memBuffer.GetBuffer();
				}
			}
			else
			{
				responsePacket->responseData = responsePacket->memBuffer.GetBuffer();
			}
			


            if (ret != 0) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
        }
		else if (req->reqType == kHttpRequestFileUpLoad) { //Post file Request
			int32_t ret = processPostFileTask(req, writeData, &responsePacket->responseData, &responsePacket->responseCode);
			if (ret != 0) {
				responsePacket->succeed = false;
				responsePacket->responseData = errorBuffer;
			}
		} 
		else if (req->reqType == kHttpRequestDownloadFile) { //Download File Request
            bool fullyDownloaded = true;
            std::vector<std::string>::iterator iter;
            std::string saveFileName;
            std::string needDownload;
            
            for (iter = req->files.begin(); iter != req->files.end(); ++iter) {
                needDownload = *iter;
                std::string::size_type pos = needDownload.rfind("/");
                
                if (pos != std::string::npos) {
                    saveFileName = needDownload.substr(pos + 1);
                } else {
                    saveFileName = needDownload;
                }
                
                //If the download url is http://www.xxx.com/yyy.html
                //The saved file name must be yyy.html
                saveFileName = CCFileUtils::sharedFileUtils()->getWritablePath() + saveFileName;
                FILE *handle = fopen(saveFileName.c_str(), "w+");
                if (!handle) {
                    fullyDownloaded = false;
                    break;
                }
                req->url = needDownload;
                int32_t ret = processDownloadTask(req, writeFile, handle, &responsePacket->responseCode);
                if (handle) {
                    fclose(handle);
                }
                if (ret != 0) {
                    fullyDownloaded = false;
                    break;
                }
            }
            
            //Only consider download task successfully when all the files downloaded
            if (!fullyDownloaded) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
        }
        
        pthread_mutex_lock(&s_responseQueueMutex);
        s_responseQueue->push(responsePacket);
        pthread_mutex_unlock(&s_responseQueueMutex);
    }
    
    //If worker thread received quit signal, clean up un-completed request queue
    releaseRequestQueue();
    
    if (s_pSem != NULL) {
#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
        sem_unlink(semaphoreName.c_str());
        sem_close(s_pSem);
#else
        sem_destroy(s_pSem);
#endif
        
        s_pSem = NULL;
        
        pthread_mutex_destroy(&s_requestQueueMutex);
        pthread_mutex_destroy(&s_responseQueueMutex);
        
        delete s_requestQueue;
        delete s_responseQueue;
    }

    pthread_exit(NULL);
    
    return 0;
}

//Release Http request task queue
void releaseRequestQueue()
{
    pthread_mutex_lock(&s_requestQueueMutex);
    
    int32_t requestQueueSize = s_requestQueue->size();
    if (requestQueueSize > 0) {
        for (int32_t i = 0; i < requestQueueSize; ++i) {
            HttpRequestPacket *packet = s_requestQueue->front();
            s_requestQueue->pop();
            
            delete packet;
        }
        
        s_asyncRequestCount -= requestQueueSize;
    }
    
    pthread_mutex_unlock(&s_requestQueueMutex);
}

//Configure curl's timeout property
bool configureCURL(CURL *handle)
{
    if (!handle) {
        return false;
    }
    
    int32_t code;
    code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errorBuffer);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, CCHttpRequestEx::sharedHttpRequest()->getDownloadTimeout());
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, CCHttpRequestEx::sharedHttpRequest()->getConnectTimeout());
    if (code != CURLE_OK) {
        return false;
    }

	std::string filePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	filePath += "cookies.txt";
	code = curl_easy_setopt(handle, CURLOPT_COOKIEFILE, filePath.c_str());
	if (code != CURLE_OK)
	{
		return false;
	}

	code = curl_easy_setopt(handle, CURLOPT_COOKIEJAR, filePath.c_str());
	if (code != CURLE_OK)
	{
		return false;
	}
		
	// Shutdown the alarm
	code = curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
	if (code != CURLE_OK)
	{
		return false;
	}
	
    return true;
}

//Process Get Request
int processGetTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, task->url.c_str());
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_perform(curl);
        if (code != CURLE_OK) {
            break;
        }
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode); 
        if (code != CURLE_OK || *responseCode / 100 != 2) {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    return (code == CURLE_OK ? 0 : 1);
}

//Process POST Request
int processPostTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode ,write_callback headCallback ,void *headStream)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, task->url.c_str());
        if (code != CURLE_OK) {
            break;
        }

        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK) {
            break;
        }

		//设置写文件头
		if (headStream != NULL && headCallback != NULL)
		{
			code = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headCallback);
			if (code != CURLE_OK) {
				break;
			}
			code = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, headStream);
			if (code != CURLE_OK) {
				break;
			}
		}
		

        code = curl_easy_setopt(curl, CURLOPT_POST, 1);
        if (code != CURLE_OK) {
            break;
        }

        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, task->reqData.c_str());
        if (code != CURLE_OK) {
            break;
        }

//		if (task->reqType == kHttpRequestFileUpLoad)
// 		{
// 			struct curl_httppost *post=NULL;
// 			struct curl_httppost *last=NULL;
// 			struct curl_slist *headerlist=NULL;
// 
// 			CURLFORMcode code = curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, task->reqUploadFileName, CURLFORM_END);
// 			if (code != CURL_FORMADD_OK) {
// 				break;
// 			}
// 
// 			code = curl_formadd(&post, &last, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "upload", CURLFORM_END);
// 			if (code != CURL_FORMADD_OK) {
// 				break;
// 			}
// 		}

// 		//gzip
// 		code = curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
// 		if (code != CURLE_OK) {
// 			break;
// 		}
// 		code = curl_easy_setopt (curl , CURLOPT_ENCODING, "gzip" ); //设置为客户端支持gzip压缩
// 		if (code != CURLE_OK) {
// 			break;
// 		}
// 		code = curl_easy_setopt (curl , CURLOPT_ENCODING, "gzip" );

		code = curl_easy_perform(curl);
		if (code != CURLE_OK) {
			break;
		}

// 		if(1)//查看内容类型
// 		{
// 			char *contype;
// 			code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contype);
// 
// 			if((CURLE_OK != code))
// 			{
// 				break;
// 			}
// 			CCLOG("get contnen type = %s",contype);
// 
// 		}
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode); 
        if (code != CURLE_OK || *responseCode / 100 != 2) {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    return (code == CURLE_OK ? 0 : 1);    
}

//Process POST Request
int processPostFileTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
	//
	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";
	/* Fill in the file upload field */
	const char* pFileName = task->reqUploadFileName.c_str();
	const char* pFileKeyName = "";
	if (task->str_copyName.length() == 0)
	{
		pFileKeyName = "file";
	}
	else
	{
		pFileKeyName = task->str_copyName.c_str();
	}

	CURLFORMcode errorCode = curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, pFileKeyName,
		CURLFORM_FILE, pFileName,
		CURLFORM_END);
	if (errorCode != CURL_FORMADD_OK)
	{
		return 0;
	}

	/* Fill in the filename field */
	errorCode = curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "filename",
		CURLFORM_COPYCONTENTS, "myhead.jpg",
		CURLFORM_END);
	if (errorCode != CURL_FORMADD_OK)
	{
		return 0;
	}

	CCArray* aryPost = EasyUseApi::StringSepratedByString(task->reqData.c_str() , "&");
	for (int i=0;aryPost && i<aryPost->count();i++ )
	{
		CCString* strPostField = (CCString*)(aryPost->objectAtIndex(i));
		if (strPostField)
		{
			CCArray* aryPostField = EasyUseApi::StringSepratedByString(strPostField->getCString() , "=");
			if (aryPostField->count() == 2)
			{
				CCString* str1 = (CCString*)(aryPostField->objectAtIndex(0));
				CCString* str2 = (CCString*)(aryPostField->objectAtIndex(1));
				errorCode = curl_formadd(&formpost,
					&lastptr,
					CURLFORM_COPYNAME, str1->getCString(),
					CURLFORM_COPYCONTENTS, str2->getCString(),
					CURLFORM_END);
				if (errorCode != CURL_FORMADD_OK)
				{
					CCLOG("Failed to add post field %s=%s",str1->getCString(),str2->getCString());
					return 0;
				}

			}
			
		}
		

	}


	/* Fill in the submit field too, even if this is rarely needed */
	errorCode = curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "send",
		CURLFORM_END);
	if (errorCode != CURL_FORMADD_OK)
	{
		return 0;
	}

	/* initalize custom header list (stating that Expect: 100-continue is not wanted */
	headerlist = curl_slist_append(headerlist, buf);


	CURLcode code = CURL_LAST;
	CURL *curl = curl_easy_init();

	do {
		if (!configureCURL(curl)) {
			break;
		}

		code = curl_easy_setopt(curl, CURLOPT_URL, task->url.c_str());
		if (code != CURLE_OK) {
			break;
		}

		code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		if (code != CURLE_OK) {
			break;
		}
		code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
		if (code != CURLE_OK) {
			break;
		}

		/* only disable 100-continue header if explicitly requested */
		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


		code = curl_easy_perform(curl);
		if (code != CURLE_OK) {
			break;
		}

		code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode); 
		if (code != CURLE_OK || *responseCode / 100 != 2) {
			code = CURLE_HTTP_RETURNED_ERROR;
		}
	} while (0);
	if (curl) {
		curl_easy_cleanup(curl);
	}

	return (code == CURLE_OK ? 0 : 1);    
}


//Process Download Request
int processDownloadTask(HttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
    return processGetTask(task, callback, stream, responseCode);
}

CCHttpRequestEx::CCHttpRequestEx()
{
    m_timeout = 30;
    m_connectTimeout = 5;
	m_requestIndex = 0;

#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
    CCString* str = CCString::createWithFormat("%s%d",CC_ASYNC_HTTPREQUEST_SEMAPHORE,arc4random()%1000);
    if (str)
    {
        semaphoreName = str->getCString();
}
    else
        semaphoreName =CC_ASYNC_HTTPREQUEST_SEMAPHORE;
#endif

}

CCHttpRequestEx::~CCHttpRequestEx()
{
    need_quit = true;
    
    if (s_pSem != NULL) {
        sem_post(s_pSem);
    }
}

//Lazy create semaphore & mutex & thread
bool CCHttpRequestEx::lazyInitThreadSemphore()
{
    if (s_pSem != NULL) {
        return true;
    } else {
#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
        
        s_pSem = sem_open(semaphoreName.c_str(), O_CREAT, 0644, 0);
        if (s_pSem == SEM_FAILED) {
            
          
            CCLog("Open HttpRequest Semaphore failed");
            s_pSem = NULL;
            return false;
        }
#else
        int semRet = sem_init(&s_sem, 0, 0);
        if (semRet < 0) {
            CCLog("Init HttpRequest Semaphore failed");
            return false;
        }
        
        s_pSem = &s_sem;
#endif

#ifdef DEBUG
		s_initTimes++;
		std::string httpLogFilePath = CCFileUtils::sharedFileUtils()->getWritablePath();
		httpLogFilePath += "httpErrorLog.txt";

		time_t tt = time(NULL);
//		const char* pszTime = ctime(&tt);
		struct tm* t = localtime(&tt);
		char szTime[30] = {0};
		sprintf(szTime, "[%d-%02d-%02d %02d:%02d:%02d] ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		std::string strLog = szTime;
		strLog += CCString::createWithFormat("s_initTimes %d\n", s_initTimes)->m_sString;
		FILE* file = fopen(httpLogFilePath.c_str(), "a+");
		fwrite(strLog.c_str(), 1, strLog.length(), file);
		fclose(file);
#endif
        s_requestQueue = new std::queue<HttpRequestPacket *>();
        s_responseQueue = new std::queue<HttpResponsePacket *>();
        
        pthread_mutex_init(&s_requestQueueMutex, NULL);
        pthread_mutex_init(&s_responseQueueMutex, NULL);
        
        pthread_create(&s_requestThread, NULL, requestThread, NULL);
        pthread_detach(s_requestThread);
        
        need_quit = false;
    }
    
    return true;
}

//下载内存块
void CCHttpRequestEx::addDataGetTask(std::string &url, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag)
{
	bool init = lazyInitThreadSemphore();
	if (!init) {
		return;
	}

	if (0 == s_asyncRequestCount) {
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this, 0, false);
	}

	++s_asyncRequestCount;
	if (pTarget) {
		pTarget->retain();
	}

	HttpRequestPacket *packet = new HttpRequestPacket();
	packet->reqType = kHttpRequestMemoryDownload;
	packet->url = url;
	packet->pTarget = pTarget;
	packet->pSelector = pSelector;
	packet->nTag = nTag;
	pthread_mutex_lock(&s_requestQueueMutex);
	s_requestQueue->push(packet);
	pthread_mutex_unlock(&s_requestQueueMutex);

	//Notify thread start to work
	sem_post(s_pSem);
}


//Add a get task to queue
void CCHttpRequestEx::addGetTask(std::string &url, cocos2d::CCObject *pTarget, SEL_CallFuncND pSelector,int nTag)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return;
    }
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this, 0, false);
    }
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }
    
    HttpRequestPacket *packet = new HttpRequestPacket();
    packet->reqType = kHttpRequestGet;
    packet->url = url;
    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->nTag = nTag;
    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    //Notify thread start to work
    sem_post(s_pSem);
}

/*上传文件*/
void CCHttpRequestEx::addPostFileTask(std::string url, std::string postData, CCObject *pTarget, SEL_CallFuncND pSelector, std::string filePath,int nTag , std::string copyName )
{

	bool init = lazyInitThreadSemphore();
	if (!init) {
		return;
	}

	if (0 == s_asyncRequestCount) {
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this, 0, false);
	}

	++s_asyncRequestCount;
	if (pTarget) {
		pTarget->retain();
	}

	HttpRequestPacket *packet = new HttpRequestPacket();
	packet->reqType = kHttpRequestFileUpLoad;
	packet->str_copyName = copyName;
	packet->url = url;
	packet->reqData = postData;
	packet->pTarget = pTarget;
	packet->pSelector = pSelector;
	packet->nTag = nTag;
	packet->reqUploadFileName = filePath;
	pthread_mutex_lock(&s_requestQueueMutex);
	s_requestQueue->push(packet);
	pthread_mutex_unlock(&s_requestQueueMutex);

	//Notify thread start to work
	sem_post(s_pSem);
}

//Add a post task to queue
int  CCHttpRequestEx::addPostTask(std::string &url, std::string &postData, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return 0;
    }
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this, 0, false);
    }
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }

	m_requestIndex++;
    
    HttpRequestPacket *packet = new HttpRequestPacket();
    packet->reqType = kHttpRequestPost;
    packet->url = url;
    packet->reqData = postData;
    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->nTag = nTag;
	packet->nRequestIndex = m_requestIndex;

    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    //Notify thread start to work
    sem_post(s_pSem);

	return m_requestIndex;
}

//Add a download task to queue
void CCHttpRequestEx::addDownloadTask(std::vector<std::string> &urls, CCObject *pTarget, SEL_CallFuncND pSelector,int nTag)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return;
    }
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this, 0, false);
    }
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }
    
    HttpRequestPacket *packet = new HttpRequestPacket();
    packet->reqType = kHttpRequestDownloadFile;
    packet->files = urls;
    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->nTag = nTag;
    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    sem_post(s_pSem);    
}

//Poll and notify main thread if responses exists in queue
void CCHttpRequestEx::httpRequestCallback(float delta)
{
    std::queue<HttpResponsePacket *> *pQueue = s_responseQueue;
    
    pthread_mutex_lock(&s_responseQueueMutex);
    if (pQueue->empty()) {
        pthread_mutex_unlock(&s_responseQueueMutex);
    } else {
        HttpResponsePacket *packet = pQueue->front();
        pQueue->pop();
  
        pthread_mutex_unlock(&s_responseQueueMutex);
        
        --s_asyncRequestCount;

#ifdef COCOS2D_DEBUG
		if (s_asyncRequestCount < 0)
		{
			std::string httpLogFilePath = CCFileUtils::sharedFileUtils()->getWritablePath();
			httpLogFilePath += "httpErrorLog.txt";
			time_t tt = time(NULL);
			struct tm* t = localtime(&tt);
			char szTime[30] = {0};
			sprintf(szTime, "[%d-%02d-%02d %02d:%02d:%02d] ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			std::string strLog = szTime;
			strLog += "s_asyncRequestCount < 0\n";
			FILE* file = fopen(httpLogFilePath.c_str(), "a+");
			fwrite(strLog.c_str(), 1, strLog.length(), file);
			fclose(file);
		}
#endif
        if (0 == s_asyncRequestCount) {
            CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(CCHttpRequestEx::httpRequestCallback), this);
        }
        
        HttpRequestPacket *orgRequest = packet->request;
        CCObject *pTarget = orgRequest->pTarget;
        SEL_CallFuncND pSelector = orgRequest->pSelector;

        if (pTarget && pSelector) {
            (pTarget->*pSelector)((CCNode *)this, packet);
            
            pTarget->release();
        }
        
        delete orgRequest;
        delete packet;
    }
}

NS_CC_END



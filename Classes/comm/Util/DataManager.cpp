#include "DataManager.h"
#include "CCJSONConverter.h"
#include "crypto.h"
#include "userDataMacro.h"
#include "StringUtil.h"

#if (CC_PLATFORM_WIN32 != CC_TARGET_PLATFORM)
#	include "NDKHelper.h"
#endif

#if (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
#	include <jni.h> 
#	include "platform/android/jni/JniHelper.h" 
#	include <android/log.h>
#	define JNI_CLASS "com/bailin/BailinActivity"
#elif (CC_PLATFORM_IOS == CC_TARGET_PLATFORM)
#import <AdSupport/ASIdentifierManager.h>
#import "OpenUDID.h"
// #import "KeychainItemWrapper.h"
#endif

#ifdef ENABLE_UMENG_DATA
// 加载友盟操作的头文件
#   include "MobClickCpp.h"
#endif

USING_NS_CC;

// 异或key
const char SECRECT_KEY[] = "20140419";

// 帐号和密码的异或key （密码存的是MD5的值，所以不暂时不用）
// const char ACCOUNT_KEY[] = "20140501";

NS_BAILIN_UTIL_BEGIN

DataManagerUtil::DataManagerUtil()
{
	m_pDictionaryGlobalData = NULL;

    // m_pDictionarySecurityData = CCDictionary::create();
    // CC_SAFE_RETAIN(m_pDictionaryGlobalData);
	m_pDictionaryStringData = getLocaleStringData();
	m_pDictionaryGlobalData = getGlobalData();
}

DataManagerUtil::~DataManagerUtil()
{
	CC_SAFE_RELEASE_NULL(m_pDictionaryStringData);
	CC_SAFE_RELEASE_NULL(m_pDictionaryGlobalData);
	// CC_SAFE_RELEASE_NULL(m_pDictionarySecurityData);
}

static DataManagerUtil* g_sharedDataManager = NULL;
 DataManagerUtil* DataManagerUtil::sharedDataManagerUtil()
{
	if (!g_sharedDataManager)
	{
		g_sharedDataManager = new DataManagerUtil();
#if 0
		std::string strAppString = "";
		// 底层获得对应的应用信息
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		do
		{
			// 定义Jni函数信息结构体 
			JniMethodInfo minfo;

			bool bHaveMethod = JniHelper::getStaticMethodInfo(minfo, 
				JNI_CLASS,  //类的路径
				"getAppInfo",   //方法名 
				"()Ljava/lang/String;");   //括号里的是参数，后面的是返回值

			jstring jstr;
			if (bHaveMethod)
			{
				jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);

				char* pszAppInfoString = strdup(minfo.env->GetStringUTFChars(jstr, 0));
				// CCLOG("getAppProperty--------->jni-java-getAppVersionCode function done, versionName:%s", pszAppInfoString);
				strAppString = pszAppInfoString;
				minfo.env->ReleaseStringUTFChars(jstr, pszAppInfoString);
			}
		} while (0);

		g_sharedDataManager->SetGlobalDataString(DATA_KEY_PLATFORM, "2");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		g_sharedDataManager->SetGlobalDataString(DATA_KEY_PLATFORM, "1");
        NSString *adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
        const char* idfa = NULL;
        if (adId)
        {
           idfa = [adId UTF8String];
        }
        
        NSDictionary* dict = [[NSBundle mainBundle ] infoDictionary];
        const char* ver = NULL;
        if (dict)
        {
            NSString* version = [dict objectForKey:@"CFBundleVersion"];
            ver = [version UTF8String];
        }
        
//        NSString* openUdid = [OpenUDID value];
        const char* udid = NULL;
        KeychainItemWrapper *wrapper = [[KeychainItemWrapper alloc] initWithIdentifier:@"_com.provemyself.treasure"
                                                                           accessGroup:nil];
        NSString * pUDID = [wrapper objectForKey:(id)kSecAttrService];
        NSLog(@"openUdid111 = %@",pUDID);
        NSString* openUdid = pUDID;
        if(openUdid == NULL)
        {
            openUdid = [OpenUDID value];
        }
        
        if(openUdid != NULL)
        {
            NSLog(@"openUdid222 = %@",openUdid);
            [wrapper setObject:openUdid forKey:(id)kSecAttrService];
             udid = [openUdid UTF8String];
            [wrapper release];
        }
//        udid = "122223345";
        strAppString = CCString::createWithFormat("{\"version\":\"%s\",\"UDID\":\"%s\",\"IDFA\":\"%s\"}",ver,udid,idfa)->getCString();
        
#else
		// WIN32 上默认参数设置
		strAppString = "{\"version\":\"1.0.1\",\"UDID\":\"1234567890\"}";
		g_sharedDataManager->SetGlobalDataString(DATA_KEY_PLATFORM, "3");
#endif
		CCLOG("appInfo = %s", strAppString.c_str());

		// 把JSON数据转换成CCDictionary
		CCDictionary* pData = CCJSONConverter::sharedConverter()->dictionaryFrom(strAppString.c_str());
		g_sharedDataManager->SetGlobalDataString(DATA_KEY_VERSION, pData->valueForKey("version")->getCString());
		g_sharedDataManager->SetGlobalDataString(DATA_KEY_UDID, pData->valueForKey("UDID")->getCString());
        g_sharedDataManager->SetGlobalDataString(DATA_KEY_IDFA, pData->valueForKey("IDFA")->getCString());
#endif
	}

	return g_sharedDataManager;
}

void DataManagerUtil::purgeSharedDataManagerUtil()
{
	CC_SAFE_DELETE(g_sharedDataManager);
}

cocos2d::CCDictionary* DataManagerUtil::getGlobalData()
{
	if (!m_pDictionaryGlobalData)
	{
		m_pDictionaryGlobalData = CCDictionary::create();
		m_pDictionaryGlobalData->retain();
	}

	return m_pDictionaryGlobalData;
}


// 得到当前语言代码的对应字符串表示
const char* getCurrentLanguageCode(ccLanguageType languange)
{
	const char* pszLanguange = NULL;
	switch(languange)
	{
	case kLanguageEnglish:
		pszLanguange = "en";
		break;
	case kLanguageChinese:
		pszLanguange = "zh";
		break;
	case kLanguageFrench:
		pszLanguange = "fr";
		break;
	case kLanguageItalian:
		pszLanguange = "it";
		break;
	case kLanguageGerman:
		pszLanguange = "de";
		break;
	case kLanguageSpanish:
		pszLanguange = "es";
		break;
	case kLanguageRussian:
		pszLanguange = "ru";
		break;
	case kLanguageKorean:
		pszLanguange = "ko";
		break;
	case kLanguageJapanese:
		pszLanguange = "ja";
		break;
	case kLanguageHungarian:
		pszLanguange = "hu";
		break;
	case kLanguagePortuguese:
		pszLanguange = "pt";
		break;
	case kLanguageArabic:
		pszLanguange = "ar";
		break;
	}

	return pszLanguange;
}

// 获取当前语言对应的
const char* getLocalizableFileName()
{
	const char * languageCode = getCurrentLanguageCode(CCApplication::sharedApplication()->getCurrentLanguage());
	if (languageCode == NULL)
	{
		// 默认英文版本
		languageCode = "en";
	}

	CCString* strFile = CCString::createWithFormat("localizableString/stringData_%s.plist", languageCode);
	CCLOG("getLocalizableFileName %s", languageCode);

	if (strFile != NULL)
	{
		std::string path =  CCFileUtils::sharedFileUtils()->fullPathForFilename(strFile->getCString());

		if (CCFileUtils::sharedFileUtils()->isFileExist(path.c_str()))
		{
			return strFile->getCString();
		}
	}

	// 默认中文语言包
	return "localizableString/stringData_en.plist";
}

cocos2d::CCDictionary* DataManagerUtil::getLocaleStringData()
{
	CCDictionary* pDictData = NULL;
	std::string localString = getLocalizableFileName();
    std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(localString.c_str());
    if (!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath.c_str()))
        CCLOG("%s is not Exist!!!", strFullPath.c_str());
	pDictData = CCDictionary::createWithContentsOfFile(strFullPath.c_str());
	if (pDictData)
	{
		pDictData->retain();
	}
	return pDictData;
}

std::string DataManagerUtil::GetUmengOnlineConfig(const char*pkey)
{
    std::string strValue = "";
#ifdef ENABLE_UMENG_DATA
    // 判断下是否有友盟数据
    strValue = MobClickCpp::getConfigParams(pkey);
#endif
    
    return strValue;
}

const char* DataManagerUtil::GetUTF8StringInDictionary( const char* pszSection, const char* pszKey )
{
	if (!m_pDictionaryStringData)
	{
		m_pDictionaryStringData = getLocaleStringData();
	}

	CCAssert(pszSection != NULL, "seciton null" );
	CCAssert(pszKey != NULL, "key null" );

	if (!m_pDictionaryStringData)
	{
		return "";
	}
	if (pszSection ==NULL || pszKey== NULL) {
		return "";
	}
	// 得到对应的section
	CCDictionary* pDictSection = dynamic_cast<CCDictionary*>(m_pDictionaryStringData->objectForKey(pszSection));
	if (pDictSection)
	{
		CCString* pStrValue = dynamic_cast<CCString*>(pDictSection->objectForKey(pszKey));
		if (pStrValue)
		{
#ifdef COCOS2D_DEBUG
			CCLOG("%s %s", pszKey, pStrValue->getCString());
#endif
			return pStrValue->getCString();
		}
		else
		{
			CCLOG("key %s value null", pszKey);
#ifdef COCOS2D_DEBUG
			CCAssert(pStrValue!=NULL, "key value null");
#endif
		}
	}
	else
	{

		CCLOG("pszSection %s null", pszSection);
#ifdef COCOS2D_DEBUG
		CCAssert(pDictSection!=NULL, "section value null");
#endif
	}

	return "";
}

const char* DataManagerUtil::GetUTF8StringInDictionaryForServer( const char* pszKey )
{
	CCDictionary*  pDictStringData=NULL;
	if (!pDictStringData)
	{
		const char * languageCode = getCurrentLanguageCode(CCApplication::sharedApplication()->getCurrentLanguage());
		if (languageCode == NULL)
		{
			languageCode = "en";
		}
		std::string localString = "localizableString/serverData_en.plist";
		CCString* strFile = CCString::createWithFormat("localizableString/serverData_%s.plist", languageCode);

		if (strFile!=NULL)
		{
			std::string path =  CCFileUtils::sharedFileUtils()->fullPathForFilename(strFile->getCString());

			if (CCFileUtils::sharedFileUtils()->isFileExist(path.c_str()))
			{
				localString = strFile->getCString();
			}
		}

		// 这里判断下文件是否存在，如果不存在就跳过，使用服务器返回的数据显示
		if (CCFileUtils::sharedFileUtils()->isFileExist(localString))
		{
			pDictStringData = CCDictionary::createWithContentsOfFile(localString.c_str());
		}

	}
	if (pDictStringData == NULL) {
		return "";
	}

	CCAssert(pszKey != NULL, "key null" );
	if (!m_pDictionaryStringData)
	{
		return "";
	}
	if (   pszKey== NULL)
	{
		return "";
	}

	// 得到对应的section
	CCDictionary* pDictSection = dynamic_cast<CCDictionary*>(pDictStringData->objectForKey("commonServer"));
	if (pDictSection)
	{
		CCString* pStrValue = dynamic_cast<CCString*>(pDictSection->objectForKey(pszKey));
		if (pStrValue)
		{
#ifdef COCOS2D_DEBUG
			CCLOG("%s %s", pszKey, pStrValue->getCString());
#endif
			return pStrValue->getCString();
		}
		else
		{
			CCLOG("key %s value null", pszKey);
#ifdef COCOS2D_DEBUG
			CCAssert(pStrValue!=NULL, "key value null");
#endif
		}
	}
	else
	{

#ifdef COCOS2D_DEBUG
		CCAssert(pDictSection!=NULL, "section value null");
#endif
	}

	return "";
}

long DataManagerUtil::GetLongValueInDictionary( const char* pszSection, const char* pszKey )
{
	long lRet = 0;
	if (!m_pDictionaryStringData)
	{
		m_pDictionaryStringData = getLocaleStringData();
	}

	if (!m_pDictionaryStringData)
	{
		return lRet;
	}

	// 得到对应的section
	CCDictionary* pDictSection = dynamic_cast<CCDictionary*>(m_pDictionaryStringData->objectForKey(pszSection));
	if (pDictSection)
	{
		CCObject* pObj = pDictSection->objectForKey(pszKey);
		if (pObj)
		{
			lRet = GetLongValueWithObject(pObj);
		}
	}

	return lRet;
}

long DataManagerUtil::GetGlobalDataLong( const char* pszKey )
{
	CCDictionary* pDict = getGlobalData();
	if (pDict)
	{
		CCInteger* pInt = dynamic_cast<CCInteger*>(pDict->objectForKey(pszKey));
		if (pInt)
		{
			return pInt->getValue();
		}
	}
	return 0;
}

void DataManagerUtil::SetGlobalDataLong( const char* pszKey, long lValue )
{
	CCDictionary* pDict = getGlobalData();
	if (pDict)
	{
		pDict->setObject(CCInteger::create(lValue), pszKey);
	}
}

const char* DataManagerUtil::GetGlobalDataString( const char* pszKey )
{
	CCDictionary* pDict = getGlobalData();
	if (pDict)
	{
		const CCString* pValue = dynamic_cast<CCString*>(pDict->objectForKey(pszKey));
		if (pValue)
		{
			return pValue->getCString();
		}
	}

	return "";
}

void DataManagerUtil::SetGlobalDataString( const char* pszKey, const char* pszValue )
{
	CCDictionary* pDict = getGlobalData();
	if (pDict)
	{
		pDict->setObject(ccs(pszValue), pszKey);
	}
}

void DataManagerUtil::SetGlobalDataObject(const char *pszKey, cocos2d::CCObject *pData)
{
    CCDictionary* pDict = getGlobalData();
    if (pDict)
    {
        pDict->setObject(pData, pszKey);
    }
}

CCObject* DataManagerUtil::GetGlobalDataObject(const char *pszKey)
{
    CCObject* pRet = NULL;
    CCDictionary* pDict = getGlobalData();
    if (pDict)
    {
        pRet = pDict->objectForKey(pszKey);
    }
    return pRet;
}

long DataManagerUtil::GetLongValueWithObject( cocos2d::CCObject* pObj )
{
	long nRet =0;
	if (pObj) {
		if (typeid(*pObj)  == typeid(CCString) )
		{
			CCString* pStr = (CCString*) pObj;
			nRet = atol(pStr->getCString());
			// nRet = pStr->intValue();
		}
		else  if (typeid(*pObj)  == typeid(CCNumber))
		{
			CCNumber* pStr = (CCNumber*) pObj;
			nRet = (long)pStr->getDoubleValue();
		}
	}

	return nRet;
}

std::string DataManagerUtil::ReadDataFromLocal( const std::string& key )
{
	std::string retStr = "";
	if (key.empty())
	{
		return retStr;
	}

	retStr = CCUserDefault::sharedUserDefault()->getStringForKey(key.c_str(), "");
	retStr = base64_decode(retStr);
	retStr = GetEncryptString(retStr);

	return retStr;
}

bool DataManagerUtil::WriteDataToLocal( const std::string& key, const std::string& value )
{
	if (value.empty())
	{
		return false;
	}

	std::string strEncryptValue = GetEncryptString(value);
	std::string base64Value = base64_encode((const unsigned char*)strEncryptValue.c_str(), strEncryptValue.length());
	CCUserDefault::sharedUserDefault()->setStringForKey(key.c_str(), base64Value);
	CCUserDefault::sharedUserDefault()->flush();

	return true;
}

std::string DataManagerUtil::GetEncryptString( const std::string& str )
{
	return crypto::getXORString(SECRECT_KEY, str);
}

float DataManagerUtil::GetFloatValueWithObject( cocos2d::CCObject* pObj )
{
	float fRet = 0.0f;
	if (pObj) 
	{
		//   CCLOG("%s %s",typeid(*pObj) ,typeid(CCString) );
		if (typeid(*pObj)  == typeid(CCString) )
		{
			fRet = ((CCString*)pObj)->floatValue();
		}
		else if (typeid(*pObj)  == typeid(CCInteger) )
		{
			fRet = ((CCInteger*)pObj)->getValue();
		}
		else if(typeid(*pObj) == typeid(CCNumber))
		{
			fRet = ((CCNumber*)pObj)->getDoubleValue();
		}
	}

	return fRet;
}

double DataManagerUtil::GetDoubleValueWithObject( cocos2d::CCObject* pObj )
{
	double dRet =0;
	if (pObj)
    {
		//   CCLOG("%s %s",typeid(*pObj) ,typeid(CCString) );
		if (typeid(*pObj)  == typeid(CCString) )
		{
			dRet = ((CCString*)pObj)->doubleValue();
		}
		else if (typeid(*pObj)  == typeid(CCInteger) )
		{
			dRet = ((CCInteger*)pObj)->getValue();
		}
		else if(typeid(*pObj) == typeid(CCNumber))
		{
			dRet = ((CCNumber*)pObj)->getDoubleValue();
		}
	}

	return dRet;
}

long long DataManagerUtil::GetLongLongValueWithObject(CCObject* pObj)
{
	long long nRet =0;
	if (pObj)
    {
		if (typeid(*pObj)  == typeid(CCNumber) )
		{
			CCNumber* pStr = (CCNumber*) pObj;
			// 这里注意精度丢失问题
			nRet = (long long)pStr->getDoubleValue();
		}
		if (typeid(*pObj)  == typeid(CCString) )
		{
			CCString* pStr = (CCString*) pObj;

#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
			nRet = _atoi64(pStr->getCString());
#else
			nRet = atoll(pStr->getCString());
#endif
		}

	}

	return nRet;
}

void DataManagerUtil::SendNDKMessages( const char* pszNativeFuncName, cocos2d::CCDictionary* pData )
{
#if (CC_PLATFORM_WIN32 != CC_TARGET_PLATFORM)
	SendMessageWithParams(pszNativeFuncName, pData);
#endif
}

/*
template<typename T>
bool DataManagerUtil::CheckSecurityData(const char *pszKey, T data)
{
    unsigned long nCrc32Check = 0;
    SecurityMap::iterator iter = m_mapSecurityData.find(string(pszKey));
    if(iter != m_mapSecurityData.end())
    {
        // CCLOG("key:[%s], value:[%lu]", iter->first.c_str(), iter->second);
        nCrc32Check = iter->second;
    }
    
    unsigned long nCrc32 = crypto::Crc32(&data, sizeof(T));
    
    return nCrc32 == nCrc32Check;
}

template <typename T>
bool DataManagerUtil::SetSecurityData(const char *pszKey, T *pData, T addData)
{
    bool bRet = CheckSecurityData(pszKey, *pData);
    
    if (bRet)
    {
        // 附加值
        *pData += addData;
        
        // 回填校验值
        unsigned long nCheckCode = crypto::Crc32(pData, sizeof(T));
        // m_pDictionarySecurityData->setObject(CCString::createWithFormat("%lu", nCheckCode), pszKey);
        m_mapSecurityData[string(pszKey)] = nCheckCode;
    }
    
    return bRet;
}
//*/

//*
bool DataManagerUtil::CheckSecurityData(const char *pszKey, long lData)
{
    // CCString* pCheckCode = dynamic_cast<CCString*>(m_pDictionarySecurityData->objectForKey(pszKey));
    
    unsigned long nCrc32Check = 0;
    SecurityMap::iterator iter = m_mapSecurityData.find(string(pszKey));
    if(iter != m_mapSecurityData.end())
    {
        // CCLOG("key:[%s], value:[%lu]", iter->first.c_str(), iter->second);
        nCrc32Check = iter->second;
    }
    
    unsigned long nCrc32 = crypto::Crc32(&lData, sizeof(lData));
    
    return nCrc32 == nCrc32Check;
}

bool DataManagerUtil::CheckSecurityData(const char *pszKey, unsigned long ulData)
{
    // CCString* pCheckCode = dynamic_cast<CCString*>(m_pDictionarySecurityData->objectForKey(pszKey));
    
    unsigned long nCrc32Check = 0;
    SecurityMap::iterator iter = m_mapSecurityData.find(string(pszKey));
    if(iter != m_mapSecurityData.end())
    {
        // CCLOG("key:[%s], value:[%lu]", iter->first.c_str(), iter->second);
        nCrc32Check = iter->second;
    }
    
    unsigned long nCrc32 = crypto::Crc32(&ulData, sizeof(ulData));
    
    return nCrc32 == nCrc32Check;
}

bool DataManagerUtil::CheckSecurityData(const char *pszKey, long long llData)
{
    
    unsigned long nCrc32Check = 0;
    SecurityMap::iterator iter = m_mapSecurityData.find(string(pszKey));
    if(iter != m_mapSecurityData.end())
    {
        nCrc32Check = iter->second;
    }
    
    unsigned long nCrc32 = crypto::Crc32(&llData, sizeof(llData));
    
    return nCrc32 == nCrc32Check;
}

bool DataManagerUtil::SetSecurityData(const char *pszKey, long *plData, long lAddData)
{
    bool bRet = CheckSecurityData(pszKey, *plData);
    
    if (bRet)
    {
        // 附加值
        *plData += lAddData;
        
        // 回填校验值
        unsigned long nCheckCode = crypto::Crc32(plData, sizeof(long));
        // m_pDictionarySecurityData->setObject(CCString::createWithFormat("%lu", nCheckCode), pszKey);
        m_mapSecurityData[string(pszKey)] = nCheckCode;
    }
    
    return bRet;
}

bool DataManagerUtil::SetSecurityData(const char *pszKey, unsigned long *pulData, unsigned long ulAddData)
{
    bool bRet = CheckSecurityData(pszKey, *pulData);
    
    if (bRet)
    {
        // 附加值
        *pulData += ulAddData;
        
        // 回填校验值
        unsigned long nCheckCode = crypto::Crc32(pulData, sizeof(unsigned long));
        // m_pDictionarySecurityData->setObject(CCString::createWithFormat("%lu", nCheckCode), pszKey);
        m_mapSecurityData[string(pszKey)] = nCheckCode;
    }
    
    return bRet;
}

bool DataManagerUtil::SetSecurityData(const char *pszKey, long long *pllData, long long llAddData)
{
    bool bRet = CheckSecurityData(pszKey, *pllData);
    
    if (bRet)
    {
        // 附加值
        *pllData += llAddData;
        
        // 回填校验值
        unsigned long nCheckCode = crypto::Crc32(pllData, sizeof(long long));
        // m_pDictionarySecurityData->setObject(CCString::createWithFormat("%lu", nCheckCode), pszKey);
        m_mapSecurityData[string(pszKey)] = nCheckCode;
    }
    
    return bRet;
}
//*/

void DataManagerUtil::SetSecurityCode(const char *pszKey, unsigned long nCode)
{
    m_mapSecurityData[string(pszKey)] = nCode;
}

std::string DataManagerUtil::ReadDataWithChecksum(const char *pszKey)
{
    std::string strRet = "";
    if (IsCheckKeyDataValidate(pszKey))
    {
        strRet = CCUserDefault::sharedUserDefault()->getStringForKey(pszKey);
    }
    return strRet;
}

void DataManagerUtil::WriteDataWithChecksum(const char *pszKey, const char* pszValue)
{
    // 写入数据，顺带写入校验码
    CCUserDefault::sharedUserDefault()->setStringForKey(pszKey, pszValue);
    
    // 校验Key
    std::string strCheckKey = pszKey;
    strCheckKey += "Checksum";
    std::stringstream ss;
    // 顺便加上自己的key，使得每个校验值的值都是动态的，即相同的校验值对应的不是同一个值
    ss << pszValue << SECRECT_KEY << pszKey;
    std::string strCheckData = ss.str();
    // 计算数据的校验码
    std::string strCheckSum = crypto::MD5(strCheckData.c_str());
    
    // 写入校验数据
    CCUserDefault::sharedUserDefault()->setStringForKey(strCheckKey.c_str(), strCheckSum);
}

bool DataManagerUtil::IsCheckKeyDataValidate(const char *pszKey)
{
    bool bRet = false;
    std::string strValue = CCUserDefault::sharedUserDefault()->getStringForKey(pszKey, "");
    std::stringstream ss;
    // 顺便加上自己的key，使得每个校验值的值都是动态的，即相同的校验值对应的不是同一个值
    ss << strValue.c_str() << SECRECT_KEY << pszKey;
    std::string strCheckData = ss.str();
    // 得到校验码
    std::string strCheckSumData = crypto::MD5(strCheckData.c_str());
    
    std::string strCheckKey = pszKey;
    strCheckKey += "Checksum";
    std::string strCheckSum = CCUserDefault::sharedUserDefault()->getStringForKey(strCheckKey.c_str(), "");
    
    if (strValue.empty() && strCheckSum.empty())
    {
        // 无存储数据，有效
        return true;
    }
    
    if (strCheckSumData == strCheckSum)
    {
        // 校验码一致，有效
        bRet = true;
    }
    
    return bRet;
}

NS_BAILIN_UTIL_END
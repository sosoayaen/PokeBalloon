#include "StringUtil.h"

#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
#	include <locale>
#endif

USING_NS_CC;
USING_NS_BAILIN_UTIL;

StringUtil::StringUtil()
{

}

StringUtil::~StringUtil()
{

}

static StringUtil* g_sharedStringUtil = NULL;

StringUtil* StringUtil::sharedStringUtil()
{
	if (!g_sharedStringUtil)
	{
		g_sharedStringUtil = new StringUtil();
	}

	return g_sharedStringUtil;
}

void StringUtil::purgeSharedStringUtil()
{
	CC_SAFE_DELETE(g_sharedStringUtil);
}

std::string StringUtil::StringTrimLeft( const std::string& strContent )
{
	int idx = 0;
	for (std::string::const_iterator iter = strContent.begin(); iter != strContent.end(); ++iter)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (std::isspace(*iter, std::locale("Chinese_China.936")))
#else
		if (std::isspace(*iter))
#endif
		{
			idx++;
		}
		else
		{
			break;
		}
	}

	return strContent.substr(idx);
}

std::string StringUtil::StringTrimRight( const std::string& strContent )
{
	if (strContent.begin() == strContent.end()) {
		return strContent;
	}

	int idx = 0;
	for (std::string::const_reverse_iterator iter = strContent.rbegin(); iter != strContent.rend(); iter++)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (std::isspace(*iter, std::locale("Chinese_China.936")))
#else
		if (std::isspace(*iter))
#endif
		{
			idx++;
		}
		else
		{
			break;
		}
	}

	return strContent.substr(0, strContent.length() - idx);
}

std::string StringUtil::StringTrim( const std::string& strContent )
{
	return StringTrimLeft(StringTrimRight(strContent));
}

std::string bailin::util::StringUtil::GetKeyValueStringWithDictionary( cocos2d::CCDictionary* pData, const char* pszSep1, const char* pszSep2 )
{
	std::stringstream str;
	if (pData)
	{
		// 先得到所有的key
		CCArray* arrayKey = pData->allKeys();
		CCObject* pObj = NULL;
		CCARRAY_FOREACH(arrayKey, pObj)
		{
			CCString* pKey = dynamic_cast<CCString*>(pObj);
			if (pKey)
			{
				const CCString* pValue = pData->valueForKey(pKey->getCString());
				str  << pszSep1 << pKey->getCString() << pszSep2 << pValue->getCString();
			}
		}
	}

	// 去掉头部的连接符
	return str.str().substr(1);
}



time_t bailin::util::StringUtil::GetTime(size_t year,size_t month,size_t day)
{
    time_t tt1;
    tm     tm1;
    time(&tt1);//1970.01.01-2038.01.18
    memcpy(&tm1,localtime(&tt1),sizeof(tm));
    tm1.tm_year = year - 1900;
    tm1.tm_mon  = month - 1;
    tm1.tm_mday = day;
    tt1=mktime(&tm1);
    return tt1;
}


/************************************************************************/
/* StringUtil.h                                                         */
/* Author: Jason Tou                                                    */
/* 提供一些字符串相关的简便操作                                         */
/************************************************************************/

#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include "cocos2d.h"
#include "bailinMacro.h"

NS_BAILIN_UTIL_BEGIN

class StringUtil
{
public:
	StringUtil();
	~StringUtil();

	/**
	* @brief 获得单例对象
	*/
	static StringUtil* sharedStringUtil();

	/**
	* @brief 释放单例对象
	*/
	static void purgeSharedStringUtil();

	/**
	* @brief 字符串去除左边的空格
	* @param strContent 待处理的 std::string 对象
	* @return std::string 返回一个新的 std::string 对象
	*/
	std::string StringTrimLeft(const std::string& strContent);
	
	/**
	* @brief 字符串去除右边的空格
	* @param strContent 待处理的 std::string 对象
	* @return std::string 返回一个新的 std::string 对象
	*/
	std::string StringTrimRight(const std::string& strContent);

	/**
	* @brief 字符串去除两边的空格
	* @param strContent 待处理的 std::string 对象
	* @return std::string 返回一个新的 std::string 对象
	*/
	std::string StringTrim(const std::string& strContent);

	/**
	* @brief 把CCDictionary中的数据，按照key=value的格式返回
	* @param pData
	* @param pszSeq1 大分割，如key=value&key=value中的&符号
	* @param pszSeq1 小分割，如key=value中的=符号
	*/
	std::string GetKeyValueStringWithDictionary(cocos2d::CCDictionary* pData, const char* pszSep1, const char* pszSep2);
    
    time_t GetTime(size_t year,size_t month,size_t day);
    
protected:
	
private:
};

NS_BAILIN_UTIL_END
#endif
/************************************************************************/
/* DataManager.h                                                        */
/* Author: Jason Tou                                                    */
/* 建立一个数据管理中心类，提供整个程序一个独立唯一的全局数据中心，保证 */
/* 线程安全处理数据。处理的内容包括文件读取，转换等等操作               */
/************************************************************************/

#ifndef __DATA_MANAGER_H__
#define __DATA_MANAGER_H__

#include "cocos2d.h"
#include "bailinMacro.h"

NS_BAILIN_UTIL_BEGIN

class DataManagerUtil
{
public:
	DataManagerUtil();
	~DataManagerUtil();

	/**
	* @brief 得到单例
	*/
	static DataManagerUtil* sharedDataManagerUtil();

	/**
	* @brief 释放单例
	*/
	static void purgeSharedDataManagerUtil();

	/**
	* @brief 从字典中获得对应的字符串数据，已经是UTF8格式
	* @param pszSection	对应的大区段
	* @param pszKey 对应的键值
	* @return string
	*/
	const char* GetUTF8StringInDictionary(const char* pszSection, const char* pszKey);
    
    /**
     * @brief 从字典中获得对应的字符串数据，已经是UTF8格式  主要用于翻译服务端返回的提示信息
     * @param pszSection	对应的大区段
     * @param pszKey 对应的键值
     * @return string
     */
	const char* GetUTF8StringInDictionaryForServer( const char* pszKey);

	/**
	* @brief 从字典中获得对应的整形数据
	* @param pszSection	对应的大区段
	* @param pszKey 对应的键值
	* @return long
	*/
	long GetLongValueInDictionary(const char* pszSection, const char* pszKey);

	/**
	* @brief 得到全局的整形数据
	* @param pszKey 对应的键
	*/
	long GetGlobalDataLong(const char* pszKey);

	/**
	* @brief 设置全局的long数据
	* @param pszKey 对应的键
	* @param lValue 需要设置的long值
	*/
	void SetGlobalDataLong(const char* pszKey, long lValue);

    /**
     * @brief 得到全局的整形数据
     * @param pszKey 对应的键
     */
    std::string GetUmengOnlineConfig(const char*pkey);
    
	/**
	* @brief 得到全局的字符串数据
	* @param pszKey 对应的键
	*/
	const char* GetGlobalDataString(const char* pszKey);

	/**
	* @brief 设置全局的字符串数据
	* @param pszKey 对应的键
	* @param pszValue 对应设置的字符串值
	*/
	void SetGlobalDataString(const char* pszKey, const char* pszValue);

	/**
	* @brief 得到异或后的字符串
    * @param str 输入字符串，仅能对字符串数据做加密
	*/
	std::string GetEncryptString(const std::string& str);

	// 设置本地文件的读写，以key value模式存放
	std::string ReadDataFromLocal(const std::string& key);
	bool WriteDataToLocal(const std::string& key, const std::string& value);
    
	/**
	* @brief 从CCObject中获得long数据
	*/
	long GetLongValueWithObject(cocos2d::CCObject* pObj);

	/**
	* @brief 从CCObject中获得float数据
	*/
	float GetFloatValueWithObject( cocos2d::CCObject* pObj );

	/**
	* @brief 从CCObject中获得fouble数据
	*/
	double GetDoubleValueWithObject( cocos2d::CCObject* pObj );

	/**
	* @brief 从CCObject中获得long long数据
	*/
	long long GetLongLongValueWithObject(cocos2d::CCObject* pObj);

	/**
	* @brief 给底层NDK发送消息调用对应的函数
	*/
	void SendNDKMessages(const char* pszNativeFuncName, cocos2d::CCDictionary* pData);
    
    /**
     * @brief 
     */

protected:
	// 全局唯一的字符串数据
	cocos2d::CCDictionary* m_pDictionaryStringData;

	// 内存中的全局数据
	cocos2d::CCDictionary* m_pDictionaryGlobalData;
    
private:

	// 加载字符串配置
	cocos2d::CCDictionary* getLocaleStringData();

	// 得到全局数据对象
	cocos2d::CCDictionary* getGlobalData();
    
    //获取keychain 数据（udid,uid,passward）
    cocos2d::CCDictionary* getKeyChainData();

};

NS_BAILIN_UTIL_END
#endif // __DATA_MANAGER_H__
	
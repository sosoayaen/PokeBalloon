/**
 * 得到底层应用程序的信息
 *
 */
#ifndef __APP_INFO_H__
#define __APP_INFO_H__

#include <string>

class AppInfo
{
public:
	AppInfo()
	{
		m_nVersionCode = 0;
		m_nChannelCode = 0;
		m_strVersionName = "0.00.00";
		m_strChannelName = "";
	}

	virtual ~AppInfo(){}

	static AppInfo* sharedAppInfo();

	/**
	 * @brief 得到程序的版本号
	 * @param const std::string& 版本号是字符串型
	 */
	const std::string& getVersionName();

	/**
	 * @brief 得到程序的编译版本号
	 * @param int 版本号是整形
	 */
	int getVersionCode();

private:
	int m_nVersionCode;

	std::string m_strVersionName;

	int m_nChannelCode;

	std::string m_strChannelName;

};

#endif // __APP_INFO_H__

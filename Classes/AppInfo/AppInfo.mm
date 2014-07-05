/**
 * 用来获取iOS应用的信息等
 */
#include "AppInfo.h"

static AppInfo* g_sAppInfo = NULL;

AppInfo* AppInfo::sharedAppInfo()
{
    if (g_sAppInfo == NULL)
    {
        g_sAppInfo = new AppInfo();
    }
    return g_sAppInfo;
}

const std::string& AppInfo::getVersionName()
{
    if (m_strVersionName != "0.00.00")
        return m_strVersionName;
    
    // 获取版本号
    NSString* versionName = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    m_strVersionName = versionName.UTF8String;
    return m_strVersionName;
}

int AppInfo::getVersionCode()
{
    if (m_nVersionCode > 0)
        return m_nVersionCode;
    
    // 获取编译版本号
    NSString* versionCode = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleVersionKey];
    m_nVersionCode = [versionCode intValue];
    return m_nVersionCode;
}
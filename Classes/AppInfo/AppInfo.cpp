/**
 * Android 获取程序信息实现类
 */
#include "AppInfo.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#ifndef NULL
#	define NULL 0
#endif

USING_NS_CC;

#define JNI_CLASS "com/wardrums/lib/WDBaseActivity"

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

	// 调用JNI中的获得VersionName的函数
	JniMethodInfo methodInfo;

	bool isHave = JniHelper::getStaticMethodInfo(methodInfo,
			JNI_CLASS,
			"getContext",
			"()Landroid/content/Context;");

	jobject activityObj;
	if (isHave)
	{
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(methodInfo, JNI_CLASS, "getVersionName", "()Ljava/lang/String;");

	if (isHave)
	{
		jstring jStrValue = (jstring)methodInfo.env->CallObjectMethod(activityObj, methodInfo.methodID);
		char* pszValue = strdup(methodInfo.env->GetStringUTFChars(jStrValue, false));
		m_strVersionName = pszValue;
		methodInfo.env->ReleaseStringUTFChars(jStrValue, pszValue);
	}

	return m_strVersionName;
}

int AppInfo::getVersionCode()
{
	if (m_nVersionCode > 0)
		return m_nVersionCode;

	// 调用JNI中的获得VersionCode的函数
	JniMethodInfo methodInfo;

	bool isHave = JniHelper::getStaticMethodInfo(methodInfo,
			JNI_CLASS,
			"getContext",
			"()Landroid/content/Context;");

	jobject activityObj;
	if (isHave)
	{
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(methodInfo, JNI_CLASS, "getVersionCode", "()I");

	if (isHave)
	{
		m_nVersionCode = methodInfo.env->CallIntMethod(activityObj, methodInfo.methodID);
	}

	return m_nVersionCode;
}

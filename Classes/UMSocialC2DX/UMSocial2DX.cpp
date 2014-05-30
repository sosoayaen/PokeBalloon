#include "UMSocial2DX.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

USING_NS_CC;

std::string UMSocial2DX::m_strPackageName = "";

void UMSocial2DX::setAppKey(const char* pszAppKey, const char* pszPackageName /* = NULL */)
{
	if (pszPackageName)
		m_strPackageName = pszPackageName;
	else
		return;

	JniMethodInfo methodInfo;

	bool isHave = JniHelper::getStaticMethodInfo(methodInfo,
			m_strPackageName.c_str(),
			"getContext",
			"()Landroid/content/Context;");

	jobject activityObj;
	if (isHave)
	{
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(methodInfo, m_strPackageName.c_str(), "setUMengSocialAppKey", "(Ljava/lang/String;)V");

	if (isHave)
	{
		jstring jsAppKey = methodInfo.env->NewStringUTF(pszAppKey);
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, jsAppKey);
	}
}

void UMSocial2DX::shareSNS(CCDictionary* pDictData)
{
	if (m_strPackageName.empty()) return;
	// 显示SNS分享窗口
	JniMethodInfo methodInfo;

	bool isHave = JniHelper::getStaticMethodInfo(methodInfo,
			m_strPackageName.c_str(),
			"getContext",
			"()Landroid/content/Context;");

	jobject activityObj;
	if (isHave)
	{
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(methodInfo, m_strPackageName.c_str(), "showSNSBoard", "(Ljava/lang/String;Ljava/lang/String;)V");

	if (isHave)
	{
		jstring jsContent = methodInfo.env->NewStringUTF(pDictData->valueForKey("shareText")->getCString());
		jstring jsImagePath = methodInfo.env->NewStringUTF(pDictData->valueForKey("shareImage")->getCString());
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, jsContent, jsImagePath);
	}
}

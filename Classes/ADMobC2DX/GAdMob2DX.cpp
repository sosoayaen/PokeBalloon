#include "GAdMob2DX.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#ifndef NULL
#	define NULL 0
#endif

USING_NS_CC;

static GAdMob2DX* g_pGAdMobInstance = NULL;

GAdMob2DX* GAdMob2DX::sharedGAdMob2DX()
{
    if (!g_pGAdMobInstance)
    {
        g_pGAdMobInstance = new GAdMob2DX;
    }
    
    return g_pGAdMobInstance;
}

bool GAdMob2DX::init(const char* pszUnitID, const char* pszPackageName /* = NULL */)
{
	if (pszPackageName)
		m_strPackageName = pszPackageName;
	else
		return false;

	// 调用JNI中的初始化函数
	JniMethodInfo methodInfo;

	CCLOG("before getAcitvity");
	bool isHave = JniHelper::getStaticMethodInfo(methodInfo,
			m_strPackageName.c_str(),
			"getContext",
			"()Landroid/content/Context;");

	jobject activityObj;
	if (isHave)
	{
		CCLOG("have getAcitvity");
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(methodInfo, m_strPackageName.c_str(), "initAdView", "(Ljava/lang/String;)V");

	if (isHave)
	{
		jstring junitID = methodInfo.env->NewStringUTF(pszUnitID);
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, junitID);
	}
	
	return true;
}

void GAdMob2DX::setVisible(bool bShow)
{
	if (m_strPackageName.empty())
	{
		return;
	}
	// 调用JNI中的初始化函数
	
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

	isHave = JniHelper::getMethodInfo(methodInfo, m_strPackageName.c_str(), "setVisibleAdView", "(Z)V");

	if (isHave)
	{
		jboolean jbVisible = bShow ? JNI_TRUE : JNI_FALSE;
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, jbVisible);
	}
	
	return;	
}

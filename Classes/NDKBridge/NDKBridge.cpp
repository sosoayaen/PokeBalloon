/**
 *
 * Android 的NDK桥接层
 *
 */
#include "NDKBridge.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#ifndef NULL
#	define NULL 0
#endif

USING_NS_CC;

#define JNI_CLASS "com/wardrums/lib/WDBaseActivity"

static JniMethodInfo g_sMethodInfo;
static jobject g_sActivityObj;

NDKBridge* g_sNDKBridge = NULL;

NDKBridge* NDKBridge::sharedNDKBridge()
{
	if (g_sNDKBridge == NULL)
	{
		g_sNDKBridge = new NDKBridge();
		CCAssert(g_sNDKBridge, "NDKBridge instance create failed...");
		memset(&g_sMethodInfo, 0, sizeof(JniMethodInfo));
		bool bInitJNI = JniHelper::getStaticMethodInfo(g_sMethodInfo,
				JNI_CLASS,
				"getContext",
				"()Landroid/content/Context;");
		CCAssert(bInitJNI, "JNI Object get error!");
		g_sActivityObj = g_sMethodInfo.env->CallStaticObjectMethod(g_sMethodInfo.classID, g_sMethodInfo.methodID);

	}
	return g_sNDKBridge;
}

NDKBridge::NDKBridge()
{
    m_strIAPShareSecret = "";
	m_strIAPADProductID = "";
}

NDKBridge::~NDKBridge()
{
    
}

static jstring str2Jstring(JNIEnv* env, const char* pat)
{
	jclass strClass = env->FindClass("Ljava/lang/String;");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	jbyteArray bytes = env->NewByteArray(strlen(pat));
	env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
	jstring encoding = env->NewStringUTF("utf-8");
	return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}

void NDKBridge::setNotification(CCDictionary* pData)
{
	/*
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

	isHave = JniHelper::getMethodInfo(methodInfo, JNI_CLASS, "setNotification", "(Ljava/lang/String;J)V");

	if (isHave)
	{
		long lTimeinterval = pData->valueForKey("timeinterval")->intValue();
		// 结束游戏2天后弹出提示，之后一周一次
		if (lTimeinterval == 0)
			lTimeinterval = 86400*2;
		const std::string& strContent = pData->valueForKey("notificationText")->m_sString;
		jstring jstrContent = methodInfo.env->NewStringUTF(strContent.c_str());
		jlong jlTime = (jlong)lTimeinterval;
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, jstrContent, jlTime);
	}	
	*/
	bool bIsHave = JniHelper::getMethodInfo(g_sMethodInfo, JNI_CLASS, "setNotification", "(Ljava/lang/String;J)V");
	if (bIsHave)
	{
		long lTimeinterval = pData->valueForKey("timeinterval")->intValue();
		// 结束游戏2天后弹出提示，之后一周一次
		if (lTimeinterval == 0)
			lTimeinterval = 86400*2;
		const std::string& strContent = pData->valueForKey("notificationText")->m_sString;
		jstring jstrContent = g_sMethodInfo.env->NewStringUTF(strContent.c_str());
		jlong jlTime = (jlong)lTimeinterval;
		g_sMethodInfo.env->CallVoidMethod(g_sActivityObj, g_sMethodInfo.methodID, jstrContent, jlTime);
	}
}

void NDKBridge::cancelNotification(CCDictionary* pData)
{

}

bool NDKBridge::initAdMob(const char* pszUnitID, const char* pszClassName /* = NULL */)
{
	if (!isADOff())
		return GAdMob2DX::sharedGAdMob2DX()->init(pszUnitID, pszClassName);
	else
		return false;
}

void NDKBridge::setAdMobVisible(bool bVisible)
{
	if (isADOff())
		bVisible = false;

	GAdMob2DX::sharedGAdMob2DX()->setVisible(bVisible);
}

void NDKBridge::initIAP(CCArray* pArrayProductID, const char* pszShareSecret /* = NULL */)
{
	// TODO: 这里接入Android的内部支付初始化工作，还有对应的SDK版本的区分
}

void NDKBridge::setADProductID(const char* pszADProductID)
{

}

void NDKBridge::buyIAPProduct(unsigned int productIdx)
{
	// TODO: 这里进行Android的支付
}

bool NDKBridge::isADOff()
{
	bool bRet = false;
	return bRet;
}

void NDKBridge::restoreIAPProducts()
{
}

void NDKBridge::clearSavedPurchasedProducts()
{
}

std::string NDKBridge::getDeviceUDID()
{
	std::string retStr = "";
	bool bIsHave = JniHelper::getMethodInfo(g_sMethodInfo, JNI_CLASS, "getDeviceUDID", "()Ljava/lang/String;");
	if (bIsHave)
	{
		jstring udid = g_sMethodInfo.env->CallObjectMethod(g_sActivityObj, g_sMethodInfo.methodID);
		retStr = JniHelper::jstring2string(udid);
	}
	return retStr;
}

std::string NDKBridge::getDeviceName()
{
	std::string retStr = "";
	bool bIsHave = JniHelper::getMethodInfo(g_sMethodInfo, JNI_CLASS, "getDeviceName", "()Ljava/lang/String;");
	if (bIsHave)
	{
		jstring udid = g_sMethodInfo.env->CallObjectMethod(g_sActivityObj, g_sMethodInfo.methodID);
		retStr = JniHelper::jstring2string(udid);
	}
	return retStr;

}

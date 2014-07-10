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

NDKBridge* g_sNDKBridge = NULL;

NDKBridge* NDKBridge::sharedNDKBridge()
{
	if (g_sNDKBridge == NULL)
	{
		g_sNDKBridge = new NDKBridge();
		CCAssert(g_sNDKBridge, "NDKBridge instance create failed...");
	}
	return g_sNDKBridge;
}

NDKBridge::NDKBridge()
{
    
}

NDKBridge::~NDKBridge()
{
    
}

static jstring stoJstring(JNIEnv* env, const char* pat)
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

	isHave = JniHelper::getMethodInfo(methodInfo, JNI_CLASS, "setNotification", "(Ljava/lang/String;LL)");

	if (isHave)
	{
		long timeinterval = pData->valueForKey("timeinterval")->intValue();
		const std::string& strContent = pData->valueForKey("notificationText")->m_sString;
		int len = strContent.length();
		jstring jstrContent = stoJstring(methodInfo.env, strContent.c_str());
		methodInfo.env->CallVoidMethod(activityObj, methodInfo.methodID, jstrContent, timeinterval);
	}	
}

void NDKBridge::cancelNotification(CCDictionary* pData)
{

}

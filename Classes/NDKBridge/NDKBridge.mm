/**
  * iOS NDK桥接文件
  *
  */

#include "NDKBridge.h"
#import "NDKManager.h"

static NDKBridge* g_sNDKBridge = NULL;

NDKBridge* NDKBridge::sharedNDKBridge()
{
    if (g_sNDKBridge == NULL)
    {
        g_sNDKBridge = new NDKBridge;
    }
    return g_sNDKBridge;
}

NDKBridge::NDKBridge()
{
    
}

NDKBridge::~NDKBridge()
{
    
}

//

void NDKBridge::setNotification(cocos2d::CCDictionary *pData)
{
    NSString *strContent = [[NSString alloc] initWithUTF8String: pData->valueForKey("notificationText")->getCString()];
    NSString *strNotificationName = [[NSString alloc] initWithUTF8String: pData->valueForKey("notificationName")->getCString()];
    u_int timeinterval = pData->valueForKey("timeinterval")->uintValue();
    // 调用iOS底层的通知接口
    [[NDKManager sharedNDKManager] setLocalNotificationWithContent: strContent notificationName:strNotificationName timeinterval:timeinterval forceAdd:NO];
}

void NDKBridge::cancelNotification(cocos2d::CCDictionary *pData)
{
    NSString *strNotificationName = [[NSString alloc] initWithUTF8String: pData->valueForKey("notificationName")->getCString()];
    [[NDKManager sharedNDKManager] cancelNotification: strNotificationName];
}
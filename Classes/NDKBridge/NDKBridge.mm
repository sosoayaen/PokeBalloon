/**
  * iOS NDK桥接文件
  *
  */

#include "NDKBridge.h"
#import "NDKManager.h"
#import "IAPShare.h"
#include "bailinUtil.h"

USING_NS_CC;
USING_NS_BAILIN_UTIL;

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

void NDKBridge::initIAP(cocos2d::CCArray *pArrayProductID)
{
    if (![IAPShare sharedHelper].iap)
    {
        NSMutableArray* pNSArray = [[NSMutableArray alloc] init];
        CCObject* pObj = NULL;
        CCARRAY_FOREACH(pArrayProductID, pObj)
        {
            CCString* pValue = dynamic_cast<CCString*>(pObj);
            if (pValue)
            {
                NSString* pString = [[NSString alloc] initWithUTF8String:pValue->getCString()];
                [pNSArray addObject:pString];
            }
        }
        // 把数据转化为NSSet
        NSSet* dataSet = [[NSSet alloc] initWithArray:pNSArray];
        
        // 这里注册所有的内购项目
        [IAPShare sharedHelper].iap = [[IAPHelper alloc] initWithProductIdentifiers:dataSet];
    
        [IAPShare sharedHelper].iap.production = NO;
        
        [[IAPShare sharedHelper].iap requestProductsWithCompletion:^(SKProductsRequest *request, SKProductsResponse *response) {
            if (response > 0)
            {
                // 这里把对应的数据拷贝一份放到cocos2dx的CCArray数据中，供CCTableView展现数据
                unsigned int nSize = [[IAPShare sharedHelper].iap.products count];
                CCArray* pArrayProductArray = CCArray::createWithCapacity(nSize);
                for (NSUInteger idx = 0; idx < nSize; ++idx)
                {
                    SKProduct* product = [[IAPShare sharedHelper].iap.products objectAtIndex:idx];
                    CCDictionary* pDict = CCDictionary::create();
                    pDict->setObject(ccs([product.localizedTitle UTF8String]), "title");
                    pDict->setObject(ccs([product.localizedDescription UTF8String]), "description");
                    // 得到价格
                    NSNumberFormatter *_priceFormatter = [[NSNumberFormatter alloc] init];
                    [_priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
                    [_priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                    pDict->setObject(ccs([[_priceFormatter stringFromNumber:product.price] UTF8String]), "price");
                    pDict->setObject(ccs([product.productIdentifier UTF8String]), "productId");
                    // 记录对应在数组中的位置，以便点击购买可以访问对应的购买对象
                    pDict->setObject(CCString::createWithFormat("%u", idx), "idx");
                    pArrayProductArray->addObject(pDict);
                    
                    NSLog(@"product title:[%@], description:[%@], price:[%@]", product.localizedTitle, product.localizedDescription, product.price);
                }
                
                // 直接填充数据
                DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataObject("IAP_ProductArray", pArrayProductArray);
            }
        }];
    }
}
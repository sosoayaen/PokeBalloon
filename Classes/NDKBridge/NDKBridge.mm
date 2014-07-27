/**
  * iOS NDK桥接文件
  *
  */

#include "NDKBridge.h"
#import "NDKManager.h"
#import "IAPShare.h"
#include "bailinUtil.h"
#include "Balloon_macro.h"
#include "UserData.h"

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
    m_strIAPShareSecret = "";
    m_strIAPADProductID = "";
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

void NDKBridge::initIAP(cocos2d::CCArray *pArrayProductID, const char* pszShareSecret /* = NULL */)
{
    if (![IAPShare sharedHelper].iap)
    {
        if (pszShareSecret)
            m_strIAPShareSecret = pszShareSecret;
        
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
                    // 先判断下，这个是否已经被购买过，对于一次性的道具来讲，购买过了就不用显示了，或者标注下已经购买过
                    if ([product.productIdentifier isEqualToString:@"com.wardrums.pokeballoon.removead"] && [[IAPShare sharedHelper].iap isPurchasedProductsIdentifier:product.productIdentifier])
                    {
                        continue;
                    }
                    /*
                    NSRange range = [product.productIdentifier rangeOfString:@"non-consumable"];
                    if (range.length > 0 && [[IAPShare sharedHelper].iap isPurchasedProductsIdentifier:product.productIdentifier])
                    {
                        // 该项目已被购买，无需显示
                        continue;
                    }
                    //*/
                    
                    CCDictionary* pDict = CCDictionary::create();
                    pDict->setObject(ccs([product.localizedTitle UTF8String]), "title");
                    pDict->setObject(ccs([product.localizedDescription UTF8String]), "description");
                    // 得到价格
                    NSNumberFormatter *_priceFormatter = [[NSNumberFormatter alloc] init];
                    [_priceFormatter setLocale:product.priceLocale];
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

void NDKBridge::buyIAPProduct(unsigned int productIdx)
{
    SKProduct* product = [[IAPShare sharedHelper].iap.products objectAtIndex:productIdx];
    if (!product) return;
    
    // 正在购买中
    DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_PURCHASING);
    
    // 发起购买请求
    [[IAPShare sharedHelper].iap buyProduct:product onCompletion:^(SKPaymentTransaction *transcation) {
        if (transcation.error)
        {
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_FAILED);
            NSLog(@"Buy Product error! %@", [transcation.error localizedDescription]);
        }
        else if (transcation.transactionState == SKPaymentTransactionStatePurchased)
        {
            if (m_strIAPShareSecret.empty())
            {
                UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Critical" message:@"SharedSecret is not set!" delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
                [alert show];
                [alert release];
            }
            else
            {
                // 校验回执单
                NSString* pSharedSecret = [[NSString alloc] initWithUTF8String:m_strIAPShareSecret.c_str()];
                [[IAPShare sharedHelper].iap checkReceipt:transcation.transactionReceipt AndSharedSecret:pSharedSecret onCompletion:^(NSString *response, NSError *error) {
                    NSDictionary* rec = [IAPShare toJSON:response];
                    
                    if ([rec[@"status"] integerValue] == 0)
                    {
                        NSString* productIdentifier = transcation.payment.productIdentifier;
                        [[IAPShare sharedHelper].iap provideContent:productIdentifier];
                        // 购买成功，设置全局标记
                        DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_PURCHASED);
                        // 根据购买的项目结算对应值
                        if ([transcation.payment.productIdentifier isEqualToString:@"com.wardrums.pokeballoon.removead"])
                        {
                            // 去除广告
                        }
                        else if ([transcation.payment.productIdentifier isEqualToString:@"com.wardrums.pokeballoon.buyCoinsOne"])
                        {
                            // 增加1000块钱
                            UserDataManager::sharedUserDataManager()->addGoldenCoins(1000);
                        }
                        else if ([transcation.payment.productIdentifier isEqualToString:@"com.wardrums.pokeballoon.buyCoinsTwo"])
                        {
                            // 增加3000块钱
                            UserDataManager::sharedUserDataManager()->addGoldenCoins(3000);
                        }
                        NSLog(@"Purchase Successed!!! %@", transcation.payment.productIdentifier);
                    }
                    else
                    {
                        // 校验失败，可能是仿造的
                        DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_FAILED_RECEIPT);
                        NSLog(@"Buy Failed... Receipt failed...");
                    }
                }];
            }
        }
        else if (transcation.transactionState == SKPaymentTransactionStateFailed)
        {
            // 各种原因导致购买失败
            DataManagerUtil::sharedDataManagerUtil()->SetGlobalDataLong(GLOBAL_KEY_IAP_PURCHASE_STATUS, IAP_PURCHASE_FAILED);
            NSLog(@"Buy Failed...");
        }
    }];
}

void NDKBridge::setADProductID(const char *pszADProductID)
{
    if (pszADProductID)
        m_strIAPADProductID = pszADProductID;
}

bool NDKBridge::isADOff()
{
    if (m_strIAPADProductID.empty()) return false;
    return [[IAPShare sharedHelper].iap isPurchasedProductsIdentifier:[[NSString alloc] initWithUTF8String:m_strIAPADProductID.c_str()]] == YES;
}

void NDKBridge::restoreIAPProducts()
{
    [[IAPShare sharedHelper].iap restoreProductsWithCompletion:^(SKPaymentQueue *payment, NSError *error) {
        // int numberOfTransactions = payment.transactions.count;
        
        for (SKPaymentTransaction *trans in payment.transactions)
        {
            NSString* purchased = trans.payment.productIdentifier;
            if ( [purchased isEqualToString:@"com.wardrums.pokeballoon.removead"] )
            {
                // 记录移除广告
                [[IAPShare sharedHelper].iap provideContent:purchased];
                UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Restart your App to remove the AD" delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
                
                [alert show];
                [alert release];
            }
        }
    }];
}
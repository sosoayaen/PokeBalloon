#include "UMSocial2DX.h"

USING_NS_CC;

#import "UMSocial.h"

void UMSocial2DX::setAppKey(const char *pszAppKey)
{
    NSString *strNSString = [[NSString alloc] initWithUTF8String:pszAppKey];
    [UMSocialData setAppKey:strNSString];
}

void UMSocial2DX::shareSNS(cocos2d::CCDictionary *pDictData)
{
    const CCString* pStrShareText = pDictData->valueForKey("shareText");
    NSString* strNSStringShareText = [[NSString alloc] initWithUTF8String:pStrShareText->getCString()];
    // 调用对应界面显示
    [UMSocialSnsService presentSnsIconSheetView:[UIApplication sharedApplication].keyWindow.rootViewController appKey:nil
                    shareText:strNSStringShareText
                    shareImage:[UIImage imageNamed:@"icon.png"]
                    shareToSnsNames:[NSArray arrayWithObjects:UMShareToTencent, UMShareToSina, UMShareToTwitter, UMShareToFacebook, nil]
                    delegate:nil];
}
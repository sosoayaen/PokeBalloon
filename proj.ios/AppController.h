#import <UIKit/UIKit.h>

#import "ShareSDK/ShareSDK.h"
#import "GADBannerView.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, GADBannerViewDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
    GADBannerView *bannerView_;
}

@end


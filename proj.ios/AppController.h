#import <UIKit/UIKit.h>
#import "ShareSDK/ShareSDK.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@end


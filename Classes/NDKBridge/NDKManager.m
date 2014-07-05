//
//  NDKManager.m
//  PokeBalloon
//
//  Created by mac on 14-7-5.
//
//

#import "NDKManager.h"

@implementation NDKManager

+(id) sharedNDKManager {
    static NDKManager *sharedNDKManager;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedNDKManager = [[NDKManager alloc] init];
    });
    return sharedNDKManager;
}

- (void) setLocalNotificationWithContent:(NSString *)content notificationName:(NSString *)notificationName timeinterval:(u_int)timeinteval forceAdd:(BOOL)forceAdd {
    UILocalNotification *notification = [[[UILocalNotification alloc] init] autorelease];
    
    if (notification != nil)
    {
        if (forceAdd == NO)
        {
            // 先得到所有的本地通知对象，然后看是否有当前的通知，如果没有就添加，有了就直接返回
            NSArray *notiArr = [[UIApplication sharedApplication] scheduledLocalNotifications];
            for (UILocalNotification* noti in notiArr)
            {
                NSDictionary *dict = noti.userInfo;
                if (dict && [[dict objectForKey:@"notificationName"] isEqualToString:notificationName])
                {
                    // 找到了就直接返回
                    return;
                }
            }
        }
        
        // 这里添加
        if (timeinteval == 0)
        {
            // 默认两天提醒下
            timeinteval = 86400*2;
        }
        NSDate *now = [NSDate new];
        notification.fireDate = [now dateByAddingTimeInterval:timeinteval];
        notification.timeZone = [NSTimeZone defaultTimeZone];
        notification.repeatInterval = NSWeekdayCalendarUnit;
        notification.soundName = UILocalNotificationDefaultSoundName;
        notification.applicationIconBadgeNumber = 1;
        notification.alertAction = @"Look";
        notification.alertBody = content;
        
        NSDictionary* dict = [NSDictionary dictionaryWithObject:notificationName forKey:@"notificationName"];
        notification.userInfo = dict;
        // 设定通知
        [[UIApplication sharedApplication] scheduleLocalNotification:notification];
    }
}

- (void) cancelNotification:(NSString *)notificationCancelName {
    NSArray *notiArr = [[UIApplication sharedApplication] scheduledLocalNotifications];
    
    if (notiArr) {
        for (UILocalNotification *noti in notiArr) {
            NSDictionary *dict = noti.userInfo;
            if (dict) {
                NSString *inKey = [dict objectForKey:@"notificationName"];
                if ( [inKey isEqualToString:notificationCancelName] ) {
                    // 取消本地推送
                    [[UIApplication sharedApplication] cancelLocalNotification:noti];
                }
            }
        }
    }
}

@end

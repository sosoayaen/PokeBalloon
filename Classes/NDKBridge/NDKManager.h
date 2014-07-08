//
//  NDKManager.h
//  PokeBalloon
//
//  Created by mac on 14-7-5.
//
//

#import <Foundation/Foundation.h>

@interface NDKManager : NSObject<UIAlertViewDelegate>

+(id) sharedNDKManager;

-(void) setLocalNotificationWithContent:(NSString *)content notificationName:(NSString *)notificationName timeinterval:(u_int)timeinteval forceAdd:(BOOL)forceAdd;

-(void) cancelNotification:(NSString *)notificationCancelName;

@end

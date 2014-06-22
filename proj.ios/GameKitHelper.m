//
//  GameKitHelper.m
//  PokeBalloon
//
//  Created by Jason Tou on 14-5-28.
//
//

#import "cocos2d.h"
#import "GameKitHelper.h"

@interface GameKitHelper()
        <GKGameCenterControllerDelegate> {
    BOOL _gameCenterFeaturesEnabled;
}
@end

@implementation GameKitHelper

#pragma mark Singleton stuff

+(id) sharedGameKitHelper {
    static GameKitHelper *sharedGameKitHelper;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedGameKitHelper = [[GameKitHelper alloc] init];
    });
    return sharedGameKitHelper;
}

#pragma mark Player Authentication

-(void) authenticateLocalPlayer {
    GKLocalPlayer* localPlayer =
    [GKLocalPlayer localPlayer];
    
    localPlayer.authenticateHandler =
    ^(UIViewController *viewController, NSError *error) {
        [self setLastError:error];
        
        if (cocos2d::CCDirector::sharedDirector()->isPaused()) {
            cocos2d::CCDirector::sharedDirector()->resume();
        }
        
        // Player already authenticated
        if (localPlayer.authenticated) {
            _gameCenterFeaturesEnabled = YES;
        } else if (viewController) {
            cocos2d::CCDirector::sharedDirector()->pause();
            [self presentViewController:viewController];
        } else {
            _gameCenterFeaturesEnabled = NO;
        }
    };
}

#pragma mark Property setters

-(void) setLastError:(NSError *)error {
    _lastError = [error copy];
    if (_lastError) {
        NSLog(@"GameKitHelper ERROR: [%@] [%@]", [[_lastError userInfo] description], [_lastError localizedDescription]);
        /*
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Game Center Error" message:[_lastError localizedDescription] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        */
    }
}

#pragma mark UIViewController stuff

-(UIViewController*) getRootViewController {
    return [UIApplication sharedApplication].keyWindow.rootViewController;
}

-(void) presentViewController:(UIViewController*)vc {
    UIViewController* rootVC = [self getRootViewController];
    [rootVC presentViewController:vc animated:YES completion:nil];
}

-(void) submitScore:(int64_t)score category:(NSString *)category {
    // 1: Check if Game Center
    // features are enabled
    if (!_gameCenterFeaturesEnabled) {
        NSLog(@"Player not authenticated");
        return;
    }
    
    // 2: Create a GKScore object
    GKScore* gkScore =
        [[GKScore alloc]
            initWithCategory:category];
    
    // 3: Set the score value
    gkScore.value = score;
    
    // 4: Send the score to Game Center
    [gkScore reportScoreWithCompletionHandler:^(NSError *error) {
        [self setLastError:error];
        
        BOOL success = (error == nil);
        
        if ([_delegate respondsToSelector:@selector(onScoresSubmitted:)]) {
            [_delegate onScoresSubmitted:success];
        }
    }];
}

-(void) showLeaderboard:(NSString *)leaderboard {
    if (!_gameCenterFeaturesEnabled) {
        NSLog(@"Player not authenticated");
        return;
    }
    
    GKGameCenterViewController* gameCenterViewController = [[GKGameCenterViewController alloc] init];
    gameCenterViewController.viewState = GKGameCenterViewControllerStateLeaderboards;
    gameCenterViewController.gameCenterDelegate = self;
    [[self getRootViewController] presentViewController:gameCenterViewController animated:YES completion:nil];
}

-(void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [[self getRootViewController] dismissViewControllerAnimated:YES completion:nil];
}
@end

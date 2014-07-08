//
//  GameKitHelper.h
//  PokeBalloon
//
//  Created by Jason Tou on 14-5-28.
//
//

// Include the GameKit framework
#import <GameKit/GameKit.h>

// Protocol to notify external
// objects when Game Center events occur or
// when Game Center async tasks are completed
@protocol GameKitHelperProtocol <NSObject>

-(void) onScoresSubmitted:(bool)success;

@end

@interface GameKitHelper : NSObject<GKGameCenterControllerDelegate, UIAlertViewDelegate>

@property (nonatomic, assign)
    id<GameKitHelperProtocol> delegate;

// This property holds the last know error
// that occured while using the Game Center API's
@property (nonatomic, readonly) NSError* lastError;

+(id) sharedGameKitHelper;

// Player authentication, info
-(void) authenticateLocalPlayer;

// Scores
-(void) submitScore:(int64_t)socre
           category:(NSString*)category;

// Show Leaderboard
-(void) showLeaderboard : (NSString*)leaderboard;

// Show Rate View
-(void) showRateView;

@end
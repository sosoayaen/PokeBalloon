/*
 * =====================================================================================
 *
 *       Filename:  GameKitHelper2dx.m
 *
 *    Description:  GameCenter辅助类
 *
 *        Version:  1.0
 *        Created:  05/28/2014 18:57:58
 *       Revision:  none
 *       Compiler:
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "GameKitHelper2dx.h"
#import "GameKitHelper.h"

void GameKitHelper2dx::authGameCenter()
{
    [[GameKitHelper sharedGameKitHelper] authenticateLocalPlayer];
}

void GameKitHelper2dx::sendScore(long lScore)
{
    [[GameKitHelper sharedGameKitHelper] submitScore:lScore category:@"pokeballoon.maxscore"];
}

void GameKitHelper2dx::showLeaderboard()
{
    [[GameKitHelper sharedGameKitHelper] showLeaderboard:nil];
}

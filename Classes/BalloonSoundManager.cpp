#include "BalloonSoundManager.h"
#include "platform/CCPlatformConfig.h"
#include "SimpleAudioEngine.h"
#include "bailinUtil.h"

USING_NS_BAILIN_UTIL;

using namespace CocosDenshion;

#if (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
#	define SOUND_FILE_SUFFIX ".ogg"
#elif (CC_PLATFORM_IOS == CC_TARGET_PLATFORM)
#	define SOUND_FILE_SUFFIX ".m4a"
#elif (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
#	define SOUND_FILE_SUFFIX ".wav"
#else
#   define SOUND_FILE_SUFFIX ".mp3"
#endif

#define SOUND_EFFECT_EXPLOSIVE	"music/effect/Explosive" SOUND_FILE_SUFFIX

#define SOUND_EFFECT_PUSH_BALLOON   "music/effect/push_balloon" SOUND_FILE_SUFFIX

static BalloonSoundManager* g_s_soundManager = NULL;

BalloonSoundManager* BalloonSoundManager::sharedBalloonSoundManager()
{
	if (!g_s_soundManager)
	{
		g_s_soundManager = new BalloonSoundManager;
        if (g_s_soundManager)
            g_s_soundManager->preloadAllEffect();
	}

	return g_s_soundManager;
}

void BalloonSoundManager::purgeBalloonSoundManager()
{
	if (g_s_soundManager)
	{
		// unload all effects
        g_s_soundManager->unloadAllEffect();

		delete g_s_soundManager;	
		g_s_soundManager = NULL;
	}
}

bool BalloonSoundManager::isBackgroundMusicPlaying()
{
    return SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
}

void BalloonSoundManager::playEffectWithFileName(const char* pszEffectFile)
{
    if (DataManagerUtil::sharedDataManagerUtil()->GetGlobalDataLong(OPT_SOUND_EFFECT_OFF) == 1)
        return;
    
	SimpleAudioEngine::sharedEngine()->playEffect(pszEffectFile);
}

void BalloonSoundManager::playEffectExplosive()
{
	playEffectWithFileName(SOUND_EFFECT_EXPLOSIVE);
}

void BalloonSoundManager::playEffectPushBalloon()
{
    playEffectWithFileName(SOUND_EFFECT_PUSH_BALLOON);
}

void BalloonSoundManager::playBackgroundMusic(int soundId)
{
    const char* pszBackgroundSoundName = NULL;
    switch (soundId)
    {
        case SOUND_BACKGROUND_AFTER_SCHOOL:
            pszBackgroundSoundName = SOUND_BACKGROUND_AFTER_SCHOOL_FILE;
            break;
            
        default:
            return;
    }
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic(pszBackgroundSoundName, true);
}

void BalloonSoundManager::preloadAllEffect()
{
    // preload all effects
    SimpleAudioEngine::sharedEngine()->preloadEffect(SOUND_EFFECT_EXPLOSIVE);
    SimpleAudioEngine::sharedEngine()->preloadEffect(SOUND_EFFECT_PUSH_BALLOON);
}

void BalloonSoundManager::unloadAllEffect()
{
    // unload all effects
    SimpleAudioEngine::sharedEngine()->unloadEffect(SOUND_EFFECT_EXPLOSIVE);
    SimpleAudioEngine::sharedEngine()->unloadEffect(SOUND_EFFECT_PUSH_BALLOON);
}
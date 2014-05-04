#include "BalloonSoundManager.h"
#include "platform/CCPlatformConfig.h"
#include "SimpleAudioEngine.h"

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

static BalloonSoundManager* g_s_soundManager = NULL;

BalloonSoundManager* BalloonSoundManager::sharedBalloonSoundManager()
{
	if (!g_s_soundManager)
	{
		g_s_soundManager = new BalloonSoundManager;

		// preload all effects
		SimpleAudioEngine::sharedEngine()->preloadEffect(SOUND_EFFECT_EXPLOSIVE);
	}

	return g_s_soundManager;
}

void BalloonSoundManager::purgeBalloonSoundManager()
{
	if (g_s_soundManager)
	{
		// unload all effects
		SimpleAudioEngine::sharedEngine()->unloadEffect(SOUND_EFFECT_EXPLOSIVE);

		delete g_s_soundManager;	
		g_s_soundManager = NULL;
	}
}

void BalloonSoundManager::playEffectExplosive()
{
	SimpleAudioEngine::sharedEngine()->playEffect(SOUND_EFFECT_EXPLOSIVE);
}


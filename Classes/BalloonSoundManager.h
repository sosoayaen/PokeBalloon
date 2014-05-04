#ifndef __BALLOON_SOUND_MANAGER_H__
#define __BALLOON_SOUND_MANAGER_H__

class BalloonSoundManager
{
public:
	BalloonSoundManager(){}
	virtual ~BalloonSoundManager(){}

	static BalloonSoundManager* sharedBalloonSoundManager();

	static void purgeBalloonSoundManager();

public:
	/**
	 * @brief 
	 */
	void playEffectExplosive();

};

#endif // __BALLOON_SOUND_MANAGER_H__

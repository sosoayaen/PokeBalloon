#ifndef __BALLOON_SOUND_MANAGER_H__
#define __BALLOON_SOUND_MANAGER_H__

#define SOUND_BACKGROUND_AFTER_SCHOOL       1
#define SOUND_BACKGROUND_AFTER_SCHOOL_FILE  "music/background/bg1.mp3"

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
    
    /**
     * @brief 播放背景音乐
     * @param soundId 对应背景音乐的ID
     */
    void playBackgroundMusic(int soundId);
    
    
    bool isBackgroundMusicPlaying();

};

#endif // __BALLOON_SOUND_MANAGER_H__

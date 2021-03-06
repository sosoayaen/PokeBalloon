#ifndef __BALLOON_SOUND_MANAGER_H__
#define __BALLOON_SOUND_MANAGER_H__

#define SOUND_BACKGROUND_AFTER_SCHOOL       1
#define SOUND_BACKGROUND_AFTER_SCHOOL_FILE  "music/background/bg1.mp3"

#include "Balloon_macro.h"

class BalloonSoundManager
{
public:
	BalloonSoundManager(){}
	virtual ~BalloonSoundManager(){}

	static BalloonSoundManager* sharedBalloonSoundManager();

	static void purgeBalloonSoundManager();

public:
    /**
     * @brief 预加载音效
     */
    void preloadAllEffect();
    
    /**
     * @brief 卸载所有音效
     */
    void unloadAllEffect();
    
	/**
	 * @brief 播放音效
	 */
	void playEffectExplosive();
    
    void playEffectPushBalloon();
    
    void playEffectSpendCoin();
    
    void playEffectFireworksTakeOff();
    
    void playEffectFireworksExplisive();
    
    /**
     * @brief 播放背景音乐
     * @param soundId 对应背景音乐的ID
     */
    void playBackgroundMusic(int soundId);
    
    /**
     * @brief 判断背景音乐是否在播放
     */
    bool isBackgroundMusicPlaying();

    /**
     * @brief 设置背景音乐的音量
     * @param volume 0～1之间的浮点数
     */
    void setBackgroundMusicVolume(float volume);
    
    /**
     * @brief 设置音效的音量
     * @param volume 0～1之间的浮点数
     */
    void setEffectVolume(float volume);
    
    void pauseBackgroundMusic();
    
    void resumeBackgroundMusic();
private:
    
    void playEffectWithFileName(const char* pszEffectFile);

};

#endif // __BALLOON_SOUND_MANAGER_H__

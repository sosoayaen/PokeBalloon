/*
 * =====================================================================================
 *
 *       Filename:  Balloon_macro.h
 *
 *    Description:  快捷宏定义
 *
 *        Version:  1.0
 *        Created:  2014/04/20 11时11分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOON_MACRO_H__
#define __BALLOON_MACRO_H__

/* 
 * @brief 设置布尔型的属性，得到数据使用is开头的函数
 * */
#define CC_SYNTHESIZE_BOOL(varMember, varFunc) \
	protected: bool varMember; \
	public: virtual void set##varFunc##Enable(bool parameter) { varMember = parameter;}\
	public: virtual bool is##varFunc(void) const { return varMember;}

// 全局通知的宏定义
#define NOTIFY_PAUSE    "NOTIFY_PAUSE"      // 程序进入后台
#define NOTIFY_RESUME   "NOTIFY_RESUME"     // 回到程序界面

#define SCORE_COINS_RATE    20          // 分数到金币的比例，被除数

// 全局保存的配置对应的key宏定义
#define OPT_MUSIC_OFF           "opt_MusicOFF"			// 是否关闭背景音乐
#define OPT_SOUND_EFFECT_OFF    "opt_SoundEffectOFF"	// 是否关闭音效

#endif // __BALLOON_MACRO_H__

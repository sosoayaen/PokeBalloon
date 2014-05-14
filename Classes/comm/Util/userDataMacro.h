/************************************************************************/
/* 定义一些用户的数据                                                   */
/* 比如用户的用户名、昵称、积分、token等数据在全局数据表中的key名称     */
/************************************************************************/

#ifndef __USER_DATA_MACRO_H__
#define __USER_DATA_MACRO_H__

#define DATA_KEY_NICK_NAME      "nickname"	// 用户昵称
#define DATA_KEY_ACCOUNT        "account"	// 用户账号
#define DATA_KEY_IDFA			"idfa"		// 苹果广告ID
#define DATA_KEY_UID			"uid"		// 用户ID
#define DATA_KEY_PASSWORD       "password"	// 密码
#define DATA_KEY_PASSWORD_MD5   "pwdMD5"	// 密码
#define DATA_KEY_TOKEN          "token"		// TOKEN
#define DATA_KEY_SEQUENCE       "seq"		// Sequence
#define DATA_KEY_SCORE          "score"		// 积分
#define DATA_KEY_VERSION        "version"	// 版本
#define DATA_KEY_UDID           "udid"		// UDID
#define DATA_KEY_CHANNEL        "channel"	// channel
#define DATA_KEY_CHANNEL_ID     "channelID"	// channelID
#define DATA_KEY_PLATFORM       "platform" // 设备平台
#endif // __USER_DATA_MACRO_H__
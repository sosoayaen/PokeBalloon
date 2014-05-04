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

#endif // __BALLOON_MACRO_H__

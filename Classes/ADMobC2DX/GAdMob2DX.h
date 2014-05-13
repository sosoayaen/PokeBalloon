/*
 * =====================================================================================
 *
 *       Filename:  GAdMob2DX.h
 *
 *    Description:  Google ADMob针对2dx版本通用头文件，Android和iOS平台
 *
 *        Version:  1.0
 *        Created:  2014/05/11 15时42分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __GADMob2DX_H__
#define __GADMob2DX_H__

#include <string>
#include "cocos2d.h"

class GAdMob2DX
{
public:
    /**
     * @brief 得到实例
     */
    static GAdMob2DX* sharedGAdMob2DX();
    
    /**
     * @brief 初始化广告条
     * @param pszUnitID 广告条ID
	 * @param pszPackageName 安卓程序类名，仅针对Android有效
     */
    bool init(const char* pszUnitID, const char* pszPackageName = NULL);
    
    /**
     * @brief 显示隐藏广告条
     */
    void setVisible(bool bShow);

private:
	// Android用于调用JNI的包名
	std::string m_strPackageName;
    
};

#endif // __GADMob2DX_H__

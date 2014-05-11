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
     */
    bool init(const char* pszUnitID);
    
    /**
     * @brief 显示隐藏广告条
     */
    void setVisible(bool bShow);
    
};

#endif // __GADMob2DX_H__
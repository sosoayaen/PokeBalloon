//
//  UMSocial2DX.h
//  PokeBalloon
//
//  Created by Jason Tou on 14-5-11.
//
//

#ifndef PokeBalloon_UMSocial2DX_h
#define PokeBalloon_UMSocial2DX_h

#include "cocos2d.h"

class UMSocial2DX
{
public:
    /**
     * @brief init the UMSocial
     */
    static void setAppKey(const char* pszAppKey, const char* pszPackageName = NULL);
    
    /**
     * @brief 分享内容
     * @param pDictData 分享的信息
     */
    static void shareSNS(cocos2d::CCDictionary* pDictData);

private:

	// 仅仅给Android使用
	static std::string m_strPackageName;
};

#endif

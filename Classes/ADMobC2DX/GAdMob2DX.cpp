#include "GAdMob2DX.h"

#ifndef NULL
#	define NULL 0
#endif

static GAdMob2DX* g_pGAdMobInstance = NULL;

GAdMob2DX* GAdMob2DX::sharedGAdMob2DX()
{
    if (!g_pGAdMobInstance)
    {
        g_pGAdMobInstance = new GAdMob2DX;
    }
    
    return g_pGAdMobInstance;
}

bool GAdMob2DX::init(const char* pszUnitID)
{
    bool bRet = false;

	return bRet;
}

void GAdMob2DX::setVisible(bool bShow)
{

}

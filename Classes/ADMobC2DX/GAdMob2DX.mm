#include "GAdMob2DX.h"
#import "GADBannerView.h"

static GAdMob2DX* g_pGAdMobInstance = NULL;
static GADBannerView* bannerView_ = nil;

GAdMob2DX* GAdMob2DX::sharedGAdMob2DX()
{
    if (!g_pGAdMobInstance)
    {
        g_pGAdMobInstance = new GAdMob2DX;
    }
    
    return g_pGAdMobInstance;
}

bool GAdMob2DX::init(const char* pszUnitID, const char* pszPackageName /* = NULL */)
{
    bool bRet = false;
    
    if (bannerView_ == nil)
    {
        // GADBanner
        bannerView_ = [[GADBannerView alloc] initWithAdSize:kGADAdSizeBanner];
        // bannerView_.delegate = self;
        // Specify the ad's "unit identifier." This is your AdMob Publisher ID.
        bannerView_.adUnitID = [[NSString alloc] initWithUTF8String:pszUnitID];
        
        // Let the runtime know which UIViewController to restore after taking
        // the user wherever the ad goes and add it to the view hierarchy.
        UIViewController *viewController = [UIApplication sharedApplication].keyWindow.rootViewController;
        bannerView_.rootViewController = viewController;
        [viewController.view addSubview:bannerView_];
        
        // Initiate a generic request to load it with an ad.
        GADRequest* adRequest = [GADRequest request];
        [bannerView_ loadRequest:adRequest];
        
        bRet = true;
    }
    
    return bRet;
}

void GAdMob2DX::setVisible(bool bShow)
{
    if (bannerView_ != nil)
    {
        bannerView_.hidden = !bShow;
    }
}
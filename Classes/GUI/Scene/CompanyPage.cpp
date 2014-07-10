#include "CompanyPage.h"
#include "BalloonFirstPage.h"

USING_NS_CC;

#define IMAGE_COMPANY_EN	"texture/company/company_en.png"
#define IMAGE_COMPANY_ZH	"texture/company/company_zh.png"
#define IMAGE_COMPANY_LOGO	"texture/company/company_logo.png"

CompanyPage::CompanyPage()
{
	m_pSpriteEnglishName = NULL;

	m_pSpriteLogo = NULL;

	m_pSpriteChineseName = NULL;

	m_pLayerColorMask = NULL;
}

CompanyPage::~CompanyPage()
{
	CC_SAFE_RELEASE_NULL(m_pSpriteEnglishName);
	CC_SAFE_RELEASE_NULL(m_pSpriteChineseName);
	CC_SAFE_RELEASE_NULL(m_pSpriteLogo);
	CC_SAFE_RELEASE_NULL(m_pLayerColorMask);
}

CCScene* CompanyPage::scene()
{
	CCScene* pScene = CCScene::create();

	pScene->addChild(CompanyPage::create());

	return pScene;
}

bool CompanyPage::init()
{
	bool bRet = false;

	do
	{
		// 初始化为白色底色
		CC_BREAK_IF(!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)));

		setContentSize(CCDirector::sharedDirector()->getVisibleSize());

		// 初始化3个对象
		m_pSpriteEnglishName = CCSprite::create(IMAGE_COMPANY_EN);
		m_pSpriteChineseName = CCSprite::create(IMAGE_COMPANY_ZH);
		m_pSpriteLogo = CCSprite::create(IMAGE_COMPANY_LOGO);

		CC_BREAK_IF(!m_pSpriteLogo || !m_pSpriteChineseName || !m_pSpriteEnglishName);

		m_pSpriteEnglishName->retain();
		m_pSpriteChineseName->retain();
		m_pSpriteLogo->retain();

		// 遮罩层
		m_pLayerColorMask = CCLayerColor::create(ccc4(0, 0, 0, 0));
		m_pLayerColorMask->setContentSize(CCDirector::sharedDirector()->getVisibleSize());

		m_pLayerColorMask->retain();

		bRet = true;
	} while(0);

	return bRet;
}

void CompanyPage::onEnter()
{
	CCLayerColor::onEnter();

	// 得到中心点坐标
	CCSize sizeVisible = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint posCenter = ccpMult(ccpFromSize(sizeVisible), 0.5f);

	// 右边进入
	m_pSpriteLogo->setPosition(ccp(sizeVisible.width, posCenter.y));
	m_pSpriteLogo->runAction(CCEaseBounceOut::create(CCMoveTo::create(1.3f, posCenter)));

	// 左边进入
	m_pSpriteEnglishName->setAnchorPoint(ccp(0.5f, 0));
	m_pSpriteEnglishName->setPosition(ccp(0, m_pSpriteLogo->getPositionY() + m_pSpriteLogo->getContentSize().height*1.0f));
	m_pSpriteEnglishName->runAction(CCEaseBounceOut::create(CCMoveTo::create(1.1f, ccp(posCenter.x, m_pSpriteEnglishName->getPositionY()))));

	// 左边进入
	m_pSpriteChineseName->setAnchorPoint(ccp(0.5f, 1.0f));
	m_pSpriteChineseName->setPosition(ccp(0, m_pSpriteLogo->getPositionY() - m_pSpriteLogo->getContentSize().height*1.0f));
	m_pSpriteChineseName->runAction(CCEaseBounceOut::create(CCMoveTo::create(1.5f, ccp(posCenter.x, m_pSpriteChineseName->getPositionY()))));

	// 整体变暗
	m_pLayerColorMask->runAction(CCSequence::create(CCFadeOut::create(3.0f),
			   CCDelayTime::create(1.0f),
			   CCFadeIn::create(3.0f),
		   	   CCCallFunc::create(this, callfunc_selector(CompanyPage::showGameScene)), NULL));

	addChild(m_pSpriteEnglishName);
	addChild(m_pSpriteChineseName);
	addChild(m_pSpriteLogo);
	addChild(m_pLayerColorMask);
}

void CompanyPage::onExit()
{
	CCLayerColor::onExit();
}

void CompanyPage::showGameScene()
{
	CCDirector::sharedDirector()->replaceScene(BalloonFirstPage::scene());
}

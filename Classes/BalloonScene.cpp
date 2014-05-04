#include "BalloonScene.h"
#include "BalloonSoundManager.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define BALLOON_SHOW_RATE 10     // ������ֵĸ��ʣ�10��ʾ1%��50����5%

BalloonScene::~BalloonScene()
{
}

CCScene* BalloonScene::scene()
{
    CCScene* pScene = CCScene::create();
    
    pScene->addChild(BalloonScene::create());
    
    return pScene;
}

bool BalloonScene::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!CCLayer::init());
		// ����ccbi
		CCNodeLoaderLibrary* pLoaderLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();

		CCBReader* pCCBReader = new CCBReader(pLoaderLib);

		// ��Ӧccbi�ļ�
		std::string strCCBFileName = "BalloonScene.ccbi";

		// �õ���һ������·��
		const std::vector<std::string> vSearchOrder = CCFileUtils::sharedFileUtils()->getSearchResolutionsOrder();

		// ���������Ƕ�Ӧ�ĵ�һ��Ԫ�ؼ�Ϊ��Ӧ����Դ·��
		std::string strCCBRootPath = vSearchOrder[0];

		// ����CCB����Դ·��
		pCCBReader->setCCBRootPath(strCCBRootPath.c_str());

		CCNode* pNode = pCCBReader->readNodeGraphFromFile(strCCBFileName.c_str(), this);

		if (pNode != NULL)
		{
			this->addChild(pNode);
		}

		pCCBReader->release();
        
        resetData();
        
        m_pSpriteBackground->setScaleX(getContentSize().width/m_pSpriteBackground->getContentSize().width);
        m_pSpriteBackground->setScaleY(getContentSize().height/m_pSpriteBackground->getContentSize().height);
		
		setKeypadEnabled(true);
        
        if (!m_BalloonManger.getInitFlag())
            m_BalloonManger.init(this, m_pLayerBalloon);
        
        m_pSpriteBalloonModel->setVisible(false);
		
		bRet = true;
		
	} while(0);
	
	return bRet;
}

void BalloonScene::resetData()
{
    m_ulFrame = 0;
    m_lTotalScore = 0;
    
    m_ulTimeLeft = 120;
    
    updateScore();
    updateTimeLeft();
}

void BalloonScene::updateScore()
{
    m_pLabelTTFScore->setString(CCString::createWithFormat("%ld", m_lTotalScore)->getCString());
}

void BalloonScene::updateTimeLeft()
{
    m_pLabelTTFTime->setString(CCString::createWithFormat("%lu", m_ulTimeLeft)->getCString());
}

void BalloonScene::onEnter()
{
	CCLayer::onEnter();
	// TODO: ������Զ�����볡���ĳ�ʼ��������ؼ��ĳ�ʼλ�ã���ʼ״̬��
    
    // �������㴥���ص�ע��
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), false);
    
    startGame();
    
}

void BalloonScene::onExit()
{
	CCLayer::onExit();
	// TODO: �˳�������ȡ��CCNotificationCenter���Է��������������Ƕ�Ӧ��onEnter��ʱ��Ҫ����ע��
    
    // ֹͣ�����ص�
    unscheduleUpdate();
    
    // ע�����㴥���ص�ע��
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

SEL_CallFuncN BalloonScene::onResolveCCBCCCallFuncSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

SEL_MenuHandler BalloonScene::onResolveCCBCCMenuItemSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonScene::onAssignCCBMemberVariable( CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode )
{
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBalloonModel", CCSprite*, this->m_pSpriteBalloonModel);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelTTFScore", CCLabelTTF*, this->m_pLabelTTFScore);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLabelTTFTime", CCLabelTTF*, this->m_pLabelTTFTime);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pLayerBalloon", CCLayer*, this->m_pLayerBalloon);
	CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "m_pSpriteBackground", CCSprite*, this->m_pSpriteBackground);

	return true;
}

SEL_CCControlHandler BalloonScene::onResolveCCBCCControlSelector( CCObject * pTarget, const char* pSelectorName )
{

	return NULL;
}

bool BalloonScene::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//    m_BalloonManger.touchTest(pTouch->getLocation());
    return true;
}

void BalloonScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::ccTouchCanceled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BalloonScene::balloonHitTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite)
{
    
}

void BalloonScene::balloonTouchTestSuccess(Balloon* pBalloon, cocos2d::CCSprite* pSprite)
{
    switch (pBalloon->getBalloonType())
    {
        case kBalloonTypeNormal:
            // ���ݶ�Ӧ������������ӵ��ܷ���
            m_lTotalScore += pBalloon->getBalloonScore();
            if (m_lTotalScore < 0)
            {
                m_lTotalScore = 0;
            }
            m_pLabelTTFScore->setString(CCString::createWithFormat("%ld", m_lTotalScore)->getCString());
            break;
        case kBalloonTypeMulti:
            // �˷����򣬵�ǰ�����µ�������ͨ���������
            m_BalloonManger.multipleBalloonScore(pBalloon->getBalloonScore());
            break;
        case kBalloonTypeDiv:
            // ��������
            m_lTotalScore /= pBalloon->getBalloonScore();
            if (m_lTotalScore < 0)
            {
                m_lTotalScore = 0;
            }
            m_pLabelTTFScore->setString(CCString::createWithFormat("%ld", m_lTotalScore)->getCString());
            break;
            
        default:
            break;
    }
    
}

void BalloonScene::keyBackClicked( void )
{
	// TODO:
}

void BalloonScene::keyMenuClicked( void )
{
	// TODO:
}

void BalloonScene::update(float dt)
{
    m_ulFrame++;
    
    switch (m_eGameStatus)
    {
        case GAME_STATUS_READY:
            break;
        case GAME_STATUS_RUNNING:
            if (m_ulTimeLeft > 0 && m_ulFrame % int(1/CCDirector::sharedDirector()->getAnimationInterval()) == 0)
            {
                m_ulTimeLeft--;
                updateTimeLeft();
            }
            
            // �ж���Ϸ�Ƿ����
            if (m_ulTimeLeft == 0)
            {
                m_eGameStatus = GAME_STATUS_STOP;
            }
            
            // ���������������Ʈ
            if (rand()%1000 < BALLOON_SHOW_RATE)
            {
                // ����һ������
                m_BalloonManger.addRandomBalloon();
            }
            
            // ��������λ��
            m_BalloonManger.updatePosition();
            
            break;
        case GAME_STATUS_STOP:
            unscheduleUpdate();
            
            m_pLayerBalloon->removeAllChildren();
            
            // ���������
            showResultDialog();
            
            break;
        default:
            break;
    }
    
}

void BalloonScene::showResultDialog()
{
    CCMenuItemFont* pMenuItem = CCMenuItemFont::create("Try again", this,
                                                       menu_selector(BalloonScene::onPressMenuRestartGame));
    CCMenu* pMenu = CCMenu::create(pMenuItem, NULL);
    
    pMenu->setPosition(ccpMult(ccpFromSize(getContentSize()), 0.5f));
    addChild(pMenu);
}

void BalloonScene::onPressMenuRestartGame(cocos2d::CCObject *pSender)
{
    CCNode* pNode = dynamic_cast<CCNode*>(pSender);
    if (!pNode) return;
    
    // �Ƴ���ť��
    pNode->getParent()->removeFromParent();
    
    startGame();
}

void BalloonScene::startGame()
{
    
    resetData();
    
    m_eGameStatus = GAME_STATUS_RUNNING;
    
    scheduleUpdate();
}
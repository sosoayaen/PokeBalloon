#include "DialogLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;


DialogLayer::DialogLayer():
    // m_pDefaultMenu(NULL),
	//    mTouchedMenu(false),
	m_bClickAnyWhereCloseDialog(false),
	m_bOffBackgoundColorBeforeEndDialogAction(false),
	m_pCurrentTouchMenu(NULL),
	m_pCurrentTouchEditBox(NULL),
	m_pCurrentTouchScrollView(NULL),
	m_pVisibleNode(NULL),
	m_pOtherTouchCtl(NULL),
	m_nTouchCount(0),
	m_pBoardOnEnterAction(NULL),
	m_pBoardOnExitAction(NULL),
	m_pCallFuncN(NULL),
	m_pMainBoard(NULL)
{
	m_rectClickValide = CCRectZero;
}

DialogLayer::~DialogLayer()
{
    // 移除可能retain的数据
    CC_SAFE_RELEASE_NULL(m_pBoardOnEnterAction);
    CC_SAFE_RELEASE_NULL(m_pBoardOnExitAction);
    CC_SAFE_RELEASE_NULL(m_pCallFuncN);
}

bool DialogLayer::init()
{
    bool bRet = false;
    
    do {
        CC_BREAK_IF(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 125)));
        
        bRet = true;
    } while (0);
    
    return bRet;
}

void DialogLayer::onEnter()
{
    CCLayerColor::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 2, true);

	// 播放进场动画
	if (m_pMainBoard && m_pBoardOnEnterAction)
	{
		m_pMainBoard->stopAllActions();
		m_pMainBoard->runAction(m_pBoardOnEnterAction);
	}
}

void DialogLayer::onExit()
{
    CCLayerColor::onExit();
	// 自己注销
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool DialogLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	m_nTouchCount++;

	if (m_bClickAnyWhereCloseDialog)
	{
		// 点击任何位置退出的优先级最高
		endDialog();
		return true;
	}

	if (m_rectClickValide.getMaxX() != 0 || m_rectClickValide.getMaxY() != 0)
	{
		// 区域有效，并且点不在区域内，则直接返回，不进行下面的判断
		if (!m_rectClickValide.containsPoint(pTouch->getLocation()))
			return true;
	}

	m_pCurrentTouchMenu = NULL;
	// 循环判断内部每个按钮来响应触屏事件，第一个响应后跳出循环
	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu && (*iterMenu)->ccTouchBegan(pTouch, pEvent))
		{
			// mTouchedMenu = m_pMenu->ccTouchBegan(pTouch, pEvent);
			// 记录前截获触摸消息的按钮
			m_pCurrentTouchMenu = *iterMenu;
			return true;
		}
	}

	// 循环判断是否是其他控件响应消息，比如editbox之类
	m_pCurrentTouchEditBox = NULL;
	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox && isNodeVisible((*iterEditBox)) && (*iterEditBox)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchEditBox = *iterEditBox;
			return true;
		}
	}
	m_pOtherTouchCtl = NULL;
	std::vector<CCLayer*>::iterator iterOtherCtl = m_vOtherCtlArray.begin();
	for (; iterOtherCtl != m_vOtherCtlArray.end(); ++iterOtherCtl)
	{
		if (*iterOtherCtl && isNodeVisible((*iterOtherCtl)) && (*iterOtherCtl)->ccTouchBegan(pTouch, pEvent))
		{
			m_pOtherTouchCtl = *iterOtherCtl;
			return true;
		}
	}
	// 针对ScrollView
	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
        //CCLOG(" visible %d %d",(*iterScrollView)->isVisible() ,(*iterScrollView)->isRunning());
		if (*iterScrollView && isNodeVisible((*iterScrollView)) && (*iterScrollView)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchScrollView = *iterScrollView;
			return true;
		}
	}



	// 判断是否点击在显示面板之外的区域
//	if (m_pVisibleNode)
	if (m_pVisibleNode)
	{
		CCPoint touchLocation = pTouch->getLocation();
		CCPoint local = m_pVisibleNode->convertToNodeSpace(touchLocation);
		// CCPoint local = m_pMainBoard->convertToNodeSpace(touchLocation);

		CCPoint nodePostion = m_pVisibleNode->getPosition();
		// CCPoint nodePostion = m_pMainBoard->getPosition();

		CCSize nodeContentSize = m_pVisibleNode->getContentSize();
		// CCSize nodeContentSize = m_pMainBoard->getContentSize();

		CCRect r = CCRectMake( nodePostion.x - nodeContentSize.width * m_obAnchorPoint.x,
			nodePostion.y - nodeContentSize.height * m_obAnchorPoint.y,
			nodeContentSize.width, nodeContentSize.height);
		r.origin = CCPointZero;

		// 如果当前点不包含在面板内，则直接退出当前界面
		if (!r.containsPoint(local))
		{
			if (m_pCurrentTouchScrollView && m_pCurrentTouchScrollView->isDragging())
			{
				// 如果有ScrollView被选中，并且是在拖放状态，则不退出当前对话框
				return true;
			}
			onClickOutSideOfVisibleBoard();
		}	
	}

	// 吞掉消息
    return true;
}

void DialogLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu)
	{
        m_pCurrentTouchMenu->ccTouchMoved(pTouch, pEvent);
    }

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchMoved(pTouch, pEvent);
	}
}

void DialogLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//	CCLOG("DialogLayer::ccTouchEnded()");
	m_nTouchCount--;

    if (m_pCurrentTouchMenu) 
	{
        m_pCurrentTouchMenu->ccTouchEnded(pTouch, pEvent);
    }
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchEnded(pTouch, pEvent);
	}
 	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchEnded(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;

	if (m_pCurrentTouchScrollView)
	{
		CCLOG("m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);");
		m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);
	}

	if (m_nTouchCount == 0)
	{
		m_pCurrentTouchScrollView = NULL;
	}
}

void DialogLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu) 
	{
		m_pCurrentTouchMenu->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchCancelled(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;
	
	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchScrollView = NULL;
}

void DialogLayer::pushMenu( cocos2d::CCMenu* pMenu )
{
	if (pMenu != NULL)
	{
		m_vMenuArrays.push_back(pMenu);
	}
}

void DialogLayer::pushOtherTouchedCtl( cocos2d::CCLayer* pOtherCtl )
{
	if (pOtherCtl)
	{
		m_vOtherCtlArray.push_back(pOtherCtl);
	}
}

void DialogLayer::SetVisibleBoard( CCNode* pNode )
{
	m_pVisibleNode = pNode;
}

void DialogLayer::pushEditBox( cocos2d::extension::CCEditBox* pEditBox )
{
	if (pEditBox != NULL)
	{
		m_vEditBoxArrays.push_back(pEditBox);
	}
}

/*
*	如果需要播放结束动画，请使用addBoardChildWithAction函数来设定对应的动画对象和动作
*/
bool DialogLayer::endDialog(cocos2d::CCActionInterval* pAction /*= NULL*/, float fDuration /*= 1.0f*/)
{
	if (pAction)
	{
		m_pBoardOnExitAction = pAction;
	}

	if (m_pMainBoard)
	{
		if (m_bOffBackgoundColorBeforeEndDialogAction)
		{
			setOpacity(0);
		}

		if (m_pBoardOnExitAction)
		{
			if (m_pCallFuncN)
			{
				m_pMainBoard->runAction(CCSequence::create(m_pBoardOnExitAction, m_pCallFuncN, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
			}
			else
			{
				// 默认动画播放后移除DialogLayer
				m_pMainBoard->runAction(CCSequence::create(m_pBoardOnExitAction, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
			}
		}
		else
		{
			// 没有出场动画则判断是否有回调
			if (m_pCallFuncN)
			{
				m_pMainBoard->runAction(CCSequence::create(m_pCallFuncN, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
			}
			else
			{
				// 默认动画播放后移除DialogLayer
				m_pMainBoard->runAction(CCSequence::create(CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
			}
		}
	}
	else
	{
		// 如果有动画则执行动画没有则直接删除，这个也可以由子类实现
		// removeFromParentAndCleanup(false);
		if (m_pCallFuncN)
		{
			// this->runAction(CCSequence::create(m_pCallFuncN, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
			m_pCallFuncN->execute();
		}
		ActionCallbackRemoveLayer();
	}
	return true;
}

void DialogLayer::pushScrollView( cocos2d::extension::CCScrollView* pScrollView )
{
	if (pScrollView)
	{
		m_vScrollViewArrays.push_back(pScrollView);
	}
}

bool DialogLayer::popMenu( cocos2d::CCMenu* pMenu )
{
	if (!pMenu)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu == pMenu)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popMenuByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if ((*iterMenu)->getTag() == nTag)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBoxByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if ((*iterEditBox)->getTag() == nTag)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBox( CCEditBox* pEditBox )
{
	if (!pEditBox)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox == pEditBox)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollView( CCScrollView* pScrollView )
{
	if (!pScrollView)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if (*iterScrollView == pScrollView)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollViewByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if ((*iterScrollView)->getTag() == nTag)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtl( CCLayer* pOtherCtl )
{
	if (!pOtherCtl)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if (*iterCtl == pOtherCtl)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtrlByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if ((*iterCtl)->getTag() == nTag)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}

bool DialogLayer::addBoardChildWithAction( cocos2d::CCNode* pChildNode, 
	cocos2d::CCActionInterval* pOnEnterAction /*= NULL*/,
	cocos2d::CCActionInterval* pOnExitAction /*= NULL*/, 
	cocos2d::CCCallFuncN* pAfterExitActionCallbackFuncN /*= NULL*/ )
{
	if (pChildNode)
	{
		m_pMainBoard = pChildNode;
		// 默认当前主面板即为可视面板
		m_pVisibleNode = m_pMainBoard;
		addChild(pChildNode);
	}

	m_pCallFuncN = pAfterExitActionCallbackFuncN;
	if (m_pCallFuncN)
	{
		m_pCallFuncN->retain();
	}

	m_pBoardOnEnterAction = pOnEnterAction;
	if (m_pBoardOnEnterAction)
	{
		m_pBoardOnEnterAction->retain();
	}

	m_pBoardOnExitAction = pOnExitAction;
	if (m_pBoardOnExitAction)
	{
		m_pBoardOnExitAction->retain();
	}

	return true;
}

bool  DialogLayer::isNodeVisible(cocos2d::CCNode* pNode)
{
    if (pNode==NULL)
    {
        return false;
    }
    bool bVisible = pNode->isVisible();
    
    CCNode* pParent = pNode;
    
    while (pParent && bVisible)
    {
        bVisible = pParent->isVisible();
        pParent = pParent->getParent();
    }
    
    return bVisible;
}

void DialogLayer::ActionCallbackRemoveLayer()
{
	removeFromParentAndCleanup(true);
}

void DialogLayer::SetBackgroundColorOffBeforeEndDialogAction( bool bOff )
{
	m_bOffBackgoundColorBeforeEndDialogAction = bOff;
}

void DialogLayer::onClickOutSideOfVisibleBoard()
{
	endDialog();
}

void DialogLayer::setEndCallbackFuncN( cocos2d::CCCallFuncN* pCallbackFuncN )
{
	if (pCallbackFuncN)
	{
		pCallbackFuncN->retain();
	}

	// 先把原有的释放
	if (m_pCallFuncN)
	{
		m_pCallFuncN->release();
	}
	// 设置现有的替换
	m_pCallFuncN = pCallbackFuncN;
}

void DialogLayer::setOnEnterAction( cocos2d::CCActionInterval* pOnEnterAction )
{
	m_pBoardOnEnterAction = pOnEnterAction;
	if (m_pBoardOnEnterAction)
	{
		m_pBoardOnEnterAction->retain();
	}
}

void DialogLayer::setOnExitAction( cocos2d::CCActionInterval* pOnExitAction )
{
	m_pBoardOnExitAction = pOnExitAction;
	if (m_pBoardOnExitAction)
	{
		m_pBoardOnExitAction->retain();
	}
}

void DialogLayer::setCloseDialogClickAnyWhere( bool bEnable )
{
	m_bClickAnyWhereCloseDialog = bEnable;
}

void DialogLayer::setClickValideRect( const cocos2d::CCRect& rect )
{
	m_rectClickValide = rect;
}

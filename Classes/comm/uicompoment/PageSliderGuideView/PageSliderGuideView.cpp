#include "PageSliderGuideView.h"
#include <limits>

USING_NS_CC;
USING_NS_CC_EXT;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#undef max
#endif

#define MAX_DOUBLE std::numeric_limits<double>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()

PageSliderGuideView::PageSliderGuideView():
m_pTableView(NULL)
, m_pArrayData(NULL)
, m_nCurPage(0)
, m_fValidDistance(MAX_FLOAT)
, m_bClickable(true)
, m_nValidPointSize(10)
{
}

PageSliderGuideView::~PageSliderGuideView()
{
	CC_SAFE_RELEASE_NULL(m_pArrayData);
}

CCTableViewCell* PageSliderGuideView::tableCellAtIndex( CCTableView *table, unsigned int idx )
{
	// throw std::exception("The method or operation is not implemented.");
	CCTableViewCell* pCell = table->dequeueCell();

	if (!pCell)
	{
		pCell = new CCTableViewCell;
		pCell->autorelease();
	}

	pCell->removeAllChildren();
	if (pCell)
	{
		if (m_pArrayData)
		{
			CCSpriteFrame* pSpriteFrame = dynamic_cast<CCSpriteFrame*>(m_pArrayData->objectAtIndex(idx));
			if (pSpriteFrame)
			{
				CCSize wantSize = this->getContentSize();
				CCSprite* pSpriteContent = CCSprite::createWithSpriteFrame(pSpriteFrame);
				if (pSpriteContent)
				{
					pCell->removeAllChildren();
					pSpriteContent->setPosition(ccpMult(ccpFromSize(cellSizeForTable(table)), 0.5f));
					pSpriteContent->setScale(wantSize.width/pSpriteContent->getContentSize().width);

					pCell->addChild(pSpriteContent);
				}
			}
			else
			{
				CCSize wantSize = this->getContentSize();
				CCSprite* pSpriteContent = dynamic_cast<CCSprite*>(m_pArrayData->objectAtIndex(idx));
				if (pSpriteContent)
				{
					pSpriteContent->retain();
					pSpriteContent->autorelease();
					pSpriteContent->removeFromParent();
					pCell->removeAllChildren();
					pSpriteContent->setPosition(ccpMult(ccpFromSize(cellSizeForTable(table)), 0.5f));
					pSpriteContent->setScale(wantSize.width/pSpriteContent->getContentSize().width);

					pCell->addChild(pSpriteContent);

				}
			}

		}
	}

	return pCell;
}

unsigned int PageSliderGuideView::numberOfCellsInTableView( CCTableView *table )
{
	unsigned int nRet = 0;
	if (m_pArrayData)
	{
		nRet = m_pArrayData->count();
	}
	return nRet;
}

void PageSliderGuideView::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
	// throw std::exception("The method or operation is not implemented.");
	unsigned int idx = cell->getIdx();

	// if (m_pArrayData && m_pArrayData->count() - 1 == idx)
	// {
	// 	// 点击了最后界面，移除此层
	// 	this->endDialog();
	// }
}

void PageSliderGuideView::scrollViewDidScroll( CCScrollView* view )
{
	// throw std::exception("The method or operation is not implemented.");
}

void PageSliderGuideView::scrollViewDidZoom( CCScrollView* view )
{
	// throw std::exception("The method or operation is not implemented.");
}

cocos2d::CCSize PageSliderGuideView::cellSizeForTable( CCTableView *table )
{
	// throw std::exception("The method or operation is not implemented.");
	return this->getContentSize();
}

bool PageSliderGuideView::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!DialogLayer::init());
		
		setValidDistance(CCDirector::sharedDirector()->getWinSize().width*0.1f);

		// 初始化内部的表格
		m_pTableView = CCTableView::create(this, CCDirector::sharedDirector()->getWinSize(), NULL);
		m_pTableView->setDirection(kCCScrollViewDirectionHorizontal);
		m_pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

		m_pTableView->setDelegate(this);

// 		if (m_pTableView)
// 		{
// 			this->pushScrollView(m_pTableView);
// 		}

		addChild(m_pTableView);

		bRet = true;
	} while (0);

	return bRet;
}

void PageSliderGuideView::onEnter()
{
	DialogLayer::onEnter();
}

void PageSliderGuideView::onExit()
{
	DialogLayer::onExit();
}

void PageSliderGuideView::setImageDataArray( cocos2d::CCArray* pData )
{
	if (pData)
	{
		CC_SAFE_RELEASE_NULL(m_pArrayData);
		m_pArrayData = pData;
		CC_SAFE_RETAIN(m_pArrayData);
		if (m_pTableView)
			m_pTableView->reloadData();
	}
}

PageSliderGuideView* PageSliderGuideView::create( cocos2d::CCArray* pData )
{
	PageSliderGuideView* pRet = new PageSliderGuideView;
	if (pRet && pRet->init())
	{
		pRet->setImageDataArray(pData);
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}


CCRect getBoundingBox(const std::vector<CCPoint> &points)
{
	float minX =  MAX_FLOAT;
	float maxX = -MAX_FLOAT;
	float minY =  MAX_FLOAT; 
	float maxY = -MAX_FLOAT;

	for (std::vector<CCPoint>::const_iterator i = points.begin(); i != points.end(); i++)
	{
		CCPoint point = *i;
		if (point.x < minX)
			minX = point.x;
		if (point.x > maxX)
			maxX = point.x;
		if (point.y < minY)
			minY = point.y;
		if (point.y > maxY)
			maxY = point.y;
	}
	CCRect bounds(minX, minY, (maxX - minX), (maxY - minY));
	return bounds;
}

bool PageSliderGuideView_TouchHandled = false;
void PageSliderGuideView::ccTouchEnded( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	if (PageSliderGuideView_TouchHandled == true)
	{
		//已经处理，退出
		return ;
	}
	DialogLayer::ccTouchEnded(pTouch, pEvent);

	CCPoint pt = pTouch->getLocation();
	unsigned int nSize = m_pathPoints.size();
	CCLOG("Touched point's count:%d", nSize);
	CCRect rect = getBoundingBox(m_pathPoints);

//	if (nSize >= m_nValidPointSize)
	if(rect.size.width + rect.size.height > 30 )
	{

	}
	else//  if (nSize <= 2)
	{
		// 锁定状态下不让点击
//		if (!m_bClickable) return;

        if(m_pTableView->getContainer()->numberOfRunningActions() > 0)
        {
            //如果在做动画就不让操作
            return;
        }
            
        
		if (pt.x < getContentSize().width/3)
		{
			// pszResult = "LeftToRightLine";
			if (m_nCurPage > 0)
			{
				m_nCurPage--;
				if (m_pTableView)
				{
					CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
					m_pTableView->setContentOffsetInDuration(offset, 0.2f);
					CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
					m_bClickable = false;
				}
			}
		}
		else if (pt.x > getContentSize().width*2/3)
		{
			if (m_pArrayData && m_nCurPage < m_pArrayData->count() - 1)
			{
				m_nCurPage++;
				if (m_pTableView)
				{
					CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
					m_pTableView->setContentOffsetInDuration(offset, 0.2f);
					CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
					m_bClickable = false;
				}
			}
		}

		if (m_pArrayData && m_pArrayData->count() - 1 == m_nCurPage && pt.y < getContentSize().height/2 && pt.x >  getContentSize().width/3 && pt.x <  getContentSize().width*2/3 )
		{
			// 关闭引导
			endDialog();
		}
	}
	PageSliderGuideView_TouchHandled = true;
}

bool PageSliderGuideView::ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	PageSliderGuideView_TouchHandled = false;
	bool bRet = DialogLayer::ccTouchBegan(pTouch, pEvent);

	m_pathPoints.clear();
	m_pathPoints.push_back(pTouch->getLocationInView());

	return bRet;
}

void PageSliderGuideView::ccTouchMoved( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchMoved(pTouch, pEvent);

	m_pathPoints.push_back(pTouch->getLocationInView());

	float len = m_pathPoints.back().x - m_pathPoints.front().x;

	if (PageSliderGuideView_TouchHandled == true)
	{
		//已经处理，退出
		return ;
	}


	CCRect rect = getBoundingBox(m_pathPoints);
	if (fabs(len) >= fabs(rect.size.width) && fabs(len) >= m_fValidDistance)
	{
		if (len > 0)
		{
			// pszResult = "LeftToRightLine";
			if (m_nCurPage > 0)
			{
				m_nCurPage--;
				if (m_pTableView)
				{
					CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
					m_pTableView->setContentOffsetInDuration(offset, 0.2f);
					PageSliderGuideView_TouchHandled = true;
					CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
				}
			}
		}
		else
		{
			if (m_pArrayData && m_nCurPage < m_pArrayData->count() - 1)
			{
				m_nCurPage++;
				if (m_pTableView)
				{
					CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
					m_pTableView->setContentOffsetInDuration(offset, 0.2f);
					PageSliderGuideView_TouchHandled = true;
					CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
				}
			}
			else
			{
				// 关闭引导
				endDialog();
				PageSliderGuideView_TouchHandled = true;
			}	
		}
	}

	CCLOG("----------> len :%0.2f, width: %0.2f, pageid: %d,  dis:%0.2f", len, rect.size.width, m_nCurPage, m_fValidDistance);


}

void PageSliderGuideView::ccTouchCanceled( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchCancelled(pTouch, pEvent);

	m_pathPoints.clear();
}

void PageSliderGuideView::setValidDistance( float fDistance )
{
	m_fValidDistance = fDistance;
}

float PageSliderGuideView::getValidDistance()
{
	return m_fValidDistance;
}

void PageSliderGuideView::scrollViewScrollOver( CCScrollView* view )
{
	// 滚动结束后，表明可以继续按下或者继续滑动了
	m_bClickable = true;
}

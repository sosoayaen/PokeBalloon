#include "AutoTextureManagerLayer.h"
// #include "LocalPhotoAccessMgr.h"
// #include "CommandCenter.h"

USING_NS_CC;

AutoTextureManagerLayer::AutoTextureManagerLayer()
{

}

AutoTextureManagerLayer::~AutoTextureManagerLayer()
{

}

bool AutoTextureManagerLayer::init()
{
	bool bRet = false;

	do 
	{
		CC_BREAK_IF(!CCLayer::init());

		CC_BREAK_IF(!setResourceString());

		loadTextureResources();

		bRet = true;
	} while (0);

	return bRet;
}

void AutoTextureManagerLayer::onEnter()
{
	CCLayer::onEnter();

	loadTextureResources();

	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}

void AutoTextureManagerLayer::onExit()
{
	CCLayer::onExit();

	unloadTextureResources();

	// 释放场景的消息队列
//	CommandCenter::sharedCommandCenter()->clearMessageCache();
	// 释放自定义头像缓存
//	LocalPhotoAccessMgr::shareInstance()->cleanMemoryCache();
}

void AutoTextureManagerLayer::loadTextureResources()
{
	for (TextureStringArray::iterator iterTextureFilePath = m_vTexturesString.begin();
		iterTextureFilePath != m_vTexturesString.end();
		iterTextureFilePath++)
	{
		// 判断下是否是直接加载非plist文件，直接加载到CCTextrue中即可
		if (iterTextureFilePath->find(".plist") != std::string::npos)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(iterTextureFilePath->c_str());
		}
		else if (iterTextureFilePath->find(".jpg") != std::string::npos ||
			iterTextureFilePath->find(".jpeg") != std::string::npos ||
			iterTextureFilePath->find(".png") != std::string::npos)
		{
			CCTextureCache::sharedTextureCache()->addImage(iterTextureFilePath->c_str());
		}
		else if ( iterTextureFilePath->find(".pvr") != std::string::npos)
		{
			CCTextureCache::sharedTextureCache()->addPVRImage(iterTextureFilePath->c_str());
		}

	}
}

void AutoTextureManagerLayer::unloadTextureResources()
{
	// 释放纹理资源
	for (std::vector<std::string>::iterator iterTextureFilePath = m_vTexturesString.begin();
		iterTextureFilePath != m_vTexturesString.end();
		iterTextureFilePath++)
	{
		if (iterTextureFilePath->find(".plist") != std::string::npos)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(iterTextureFilePath->c_str());
			CCDictionary* pDict = CCDictionary::createWithContentsOfFile(iterTextureFilePath->c_str());
			CCDictionary* pDictMetaData = dynamic_cast<CCDictionary*>(pDict->objectForKey("metadata"));
			if (pDictMetaData)
			{
				const char* pszTextureFilePath = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(
					pDictMetaData->valueForKey("textureFileName")->getCString(),
					iterTextureFilePath->c_str());

				CCTextureCache::sharedTextureCache()->removeTextureForKey(pszTextureFilePath);
			}
		}
		else
		{
			CCTextureCache::sharedTextureCache()->removeTextureForKey(iterTextureFilePath->c_str());
		}
	}
}

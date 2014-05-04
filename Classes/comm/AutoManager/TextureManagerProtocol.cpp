#include "TextureManagerProtocol.h"
#include "cocos2d.h"

USING_NS_CC;

TextureManagerProtocol::TextureManagerProtocol()
{

}

TextureManagerProtocol::~TextureManagerProtocol()
{

}

void TextureManagerProtocol::loadTextureResources()
{
	for (TextureStringArray::iterator iterTextureFilePath = m_vTexturesString.begin();
		iterTextureFilePath != m_vTexturesString.end();
		iterTextureFilePath++)
	{
		// �ж����Ƿ���ֱ�Ӽ��ط�plist�ļ���ֱ�Ӽ��ص�CCTextrue�м���
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

void TextureManagerProtocol::unloadTextureResources()
{
	// �ͷ�������Դ
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


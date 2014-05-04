#ifndef __TEXTURE_MANAGER_PROTOCOL_H__
#define __TEXTURE_MANAGER_PROTOCOL_H__

#include <string>
#include <vector>

// 资源字符串列表
typedef std::vector<std::string> TextureStringArray;

class TextureManagerProtocol
{
public:
	TextureManagerProtocol();
	virtual ~TextureManagerProtocol();

	/**
	 * @brief 加载纹理资源，对应的资源描述存在 m_vTexturesString 之中
	 */
	virtual void loadTextureResources();

	/**
	 * @brief 卸载纹理资源
	 */
	virtual void unloadTextureResources();	

protected:

	/**
	 * @brief 设置资源文件列表
	 */
	virtual bool setResourceString() = 0;

protected:

	// 纹理资源数组
	TextureStringArray m_vTexturesString;

private:

};

#endif
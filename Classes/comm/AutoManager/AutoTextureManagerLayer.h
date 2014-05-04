/**
 * @author: Jason Tou
 * @date:   28/3/2014
 * @email:  sosoayaen@gmail.com
 *
 * AutoTextureManagerLayer 是场景的基类，主要负责完成资源的管理和释放
 * 如果继承这个基类需要做如下事情
 * 1. 必须实现setResourceString虚函数，其内部需要直接设置m_vTexturesString变量数据
 *    增加当前场景需要管理的纹理数据，直接push_back添加即可。
 *    对应的数据可以是plist的纹理描述或者直接是图片资源文件路径
 *
 * 2. 最好异步加载资源，异步加载这个还比较麻烦，因为这里设置的资源字符串是不能在外部得到
 *
 * warnning: 目前的版本中，对应的资源加载会在init函数后做一次，然后在onEnter函数中再执
 *           行一次，而卸载资源的函数会在onExit后执行一次。这是此基类默认做的事情，所
 *           以，一般继承的子类在onExit中不要操作纹理，因为首先纹理已经被卸载了
 */
#ifndef __AUTO_TEXTURE_MANAGER_LAYER_H__
#define __BAILIN_BASE_SCENE_H__

#include "cocos2d.h"
// #include "TextureManagerProtocol.h"

// 资源字符串列表
typedef std::vector<std::string> TextureStringArray;

class AutoTextureManagerLayer :
//	public TextureManagerProtocol,
	public cocos2d::CCLayer
{
public:
	AutoTextureManagerLayer();
	virtual ~AutoTextureManagerLayer();

	virtual bool init();

	virtual void onEnter();
	virtual void onExit();

protected:
	/**
	 * @brief 设置资源文件列表
	 */
	virtual bool setResourceString() = 0;

	/**
	 * @brief 加载纹理资源，对应的资源描述存在 m_vTexturesString 之中
	 */
	virtual void loadTextureResources();

	/**
	 * @brief 卸载纹理资源
	 */
	virtual void unloadTextureResources();

protected:

	// 纹理资源数组
	TextureStringArray m_vTexturesString;

private:
};
#endif // __BAILIN_BASE_SCENE_H__
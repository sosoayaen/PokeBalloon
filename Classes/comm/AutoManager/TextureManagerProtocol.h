#ifndef __TEXTURE_MANAGER_PROTOCOL_H__
#define __TEXTURE_MANAGER_PROTOCOL_H__

#include <string>
#include <vector>

// ��Դ�ַ����б�
typedef std::vector<std::string> TextureStringArray;

class TextureManagerProtocol
{
public:
	TextureManagerProtocol();
	virtual ~TextureManagerProtocol();

	/**
	 * @brief ����������Դ����Ӧ����Դ�������� m_vTexturesString ֮��
	 */
	virtual void loadTextureResources();

	/**
	 * @brief ж��������Դ
	 */
	virtual void unloadTextureResources();	

protected:

	/**
	 * @brief ������Դ�ļ��б�
	 */
	virtual bool setResourceString() = 0;

protected:

	// ������Դ����
	TextureStringArray m_vTexturesString;

private:

};

#endif
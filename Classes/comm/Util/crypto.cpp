#include "crypto.h"
#include "crc32.h"
#include "blowfish.h"

NS_BAILIN_UTIL_BEGIN

std::string crypto::MD5(const char* pszSrc)
{
	char szResult[33] = {0};
	CMD5Encrypt::EncryptData((char*)pszSrc, szResult);
	return szResult;
}

std::string crypto::Base64Encode( unsigned char const* bytes_to_encode, unsigned int in_len )
{
	return base64_encode(bytes_to_encode, in_len);
}

std::string crypto::Base64Decode( std::string const& encoded_string )
{
	return base64_decode(encoded_string);
}

std::string crypto::getXORString( const char* key, const std::string& strValue )
{
	std::string retStr;
	retStr.clear();
	int nLen = strValue.length();
	int count = strlen(key);
	const char* pStr = strValue.c_str();

	for (int i = 0; i< nLen; i++)
	{
		int j = i%count;
		char  ch = pStr[i]^key[j];
		retStr = retStr + ch;
	}
	return retStr;
}

unsigned long crypto::Crc32(const void *buf, size_t bufLen)
{
    return Crc32_ComputeBuf(0, buf, bufLen);
}

// BlowFish 加密密钥
static const char* s_szBFKey = "BlowFishEncryptKey";
unsigned char* crypto::BlowfishEncode(const unsigned char *pBuffer, unsigned int in_len, unsigned int &out_len, const char* pszKey /* = NULL */)
{
    CBlowFish bf;
    // 初始化密钥啥的
    if (!pszKey)
        bf.Initialize((unsigned char*)s_szBFKey, strlen(s_szBFKey));
    else
        bf.Initialize((unsigned char*)pszKey, strlen(pszKey));
    // 得到加密后数据的长度
    out_len = bf.GetOutputLength(in_len);
    unsigned char* pszBuff = new unsigned char[out_len];
    // 加密数据
    bf.Encode((unsigned char*)pBuffer, (unsigned char*)pszBuff, in_len);
    return pszBuff;
}

// BlowFish 解密
unsigned char* crypto::BlowfishDecode(const unsigned char *pBuffer, unsigned int in_len, unsigned int &out_len, const char* pszKey /* = NULL */)
{
    CBlowFish bf;
    // 初始化密钥啥的
    if (!pszKey)
        bf.Initialize((unsigned char*)s_szBFKey, strlen(s_szBFKey));
    else
        bf.Initialize((unsigned char*)pszKey, strlen(pszKey));
    // 得到加密后数据的长度
    out_len = bf.GetOutputLength(in_len);
    unsigned char* pszBuff = new unsigned char[out_len];
    bf.Decode((unsigned char*)pBuffer, (unsigned char*)pszBuff, in_len);
    return pszBuff;
}

NS_BAILIN_UTIL_END
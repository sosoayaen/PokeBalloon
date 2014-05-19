#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include "bailinMacro.h"

#include "Encrypt.h"
#include "Base64.h"
#include <string>

NS_BAILIN_UTIL_BEGIN

class crypto
{
public:
	/**
	* @brief 字符串MD5编码
	* @param pszSrc 待编码的字符串
	* @return string 返回字符串
	*/
	static std::string MD5(const char* pszSrc);

	static std::string Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len);

	static std::string Base64Decode(std::string const& encoded_string);

	static std::string getXORString(const char* key, const std::string& strValue);
    
    /**
     * @brief 返回某个小于8192字节缓存数据的CRC32核
     * @param buf 缓存指针
     * @param bufLen 缓存大小
     * @return unsigned long 校验码
     */
    static unsigned long Crc32(const void *buf, size_t bufLen );
};

NS_BAILIN_UTIL_END

#endif // __CRYPTO_H__
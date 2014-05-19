#include "crypto.h"
#include "crc32.h"

std::string bailin::util::crypto::MD5(const char* pszSrc)
{
	char szResult[33] = {0};
	CMD5Encrypt::EncryptData((char*)pszSrc, szResult);
	return szResult;
}

std::string bailin::util::crypto::Base64Encode( unsigned char const* bytes_to_encode, unsigned int in_len )
{
	return bailin::util::base64_encode(bytes_to_encode, in_len);
}

std::string bailin::util::crypto::Base64Decode( std::string const& encoded_string )
{
	return bailin::util::base64_decode(encoded_string);
}

std::string bailin::util::crypto::getXORString( const char* key, const std::string& strValue )
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

int bailin::util::crypto::Crc32(const void *buf, size_t bufLen)
{
    return Crc32_ComputeBuf(0, buf, bufLen);
}
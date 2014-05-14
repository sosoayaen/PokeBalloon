//
//  Base64.h
//
//
#ifndef _BASE64__H_
#define _BASE64__H_


#include "bailinMacro.h"
#include <string>

NS_BAILIN_UTIL_BEGIN
/*******************************************************************
 函 数 名 称:  base64_encode
 功 能 描 述：  Base64编码
 参 数 说 明：  bytes_to_encode:    输入的BYTE流[in]
 in_len:             字节长度[in]
 
 返回值 说明：   编码后std::string
 作       者:    huangjf
 更 新 日 期：  2009.12.8
 *******************************************************************/
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);



/*******************************************************************
 函 数 名 称:  base64_decode
 功 能 描 述：  Base64解码
 参 数 说 明：  encoded_string: 输入的BYTE流[in]
 
 返回值 说明：   解码后std::string
 作       者:    huangjf
 更 新 日 期：  2009.12.8
 *******************************************************************/
std::string base64_decode(std::string const& encoded_string);

NS_BAILIN_UTIL_END

#endif /*_BASE64__H_*/
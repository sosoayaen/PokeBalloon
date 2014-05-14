#ifndef ENCRYPT_HEAD_FILE
#define ENCRYPT_HEAD_FILE

// #include "definition.h"
#define CountArray(array) sizeof(array)/sizeof(array[0])

//////////////////////////////////////////////////////////////////////////

//MD5 加密类 
class  CMD5Encrypt
{
	//函数定义
private:
	//构造函数
	CMD5Encrypt() {}
    
	//功能函数
public:
	//生成密文
	static void EncryptData(char* pszSrcData, char szMD5Result[33]);
};

//////////////////////////////////////////////////////////////////////////

//异或加密类
class  CXOREncrypt
{
	//函数定义
private:
	//构造函数
	CXOREncrypt() {}
    
	//功能函数
public:
	//生成密文
    

	static unsigned short EncryptData(char* pszSrcData, char* pszEncrypData, unsigned short wSize);
	//解开密文
	static unsigned short CrevasseData(char* pszEncrypData, char* pszSrcData, unsigned short wSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
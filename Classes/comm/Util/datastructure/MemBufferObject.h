#pragma once
#include "cocos2d.h"
#include "MemBuffer.h"

class MemBufferObject :
	public bailin::MemBuffer, 
	public cocos2d::CCObject
{
public:
	MemBufferObject(void);
	virtual ~MemBufferObject(void);

	static MemBufferObject* create();

	static MemBufferObject* create(const char* pBuffer, int nSize);


};


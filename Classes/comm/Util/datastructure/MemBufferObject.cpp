#include "MemBufferObject.h"


MemBufferObject::MemBufferObject(void)
{
}


MemBufferObject::~MemBufferObject(void)
{
}

MemBufferObject* MemBufferObject::create()
{
	MemBufferObject* pMemBufferObject = new MemBufferObject;
	if (pMemBufferObject)
	{
		pMemBufferObject->autorelease();
	}
	
	return pMemBufferObject;
}

MemBufferObject* MemBufferObject::create( const char* pBuffer, int nSize )
{
	MemBufferObject* pMemBufferObject = new MemBufferObject;
	if (pMemBufferObject)
	{
		pMemBufferObject->Append(pBuffer, nSize);
		pMemBufferObject->autorelease();
	}

	return pMemBufferObject;
}

//////////////////////////////////////////////////////////////////////////
// @@Author: Jason Tou
// @@Date:2013/04/26
// @@Description:
//     ���ݻ���
//////////////////////////////////////////////////////////////////////////

#ifndef __MEM_BUFFER_H__
#define __MEM_BUFFER_H__

#ifndef NULL
	#define NULL 0
#endif

#include "cocos2d.h"

namespace bailin
{
	class MemBuffer// : public cocos2d::CCObject  
	{
	public:
		MemBuffer(int nInitLen = 16);
		MemBuffer(const char* pData, int nLen = 0);
		MemBuffer(const MemBuffer& buff);
		virtual ~MemBuffer();

	public:
		virtual int GetBufferLen();
		virtual bool Append(const char* pData, int nLen = 0);
		virtual char* GetBuffer();
		virtual void Clear();

		// ������Ƴ�����
		virtual bool RemoveLeft(int nLen);
		// virtual bool RemoveRight(int nLen);

		// ����ߵ�������
		virtual bool PopLeft(MemBuffer* pBuffer, int nLen);

		bool ReAllocBuffer(int nDeltaLen);
	protected:

	protected:
		char* m_pBuffer;	// buffer data
		int m_nCurLen;
		int m_nMaxLen;
	};
}

#endif // __MEM_BUFFER_H__
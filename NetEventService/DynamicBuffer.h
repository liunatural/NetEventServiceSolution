#pragma once
#include <vector>
#include "consts.h"

class DynamicBuffer
{
public:
	DynamicBuffer()
	{
		m_buff_len = MAX_DATA_BUFFER_LEN;

		m_StreamBuffer.reserve(m_buff_len);
		m_offset = 0;
	}

	void Push(const void * pBuf, int nLen)
	{
		char* pBuf2 = (char*)pBuf;

		m_StreamBuffer.insert(m_StreamBuffer.end(), pBuf2, pBuf2 + nLen);
	}

	void Pop(int nLen)
	{
		m_offset += nLen;
		if (m_offset > m_buff_len)
		{
			m_StreamBuffer.erase(m_StreamBuffer.begin(), m_StreamBuffer.begin() + m_offset);
			m_offset = 0;
			m_StreamBuffer.reserve(m_buff_len);
		}
	}

	char* Peek()
	{
		if (m_StreamBuffer.empty())
		{
			return NULL;
		}

		return &m_StreamBuffer[0] + m_offset;
	}

	int Size()
	{
		return (int)m_StreamBuffer.size() - m_offset;
	}

	void Clear()
	{
		m_StreamBuffer.clear();
	}

private:
	std::vector<char> m_StreamBuffer;
	int m_offset;
	int m_buff_len;
};


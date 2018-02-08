#include "circularbuffer.h"
#include "../log/log.h"

CCircularBuffer::CCircularBuffer(int size)
{
	m_nDataSize = 0;
	m_nWritePos = 0;
	m_nReadPos = 0;
	m_nSize = size;
	m_buf = NULL;
	m_buf = new char[size];
	if (!m_buf)
	{
		INFO("CCircularBuffer::CCircularBuffer new error");
		return;
	}
}

CCircularBuffer::~CCircularBuffer()
{
	if (NULL != m_buf)
	{
		delete[] m_buf;
		m_buf = NULL;
	}
}

int CCircularBuffer::read(void* buf, int len)
{
	if (m_nDataSize < len)
	{
		INFO("CCircularBuffer::read 没有足够的数据, datasize:%d, len:%d", m_nDataSize, len);
		return 0;
	}

	int tempReadPos = m_nReadPos;
	if (tempReadPos < m_nWritePos)
	{
		memcpy(buf, &m_buf[tempReadPos], len);
		tempReadPos += len;
	}
	else
	{
		if (m_nSize - tempReadPos >= len)
		{
			memcpy(buf, &m_buf[tempReadPos], len);
			tempReadPos += len;
		}
		else
		{
			int endLen = m_nSize - tempReadPos;
			memcpy(buf, &m_buf[tempReadPos], endLen);
			tempReadPos = 0;
			
			memcpy((char*)buf + endLen, &m_buf[tempReadPos], len - endLen);
			tempReadPos += len - endLen;
		}
	}
	if (tempReadPos >= m_nSize)
	{
		tempReadPos = tempReadPos - m_nSize;
	}
	return len;
}

int CCircularBuffer::write(void* buf, int len)
{
	if (NULL == buf || len <= 0)
	{
		INFO("CCircularBuffer::write error");
		return 0;
	}

	int canWriteLen = 0;
	int wantToWrite = len;
	int hasSend = 0;
	char* pSend = (char*)buf;
	while (wantToWrite > 0)
	{
		while (canWriteLen = getCanWriteLen() <= 0)
		{
			if (!reallocMem())
			{
				return 0;
			}
		}

		if (canWriteLen >= wantToWrite)
		{
			memcpy(&m_buf[m_nWritePos], &pSend[hasSend], wantToWrite);
			setCurWritePos(wantToWrite);
			hasSend += wantToWrite;
			wantToWrite -= wantToWrite;
		}
		else
		{
			memcpy(&m_buf[m_nWritePos], &pSend[hasSend], canWriteLen);
			setCurWritePos(canWriteLen);
			hasSend += canWriteLen;
			wantToWrite -= canWriteLen;
		}
	}
	return hasSend;
}

void CCircularBuffer::clear()
{
	m_nDataSize = 0;
	m_nReadPos = 0;
	m_nWritePos = 0;
}

int CCircularBuffer::getCanReadLen()
{
	if (0 == m_nDataSize)
	{
		return 0;
	}

	if (m_nWritePos == m_nReadPos)
	{
		if (0 == m_nWritePos)
		{
			return m_nSize;
		}
		else
		{
			return m_nSize - m_nWritePos;
		}
	}
	else if (m_nWritePos > m_nReadPos)
	{
		return m_nWritePos - m_nReadPos;
	}
	else
	{
		return m_nSize - m_nReadPos;
	}
	return 0;
}

int CCircularBuffer::getCanWriteLen()
{
	if (m_nReadPos == m_nWritePos)
	{
		if (0 == m_nWritePos)
		{
			if (0 == m_nDataSize)
			{
				return m_nSize;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (0 == m_nDataSize)
			{
				return m_nSize - m_nWritePos;
			}
			else
			{
				return 0;
			}
		}
	}
	else if (m_nReadPos > m_nWritePos)
	{
		return m_nReadPos - m_nWritePos;
	}
	else
	{
		return m_nSize - m_nWritePos;
	}
	return 0;
}

bool CCircularBuffer::reallocMem()
{
	int newSize = m_nSize * 2;
	char* pNewBuf = new char[newSize];
	if (NULL == pNewBuf)
	{
		INFO("CCircularBuffer::reallocMem size:%d err", newSize);
		return false;
	}

	memcpy(pNewBuf, m_buf, m_nWritePos);
	int nNeedReadLen = m_nSize - m_nReadPos;
	int newReadPos = newSize = nNeedReadLen;
	memcpy(&pNewBuf[newReadPos], &m_buf[m_nReadPos], nNeedReadLen);
	m_nReadPos = newReadPos;
	delete[] m_buf;
	m_buf = NULL;

	m_nSize = newSize;
	m_buf = pNewBuf;
	return true;
}

void CCircularBuffer::setCurReadPos(int len)
{
	if (len > 0)
	{
		m_nDataSize -= len;
		m_nReadPos += len;
		if (m_nReadPos >= m_nSize)
		{
			m_nReadPos = m_nReadPos - m_nSize;
		}
	}
}

void CCircularBuffer::setCurWritePos(int len)
{
	if (len > 0)
	{
		m_nDataSize += len;
		m_nWritePos += len;
		if (m_nWritePos >= m_nSize)
		{
			m_nWritePos = m_nWritePos - m_nSize;
		}
	}
}
#include "buffer.h"
#include "../structmsg/structmsg.h"

CBuffer::CBuffer(int size) : m_buf(size)
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
}

CBuffer::~CBuffer()
{

}

int CBuffer::write(void* buf, int len)
{
	if (NULL == buf || len <= 0)
	{
		return 0;
	}

	pthread_mutex_lock(&m_mutex);

	int ret = m_buf.write(buf, len);

	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
	return ret;
}

int CBuffer::read(void* buf, int len)
{
	int getLen = 0, headLen = sizeof(TSocketInfo), dataLen = 0;

	pthread_mutex_lock(&m_mutex);
	while (true)
	{
		if (m_buf.getDataLen() < headLen)
		{
			printf("cur=%d, headlen=%d\n", m_buf.getDataLen(), headLen);
			pthread_cond_wait(&m_cond, &m_mutex);
		}
		else
		{
			getLen = m_buf.read(buf, headLen);
			if (getLen >= headLen)
			{
				m_buf.setCurReadPos(headLen);
				break;
			}
		}
	}


	pthread_mutex_unlock(&m_mutex);

	TSocketInfo* pInfo = (TSocketInfo*)buf;

	dataLen = pInfo->size - headLen;

	if (dataLen > 0)
	{
		pthread_mutex_lock(&m_mutex);
		while (true)
		{
			if (m_buf.getDataLen() < dataLen)
			{
				pthread_cond_wait(&m_cond, &m_mutex);
			}
			else
			{
				getLen = m_buf.read((char*)buf + headLen, dataLen);
				if (getLen >= dataLen)
				{
					m_buf.setCurReadPos(dataLen);
					break;
				}
			}
		}
		pthread_mutex_unlock(&m_mutex);
	}
	return pInfo->size;
}
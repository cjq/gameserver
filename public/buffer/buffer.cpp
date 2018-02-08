#include "buffer.h"

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

}
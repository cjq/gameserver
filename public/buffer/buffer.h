#ifndef __PUBLIC_BUFFER_BUFFER_H__
#define __PUBLIC_BUFFER_BUFFER_H__

#include <iostream>
#include <pthread.h>
#include "circularbuffer.h"

class CBuffer
{
public:
	CBuffer(int size);

	~CBuffer();

	int read(void* buf, int len);

	int write(void* buf, int len);

private:
	CCircularBuffer m_buf;
	pthread_cond_t  m_cond;
	pthread_mutex_t m_mutex;
};

#endif
#include "lock.h"

CGameLock::CGameLock()
{
	pthread_mutex_init(&m_lock, NULL);
}

CGameLock::~CGameLock()
{
	pthread_mutex_destroy(&m_lock);
}

void CGameLock::lock()
{
	pthread_mutex_lock(&m_lock);
}

void CGameLock::unlock()
{
	pthread_mutex_unlock(&m_lock);
}

CSafeLock::CSafeLock(CGameLock* l)
{
	m_lock = l;
	if (m_lock)
	{
		m_lock->lock();
	}
}

CSafeLock::~CSafeLock()
{
	if (m_lock)
	{
		m_lock->unlock();
	}
}
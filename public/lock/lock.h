#ifndef __PUBLIC_LOCK_H__
#define __PUBLIC_LOCK_H__

#include <pthread.h>

class CGameLock
{
public:
	CGameLock();

	~CGameLock();

	void lock();
	void unlock();
private:
	pthread_mutex_t m_lock;
};

class CSafeLock
{
public:
	CSafeLock(CGameLock* l);
	~CSafeLock();
private:
	CGameLock* m_lock;
};

#endif
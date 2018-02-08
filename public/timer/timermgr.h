#ifndef __PUBLIC_TIMERMGR_H__
#define __PUBLIC_TIMERMGR_H__

#include "../lock/lock.h"
#include "timer.h"

#include <map>
#include <pthread.h>

class CTimerMgr
{
public:
	static CTimerMgr& getInstance();

	void print_timer_info();

	void delete_all_timer();

	bool addTimer(uint32_t timerid, uint32_t delay, uint32_t periodic_delay, timer_cb user_cb, void* params);

	bool delete_timer(uint32_t timerid);

private:
	CTimerMgr();

	~CTimerMgr();

	static void* timer_out_thread(void* p);

	void _thread_handle();

private:
	CGameLock m_lock;
	std::map<int, timer*> m_mapTimers;
	pthread_t threadid;

	static wheel_timer* s_timer_handle;
};

wheel_timer* CTimerMgr::s_timer_handle = 0;

#endif
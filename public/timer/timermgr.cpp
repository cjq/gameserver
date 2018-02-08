#include "timermgr.h"
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


CTimerMgr::CTimerMgr()
{
	pthread_create(&threadid, NULL, &CTimerMgr::timer_out_thread, this);
}

CTimerMgr::~CTimerMgr()
{

}

CTimerMgr& CTimerMgr::getInstance()
{
	static CTimerMgr mgr;
	return mgr;
}

bool CTimerMgr::addTimer(uint32_t timerid, uint32_t delay, uint32_t periodic_delay, timer_cb user_cb, void* params)
{
	CSafeLock l(&m_lock);
	timer *tmr = new timer();
	if (NULL == tmr)
	{
		return false;
	}

	CTimer::timer_prepare(tmr);
	int ret = CTimer::timer_start(s_timer_handle, tmr, delay, periodic_delay, user_cb, params);
	if (TIMER_STATUE_OK == ret)
	{
		m_mapTimers.insert(std::make_pair(timerid, tmr));
	}
	return ret == TIMER_STATUE_OK ? true : false;
}


bool CTimerMgr::delete_timer(uint32_t timerid)
{
	CSafeLock l(&m_lock);
	std::map<int, timer*>::iterator it = m_mapTimers.find(timerid);
	if (it != m_mapTimers.end())
	{
		CTimer::timer_stop( s_timer_handle, it->second);
		m_mapTimers.erase(it);
	}
}

void CTimerMgr::delete_all_timer()
{
	CSafeLock l(&m_lock);
	CTimer::timer_destroy(s_timer_handle);
}

void* CTimerMgr::timer_out_thread(void* p)
{
	CTimerMgr* mgr = (CTimerMgr*)p;
	if (mgr)
	{
		mgr->_thread_handle();
	}
	return 0;
}

void CTimerMgr::_thread_handle()
{

}

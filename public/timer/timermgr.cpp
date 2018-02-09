#include "timermgr.h"
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <stdio.h>
#include "../log/log.h"
#include "../util/util.h"

wheel_timer* CTimerMgr::s_timer_handle = 0;

CTimerMgr::CTimerMgr() : m_bRunning(false)
{
	
}

CTimerMgr::~CTimerMgr()
{

}

bool CTimerMgr::initTimer()
{
	if(pthread_create(&threadid, NULL, &CTimerMgr::timer_out_thread, this))
	{
		perror("create check_log_file thread error");
	}

	int ret = CTimer::timer_create(512, 100, "timer", &s_timer_handle);
	if (TIMER_STATUE_OK != ret)
	{
		printf("CTimerMgr::initTimer ret:%d\n", ret);
		return false;
	}
	m_bRunning = true;
	printf("CTimerMgr::initTimer running:%d\n", m_bRunning);
	return true;
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
		printf("CTimerMgr::addTimer timerid:%d err\n", timerid);
		return false;
	}

	CTimer::timer_prepare(tmr);
	int ret = CTimer::timer_start(s_timer_handle, tmr, delay, periodic_delay, user_cb, params);
	if (TIMER_STATUE_OK == ret)
	{
		m_mapTimers.insert(std::make_pair(timerid, tmr));
	}
	else
	{
		printf("CTimerMgr::addTimer timerid:%d timer_start err:%d\n", timerid, ret);
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
	s_timer_handle = NULL;
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
	if (!m_bRunning)
	{
		printf("CTimerMgr::_thread_handle no running\n");
		return;
	}
	printf("s_timer_handle:%p\n", s_timer_handle);
	int ep = epoll_create(10);
	if (-1 == ep)
	{
		printf("epoll_create timer fail\n");
		return;
	}

	epoll_event events[1];
	while (1)
	{
		if (-1 == epoll_wait(ep, events, 1, 100))
		{
			printf("wait epoll error\n");
			return;
		}

		CTimer::timer_tick(s_timer_handle);
		INFO("CTimerMgr::_thread_handle");
	}
}

void CTimerMgr::print_timer_info()
{
	CTimer::timer_stats(s_timer_handle);
}


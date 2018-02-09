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

	/*
	* 打印定时器信息
	*/
	void print_timer_info();

	/*
	* 删除所有定时器
	*/
	void delete_all_timer();

	/*
	* 添加定时器并执行
	* @param timerid 唯一的定时器id
	* @param delay 多少毫秒之后执行
	* @param periodic_delay 间隔多少秒执行
	* @param user_cb 回调函数
	* @param params 回调函数参数
	*/
	bool addTimer(uint32_t timerid, uint32_t delay, uint32_t periodic_delay, timer_cb user_cb, void* params);

	/*
	* 删除定时器
	*/
	bool delete_timer(uint32_t timerid);

	/*
	* 初始化
	*/
	bool initTimer();

private:
	CTimerMgr();

	~CTimerMgr();

	static void* timer_out_thread(void* p);

	void _thread_handle();

private:
	CGameLock m_lock;
	std::map<int, timer*> m_mapTimers;	// 定时器
	pthread_t threadid;
	bool m_bRunning;
	static wheel_timer* s_timer_handle;
};


#define TimerMgr CTimerMgr::getInstance()

#endif
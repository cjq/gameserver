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
	* ��ӡ��ʱ����Ϣ
	*/
	void print_timer_info();

	/*
	* ɾ�����ж�ʱ��
	*/
	void delete_all_timer();

	/*
	* ��Ӷ�ʱ����ִ��
	* @param timerid Ψһ�Ķ�ʱ��id
	* @param delay ���ٺ���֮��ִ��
	* @param periodic_delay ���������ִ��
	* @param user_cb �ص�����
	* @param params �ص���������
	*/
	bool addTimer(uint32_t timerid, uint32_t delay, uint32_t periodic_delay, timer_cb user_cb, void* params);

	/*
	* ɾ����ʱ��
	*/
	bool delete_timer(uint32_t timerid);

	/*
	* ��ʼ��
	*/
	bool initTimer();

private:
	CTimerMgr();

	~CTimerMgr();

	static void* timer_out_thread(void* p);

	void _thread_handle();

private:
	CGameLock m_lock;
	std::map<int, timer*> m_mapTimers;	// ��ʱ��
	pthread_t threadid;
	bool m_bRunning;
	static wheel_timer* s_timer_handle;
};


#define TimerMgr CTimerMgr::getInstance()

#endif
#ifndef __PUBLIC_TIMER_H__
#define __PUBLIC_TIMER_H__

#include "../public/util/util.h"

enum TIMER_STATUE
{
	TIMER_STATUE_OK = 0,
	TIMER_STATUE_NULL_FV,
	TIMER_STATUE_NULL_NAME,
	TIMER_STATUE_NULL_WHEEL,
	TIMER_STATUE_NULL_TMR,
	TIMER_STATUE_INVALID_WHEEL,
	TIMER_STATUE_INVALID_WHEEL_SIZE,
	TIMER_STATUE_INVALID_GRANULARITY,
	TIMER_STATUE_NO_RESOURCES
};

#define TIMER_MIN_WHEEL_SIZE (32)
#define TIMER_MAX_WHEEL_SIZE (4096)

// ��ʱ�������� �Ժ���Ϊ��λ
#define TIMER_MIN_GRANULARITY (1)
#define TIMER_MAX_GRANULARITY (100)

#define TIMER_WHEEL_NAME_LEN (32)

// ÿ���۵Ķ�ʱ������
typedef struct timer_link_t_
{
	struct timer_link_t_ *prev;
	struct timer_link_t_ *next;
} timer_link_t;

// ���ӽṹ��
typedef struct 
{
	char		wheel_name[TIMER_WHEEL_NAME_LEN];
	uint32_t	magic_tag;  // �����Լ��
	uint32_t	wheel_size; // ���Ӵ�С
	uint32_t    spoke_index; // ���Ӳ�λ
	uint32_t	ticks;		 // ����ticks
	uint32_t	granularity; // ÿtick���ٺ���

	// ��ʱ����ز���
	uint32_t	timer_hiwater_mark;
	uint32_t	timer_active;
	uint32_t	timer_cancelled;
	uint32_t	timer_expired;
	uint32_t	timer_starts;

	timer_link_t *spoke;
}wheel_timer;

// ��ʱ���ṹ
typedef struct 
{
	timer_link_t	links;
	uint32_t		rotation_count;
	uint32_t		delay;
	uint32_t		periodic_delay;
	void*			func_ptr;
	void*			params;
} timer;

typedef void (*timer_cb)(timer* tmr, void* param);


class CTimer
{
public:
	/*
	* ��ⶨʱ���Ƿ�������
	*/
	static boolean_t timer_running(timer* tmr);

	/*
	* ��ʼ����ʱ��
	*/
	static void timer_prepare(timer* tmr);

	/*
	* ��ӡ��ʱ�������Ϣ
	*/
	static void timer_stats(const wheel_timer* wtmr);

	/*
	* ���һ����ʱ��������
	*/
	static TIMER_STATUE timer_start(wheel_timer* wtmr, timer* tmr, uint32_t delay, uint32_t periodic_delay, timer_cb cb, void* param);

	/*
	* �ݶ���ʱ��
	*/
	static TIMER_STATUE timer_stop(wheel_timer* wtmr, timer* tmr);

	/*
	* �������ж�ʱ��
	*/
	static TIMER_STATUE timer_destroy(wheel_timer* wtmr);

	/*
	* ������ʱ��
	*/
	static TIMER_STATUE timer_create(uint32_t wheel_size, uint32_t granularity, const char* name, wheel_timer** wtmr);

	/*
	* ��ʱ����ʱִ�к���
	*/
	static void timer_tick(wheel_timer* wtmr);
};

#endif
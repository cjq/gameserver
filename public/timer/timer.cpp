#include "timer.h"
#include "../log/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 把定时器插入到轮询定时器的槽位中
static void tmr_enqueue(wheel_timer* wtmr, timer* tmr, uint32_t delay)
{
	timer_link_t *prev, *spoke;

	uint32_t cursor;
	uint32_t tick;
	uint32_t td;

	if (delay < wtmr->granularity)
	{
		tick = 1;
	}
	else
	{
		tick = (delay / wtmr->granularity);
	}

	td = (tick % wtmr->wheel_size);

	tmr->rotation_count = (tick / wtmr->wheel_size);
	cursor = ((wtmr->spoke_index + td) % wtmr->wheel_size);

	spoke = &wtmr->spoke[cursor];
	prev = spoke->prev;
	tmr->links.next = spoke;
	tmr->links.prev = prev;
	prev->next = (timer_link_t*)tmr;
	spoke->prev = (timer_link_t*)tmr;

	return;
}

void CTimer::timer_stats(const wheel_timer* wtmr)
{
	if (NULL == wtmr)
	{
		return;
	}

	printf("\n%s \n", wtmr->wheel_name );
	printf("       Granularity=%u\n", wtmr->granularity);
	printf("        Wheel Size=%u\n", wtmr->wheel_size);
	printf("        Tick Count=%u\n", wtmr->ticks);
	printf("       Spoke Index=%u\n", wtmr->spoke_index);

	printf("     Active timers=%u\n", wtmr->timer_active);
	printf("    Expired timers=%u\n", wtmr->timer_expired);
	printf("      Hiwater mark=%u\n", wtmr->timer_hiwater_mark);
	printf("    Started timers=%u\n", wtmr->timer_starts);
	printf("  Cancelled timers=%u\n", wtmr->timer_cancelled);
}

boolean_t CTimer::timer_running(timer* tmr)
{
	if (NULL == tmr)
	{
		return false;
	}

	if (NULL != tmr->links.next)
	{
		return true;
	}

	return false;
}

void CTimer::timer_prepare(timer* tmr)
{
	if (tmr)
	{
		tmr->links.prev = NULL;
		tmr->links.next = NULL;
	}
}

TIMER_STATUE CTimer::timer_start(wheel_timer* wtmr, timer* tmr, uint32_t delay, uint32_t periodic_delay, timer_cb cb, void* param)
{
	timer_link_t *prev, *next;
	if (NULL == wtmr)
	{
		return TIMER_STATUE_NULL_WHEEL;
	}

	if (NULL == tmr)
	{
		return TIMER_STATUE_NULL_TMR;
	}

	next = tmr->links.next;
	if (next)
	{
		prev = tmr->links.prev;
		next->prev = prev;
		prev->next = next;
		wtmr->timer_active--;
	}

	tmr->func_ptr = (void*)cb;
	tmr->params = param;
	tmr->delay = delay;
	tmr->periodic_delay = periodic_delay;

	tmr_enqueue(wtmr, tmr, delay);

	wtmr->timer_starts++;
	wtmr->timer_active++;
	if (wtmr->timer_active > wtmr->timer_hiwater_mark)
	{
		wtmr->timer_hiwater_mark = wtmr->timer_active;
	}

	return TIMER_STATUE_OK;
}

TIMER_STATUE CTimer::timer_stop(wheel_timer* wtmr, timer* tmr)
{
	if (NULL == wtmr || NULL == tmr)
	{
		return TIMER_STATUE_NULL_FV;
	}

	timer_link_t *prev, *next;
	next = tmr->links.next;
	if (next)
	{
		prev = tmr->links.prev;
		next->prev = prev;
		prev->next = next;

		tmr->links.next = NULL;
		tmr->links.prev = NULL;

		wtmr->timer_active--;
		wtmr->timer_cancelled++;
	}

	if (NULL != tmr)
	{
		delete tmr;
		tmr = NULL;
	}

	return TIMER_STATUE_OK;
}

void CTimer::timer_tick(wheel_timer* wtmr)
{
	if (NULL == wtmr)
	{
		return;
	}

	timer_link_t *prev, *next, *spoke;
	timer *tmr;
	timer_cb user_cb;

	wtmr->ticks++;

	wtmr->spoke_index = (wtmr->spoke_index + 1) % wtmr->wheel_size;
	spoke = &wtmr->spoke[wtmr->spoke_index];

	tmr = (timer*)spoke->next;

	while ((timer_link_t*)tmr != spoke)
	{
		next = (timer_link_t *)tmr->links.next;
		prev = (timer_link_t *)tmr->links.prev;
		
		if (0 != tmr->rotation_count)
		{
			tmr->rotation_count--;
		}
		else
		{
			prev->next = next;
			next->prev = prev;

			tmr->links.next = NULL;
			tmr->links.prev = NULL;

			wtmr->timer_active--;
			wtmr->timer_expired++;

			user_cb = (timer_cb)tmr->func_ptr;
			(*user_cb)(tmr, tmr->params);

			if (tmr->periodic_delay > 0)
			{
				tmr_enqueue(wtmr, tmr, tmr->periodic_delay);
				wtmr->timer_active++;
			}
		}
		tmr = (timer*)next;
	}
}

TIMER_STATUE CTimer::timer_destroy(wheel_timer* wtmr)
{
	if (NULL == wtmr)
	{
		return TIMER_STATUE_NULL_WHEEL;
	}

	timer_link_t *spoke;

	timer *tmr;
	for (int i = 0; i < wtmr->wheel_size; ++i)
	{
		spoke = &wtmr->spoke[i];
		tmr = (timer*)spoke->next;

		while ((timer_link_t*)tmr != spoke)
		{
			timer_stop(wtmr, tmr);
			tmr = (timer*)spoke->next;
		}
	}

	free(wtmr);
	wtmr = NULL;
	return TIMER_STATUE_OK;
}

TIMER_STATUE CTimer::timer_create(uint32_t wheel_size, uint32_t granularity, const char* name, wheel_timer** wtmr)
{
	if (NULL == wtmr)
	{
		return TIMER_STATUE_NULL_WHEEL;
	}

	timer_link_t *spoke;
	wheel_timer *pWtmr;

	*wtmr = NULL;

	if (wheel_size < TIMER_MIN_WHEEL_SIZE || wheel_size > TIMER_MAX_WHEEL_SIZE)
	{
		return TIMER_STATUE_INVALID_WHEEL_SIZE;
	}

	if (granularity < TIMER_MIN_GRANULARITY || granularity > TIMER_MAX_GRANULARITY)
	{
		return TIMER_STATUE_INVALID_GRANULARITY;
	}

	pWtmr = (wheel_timer *)malloc(sizeof(wheel_timer));
	if (NULL == pWtmr)
	{
		return TIMER_STATUE_NO_RESOURCES;
	}

	pWtmr->spoke = (timer_link_t *)malloc(wheel_size * sizeof(timer_link_t));
	if (NULL == pWtmr->spoke)
	{
		free(pWtmr);
		return TIMER_STATUE_NO_RESOURCES;
	}

	strncpy(pWtmr->wheel_name, name, TIMER_WHEEL_NAME_LEN - 1);
	pWtmr->wheel_name[TIMER_WHEEL_NAME_LEN - 1] = '\0';
	pWtmr->ticks = 0;
	pWtmr->spoke_index = 0;
	pWtmr->granularity = granularity;
	pWtmr->wheel_size = wheel_size;

	pWtmr->timer_hiwater_mark = 0;
	pWtmr->timer_active = 0;
	pWtmr->timer_cancelled = 0;
	pWtmr->timer_expired = 0;
	pWtmr->timer_starts = 0;

	spoke = &pWtmr->spoke[pWtmr->spoke_index];
	for (int i = 0; i < pWtmr->wheel_size; ++i)
	{
		spoke->prev = spoke;
		spoke->next = spoke;

		++spoke;
	}

	*wtmr = pWtmr;
	return TIMER_STATUE_OK;
}





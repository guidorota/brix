/*
 * timer.c
 * Created on: Mar 25, 2014
 * Author: Guido Rota
 *
 * Copyright (c) 2014, Guido Rota
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation and/or 
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "logging.h"
#include "configuration.h"
#include "utils/uniform_allocator.h"
#include "runtime/timer.h"
#include "runtime/tick.h"
#include "runtime/task_scheduler.h"

struct timer_entry {
	enum bx_timer_type timer_type;		///< Type of timer (native or pcode)
	bx_uint64 period_msec;				///< Timer firing period in milliseconds
	bx_uint64 period_ticks;				///< Timer firing period in ticks
	bx_task_id task;					///< Task instance
	bx_uint64 ticks_to_next_timer;		///< Number of ticks between this timer and the next one
	struct timer_entry *next_timer;		///< Next timer entry in the list
};

static bx_uint64 ticks_to_next_to_fire;
static struct timer_entry *next_to_fire;

static struct bx_ualloc *timer_entry_ualloc;
static bx_uint8 timer_entry_storage[TM_TIMER_STORAGE_SIZE];

static void add_to_timer_list(struct timer_entry *new_timer);

static void tick_callback() {
	struct timer_entry *timer_fired;
	bx_uint64 start_tick_count;
	bx_uint64 ticks_elapsed;

	if (next_to_fire == NULL) {
		return;
	}

	bx_tick_pause();

	--ticks_to_next_to_fire;
	while (ticks_to_next_to_fire == 0) {
		start_tick_count = bx_tick_get_count();

		timer_fired = next_to_fire;
		ticks_to_next_to_fire = next_to_fire->ticks_to_next_timer;
		next_to_fire = next_to_fire->next_timer;
		bx_sched_schedule_task(timer_fired->task);

		if (timer_fired->timer_type == BX_TIMER_PERIODIC) {
			add_to_timer_list(timer_fired);
		} else {
			bx_ualloc_free(timer_entry_ualloc, timer_fired);
		}

		ticks_elapsed = bx_tick_get_count() - start_tick_count;
		if (ticks_to_next_to_fire <= ticks_elapsed) {
			ticks_to_next_to_fire = 0;
		} else {
			ticks_to_next_to_fire -= ticks_elapsed;
		}
	}

	bx_tick_resume();
}

bx_int8 bx_timer_init() {
	bx_int8 error;

	ticks_to_next_to_fire = 0;
	next_to_fire = NULL;

	timer_entry_ualloc = bx_ualloc_init(timer_entry_storage,
			TM_TIMER_STORAGE_SIZE, sizeof (struct timer_entry));
	if (timer_entry_ualloc == NULL) {
		return -1;
	}

	error = bx_tick_start(TM_TICK_PERIOD_MS, &tick_callback);
	if (error != 0) {
		return -1;
	}

	return 0;
}

bx_uint64 bx_timer_get_tick_count() {
	return bx_tick_get_count();
}

bx_int8 bx_timer_add_timer(enum bx_timer_type timer_type,
		bx_int64 time_msec, bx_task_id task_id) {
	struct timer_entry *new_timer;

	if (task_id < 0) {
		return -1;
	}

	new_timer = bx_ualloc_alloc(timer_entry_ualloc);
	if (new_timer == NULL) {
		return -1;
	}

	new_timer->task = task_id;
	new_timer->period_msec = time_msec;
	new_timer->period_ticks = time_msec / TM_TICK_PERIOD_MS;
	new_timer->timer_type = timer_type;

	bx_tick_pause();
	add_to_timer_list(new_timer);
	bx_tick_resume();

	return 0;
}

static void add_to_timer_list(struct timer_entry *new_timer) {
	struct timer_entry *timer_list_entry;
	bx_uint64 accumulated_ticks;

	if (next_to_fire == NULL) {
		next_to_fire = new_timer;
		ticks_to_next_to_fire = new_timer->period_msec;
		new_timer->next_timer = NULL;
		new_timer->ticks_to_next_timer = 0;
		return;
	}

	if (new_timer->period_ticks < ticks_to_next_to_fire) {
		new_timer->next_timer = next_to_fire;
		new_timer->ticks_to_next_timer = ticks_to_next_to_fire - new_timer->period_msec;
		ticks_to_next_to_fire = new_timer->period_ticks;
		next_to_fire = new_timer;
		return;
	}

	timer_list_entry = next_to_fire;
	accumulated_ticks = ticks_to_next_to_fire;
	while (timer_list_entry->next_timer != NULL &&
			new_timer->period_ticks < accumulated_ticks + timer_list_entry->ticks_to_next_timer) {
		accumulated_ticks += timer_list_entry->ticks_to_next_timer;
		timer_list_entry = timer_list_entry->next_timer;
	}

	new_timer->next_timer = timer_list_entry->next_timer;
	if (new_timer->next_timer != NULL) {
		new_timer->ticks_to_next_timer =
				new_timer->next_timer->period_ticks - new_timer->period_ticks;
	}
	timer_list_entry->next_timer = new_timer;
	timer_list_entry->ticks_to_next_timer = new_timer->period_ticks - accumulated_ticks;
}

bx_int8 bx_timer_destroy() {
	return bx_tick_stop();
}

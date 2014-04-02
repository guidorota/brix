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
#include "runtime/event_handler.h"

struct timer_entry {
	enum bx_timer_type timer_type;		///< Type of timer (native or pcode)
	bx_uint64 period_msec;				///< Timer firing period in milliseconds
	bx_uint64 period_ticks;				///< Timer firing period in ticks
	struct bx_event_handler *handler;	///< Handler instance
	bx_uint64 ticks_to_next_timer;		///< Number of ticks between this timer and the next one
	struct timer_entry *next_timer;		///< Next timer entry in the list
};

static bx_uint64 tick_count;

static bx_uint64 ticks_to_next_to_fire;
static struct timer_entry *next_to_fire;

static struct bx_ualloc *timer_entry_ualloc;
static bx_uint8 timer_entry_storage[TM_TIMER_STORAGE_SIZE];

static void add_to_timer_list(struct timer_entry *new_timer);

static void tick_callback() {
	++tick_count;

	if (next_to_fire == NULL) {
		return;
	}

	if (--ticks_to_next_to_fire == 0) {
		bx_ev_invoke_handler(next_to_fire->handler);
		//TODO: Continue here
	}
}

bx_int8 bx_tm_init() {
	bx_int8 error;

	tick_count = 0;
	ticks_to_next_to_fire = 0;
	next_to_fire = NULL;

	timer_entry_ualloc = bx_ualloc_init(timer_entry_storage,
			TM_TIMER_STORAGE_SIZE, sizeof (struct timer_entry));
	if (timer_entry_ualloc == NULL) {
		return -1;
	}

	error = bx_tk_start(TM_TICK_PERIOD_MS, &tick_callback);
	if (error != 0) {
		return -1;
	}

	return 0;
}

bx_int64 bx_tm_get_tick_count() {
	return tick_count;
}

bx_int8 bx_tm_add_timer(enum bx_timer_type timer_type,
		bx_int64 period_msec, struct bx_event_handler *handler) {
	struct timer_entry *new_timer;

	if (handler == NULL) {
		return -1;
	}

	new_timer = bx_ualloc_alloc(timer_entry_ualloc);
	if (new_timer == NULL) {
		return -1;
	}

	new_timer->handler = handler;
	new_timer->period_msec = period_msec;
	new_timer->period_ticks = period_msec / TM_TICK_PERIOD_MS;
	new_timer->timer_type = timer_type;
	add_to_timer_list(new_timer);

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

bx_int8 bx_tm_destroy() {
	return bx_tk_stop();
}

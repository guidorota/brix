/*
 * timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

#include "types.h"
#include "runtime/tick.h"
#include "runtime/task_scheduler.h"

enum bx_timer_type {
	BX_TIMER_PERIODIC,
	BX_TIMER_ONE_OFF
};

/**
 * Initializes the timer.
 *
 * @return 0 on successful initialization, -1 on error
 */
bx_int8 bx_tm_init();

/**
 * Returns the number of ticks elapsed since the start of the system.
 *
 * @return Number of ticks since system start
 */
bx_uint64 bx_tm_get_tick_count();

/**
 * Adds a new timer in the system.
 * There are 2 different types of timers: Periodic and One-off.
 * Periodic timers are executed periodically.
 * One-off timers are only fired once.
 *
 * @param timer_type Type of timer (periodic or one-off)
 * @param time_msec Timer fire delay
 * @param task Task to schedule when the timer fires
 *
 * @return 0 on successful addition, -1 on error
 */
bx_int8 bx_tm_add_timer(enum bx_timer_type timer_type,
		bx_int64 time_msec, bx_task_id task_id);

/**
 * Destroys the timer.
 *
 * @return 0 on successful desctruction, -1 on error
 */
bx_int8 bx_tm_destroy();

#endif /* TIMER_H_ */

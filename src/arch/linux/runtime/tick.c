/*
 * tick.c
 * Created on: Mar 26, 2014
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

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "logging.h"
#include "runtime/tick.h"
#include "runtime/critical_section.h"

static struct bx_tick {
	pthread_t thread;
	pthread_attr_t attr;
	bx_tick_callback callback;
	int period_msec;
	bx_uint64 count;
	bx_boolean paused;
} tick;

void *tick_routine(void *arg) {
	int error;
	struct timespec period;

	period.tv_sec = (1000 * 1000 * tick.period_msec) / (1000 * 1000 * 1000);
	period.tv_nsec = (1000 * 1000 * tick.period_msec) % (1000 * 1000 * 1000);

	while (1) {
		bx_critical_enter();

		++tick.count;
		if (!tick.paused) {
			tick.callback();
		}

		bx_critical_exit();

		error = nanosleep(&period, NULL);
		if (error != 0) {
			BX_LOG(LOG_DEBUG, "tick", "Error sleeping: %i", errno);
		}
		pthread_testcancel();
	}

	return NULL;
}

bx_int8 bx_tick_start(bx_int32 period_msec, bx_tick_callback callback) {
	int error;

	BX_LOG(LOG_DEBUG, "tick", "Starting tick process...");
	tick.callback = callback;
	tick.period_msec = period_msec;
	tick.count = 0;

	error = pthread_attr_init(&tick.attr);
	if (error != 0) {
		return -1;
	}
	error = pthread_create(&tick.thread, &tick.attr, tick_routine, NULL);
	if (error != 0) {
		pthread_attr_destroy(&tick.attr);
		return -1;
	}

	tick.paused = BX_BOOLEAN_FALSE;
	BX_LOG(LOG_DEBUG, "tick", "Tick process started");

	return 0;
}

void bx_tick_pause() {
	bx_critical_enter();
	tick.paused = BX_BOOLEAN_TRUE;
	bx_critical_exit();
}

void bx_tick_resume() {
	bx_critical_enter();
	tick.paused = BX_BOOLEAN_FALSE;
	bx_critical_exit();
}

bx_uint64 bx_tick_get_count() {
	bx_uint64 tick_count_copy;

	bx_critical_enter();
	tick_count_copy = tick.count;
	bx_critical_exit();

	return tick_count_copy;
}

bx_int8 bx_tick_stop() {

	BX_LOG(LOG_DEBUG, "tick", "Stopping tick process...");
	pthread_cancel(tick.thread);
	pthread_join(tick.thread, NULL);
	pthread_attr_destroy(&tick.attr);
	BX_LOG(LOG_DEBUG, "tick", "Tick process halted");

	return 0;
}

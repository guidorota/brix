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

static pthread_t thread;
static pthread_attr_t attr;

static bx_tick_callback tick_callback;
static int tick_period_msec;

void *tick_routine(void *arg) {
	int error;
	struct timespec period;

	period.tv_sec = (1000 * 1000 * tick_period_msec) / (1000 * 1000 * 1000);
	period.tv_nsec = (1000 * 1000 * tick_period_msec) % (1000 * 1000 * 1000);

	while (1) {
		tick_callback();
		error = nanosleep(&period, NULL);
		if (error != 0) {
			BX_LOG(LOG_DEBUG, "tick", "Error sleeping: %i", errno);
		}
		pthread_testcancel();
	}

	return NULL;
}

bx_int8 bx_tk_start(bx_int32 period_msec, bx_tick_callback callback) {
	int error;

	BX_LOG(LOG_DEBUG, "tick", "Starting tick process...");
	tick_callback = callback;
	tick_period_msec = period_msec;

	error = pthread_attr_init(&attr);
	if (error != 0) {
		return -1;
	}
	error = pthread_create(&thread, &attr, tick_routine, NULL);
	if (error != 0) {
		pthread_attr_destroy(&attr);
		return -1;
	}

	BX_LOG(LOG_DEBUG, "tick", "Tick process started");

	return 0;
}

bx_int8 bx_tk_stop() {

	BX_LOG(LOG_DEBUG, "tick", "Stopping tick process...");
	pthread_cancel(thread);
	pthread_join(thread, NULL);
	pthread_attr_destroy(&attr);
	BX_LOG(LOG_DEBUG, "tick", "Tick process halted");

	return 0;
}

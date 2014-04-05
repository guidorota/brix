/*
 * critical_section.c
 * Created on: Apr 2, 2014
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
#include <errno.h>
#include "logging.h"
#include "runtime/critical_section.h"

static struct bx_critical_section {
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutex_attr;
} critical_section;

bx_int8 bx_critical_init() {
	int error;

	error = pthread_mutexattr_init(&critical_section.mutex_attr);
	if (error != 0) {
		BX_LOG(LOG_DEBUG, "critical_section", "Error initializing mutex attributes: %i", errno);
		return -1;
	}
	error = pthread_mutexattr_settype(&critical_section.mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	if (error != 0) {
		BX_LOG(LOG_DEBUG, "critical_section", "Error setting mutex attributes: %i", errno);
		pthread_mutexattr_destroy(&critical_section.mutex_attr);
		return -1;
	}
	error = pthread_mutex_init(&critical_section.mutex, &critical_section.mutex_attr);
	if (error != 0) {
		BX_LOG(LOG_DEBUG, "critical_section", "Error initializing mutex: %i", errno);
		pthread_mutexattr_destroy(&critical_section.mutex_attr);
		return -1;
	}

	return 0;
}

bx_int8 bx_critical_enter() {
	int error;

	error = pthread_mutex_lock(&critical_section.mutex);
	if (error != 0) {
		BX_LOG(LOG_DEBUG, "critical_section", "Error acquiring mutex: %i", errno);
		return -1;
	}

	return 0;
}

bx_int8 bx_critical_exit() {
	int error;

	error = pthread_mutex_unlock(&critical_section.mutex);
	if (error != 0) {
		BX_LOG(LOG_DEBUG, "critical_section", "Error releasing mutex: %i", errno);
		return -1;
	}

	return 0;
}

bx_int8 bx_critical_destroy() {
	int error;

	error = pthread_mutex_destroy(&critical_section.mutex);
	error += pthread_mutexattr_destroy(&critical_section.mutex_attr);

	return error == 0 ? 0 : -1;
}

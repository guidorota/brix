/*
 * task_scheduler.h
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

#ifndef TASK_SCHEDULER_H_
#define TASK_SCHEDULER_H_

#include "types.h"
#include "runtime/pcode_manager.h"

typedef void (*native_function)();

typedef bx_ssize bx_task_id;

/**
 * Initializes the task scheduler
 *
 * @return 0 on successful initialization, -1 on error
 */
bx_int8 bx_ts_init();

/**
 * Adds a task based on a native C function.
 *
 * @param function Handler function.
 *
 * @return Task id, -1 on error
 */
bx_task_id bx_ts_add_native_task(native_function function);

/**
 * Adds a task based on a pcode routine.
 * The task_manager invokes the pcode_manager to store the pcode instructions
 * inside a bx_pcode struct.
 *
 * @param buffer Pcode instruction buffer
 * @param buffer_size Pcode instruction buffer size
 *
 * @return Task id, -1 on error
 */
bx_task_id bx_ts_add_pcode_task(void *buffer, bx_size buffer_size);

/**
 * Schedules a task for execution
 *
 * @param task_id Id of the task to schedule
 *
 * @return 0 on successful schedulation, -1 on error
 */
bx_int8 bx_ts_schedule_task(bx_task_id task_id);

/**
 * Checks if the task is scheduled.
 *
 * @task_id Task id
 *
 * @return 1 if the task is scheduled, 0 if the task isn't scheduled, -1 on error
 */
bx_int8 bx_ts_is_scheduled(bx_task_id task_id);

/**
 * Removes the task.
 *
 * @param task_id Id of the task to remove
 *
 * @return 0 on successful removal, -1 on error
 */
bx_int8 bx_ts_remove_task(bx_task_id task_id);

#endif /* TASK_SCHEDULER_H_ */

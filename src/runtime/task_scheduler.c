/*
 * event_handler.c
 * Created on: Mar 27, 2014
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
#include "runtime/task_scheduler.h"
#include "runtime/pcode_manager.h"

enum bx_task_type {
	BX_HANDLER_NATIVE,	///< Native C function
	BX_HANDLER_PCODE	///< Virtual machine function
};

struct bx_task {
	bx_uint16 id;
	enum bx_task_type task_type;
	union bx_task_data {
		native_function native_function;
		struct bx_pcode *pcode;
	} task;
	bx_boolean scheduled;
	struct bx_task *next;
};

static struct bx_ualloc *task_ualloc = NULL;
static bx_uint8 task_storage[EV_HANDLER_STORAGE_SIZE];

static bx_uint16 current_id;
static struct bx_task *scheduled_list;
static struct bx_task *stopped_list;

static void task_list_add(struct bx_task **task_list, struct bx_task *task) {
	struct bx_task *current_element;

	task->next = NULL;

	if (*task_list == NULL) {
		*task_list = task;
		return;
	}

	current_element = *task_list;
	while (current_element->next != NULL) {
		current_element = current_element->next;
	}

	current_element->next = task;
}

static struct bx_task *task_list_search(struct bx_task *task_list, bx_task_id task_id) {
	struct bx_task *current_task;

	current_task = task_list;
	while (current_task != NULL) {
		if (current_task->id == task_id) {
			break;
		}
		current_task = current_task->next;
	}

	return current_task;
}

static struct bx_task *task_list_remove(struct bx_task **task_list, bx_task_id task_id) {
	struct bx_task *current_task;
	struct bx_task *previous_task;

	if (*task_list == NULL) {
		return NULL;
	}

	previous_task = NULL;
	current_task = *task_list;
	while (current_task != NULL) {
		if (current_task->id != task_id) {
			previous_task = current_task;
			current_task = current_task->next;
			continue;
		}
		if (previous_task == NULL) {
			*task_list = current_task->next;
		} else {
			previous_task->next = current_task->next;
		}
		return current_task;
	}

	return NULL;
}

bx_int8 bx_ts_init() {

	task_ualloc = bx_ualloc_init(task_storage,
			EV_HANDLER_STORAGE_SIZE, sizeof (struct bx_task));
	if (task_ualloc == NULL) {
		return -1;
	}

	current_id = 0;
	scheduled_list = NULL;
	stopped_list = NULL;

	return 0;
}

bx_task_id bx_ts_add_native_task(native_function function) {
	struct bx_task *native_task;

	if (function == NULL) {
		return -1;
	}

	native_task = bx_ualloc_alloc(task_ualloc);
	native_task->id = current_id++;
	native_task->task_type = BX_HANDLER_NATIVE;
	native_task->task.native_function = function;
	native_task->scheduled = BX_BOOLEAN_FALSE;

	task_list_add(&stopped_list, native_task);

	return native_task->id;
}

bx_task_id bx_ts_add_pcode_task(void *buffer, bx_size buffer_size) {
	struct bx_pcode *pcode;
	struct bx_task *pcode_task;

	if (buffer == NULL) {
		return -1;
	}

	pcode = bx_pm_add(buffer, buffer_size);
	if (pcode == NULL) {
		return -1;
	}

	pcode_task = bx_ualloc_alloc(task_ualloc);
	pcode_task->id = current_id++;
	pcode_task->task_type = BX_HANDLER_PCODE;
	pcode_task->task.pcode = pcode;
	pcode_task->scheduled = BX_BOOLEAN_FALSE;

	task_list_add(&stopped_list, pcode_task);

	return pcode_task->id;
}

bx_int8 bx_ts_schedule_task(bx_task_id task_id) {
	struct bx_task *task;

	task = task_list_search(stopped_list, task_id);
	if (task == NULL) {
		BX_LOG(LOG_ERROR, "task_scheduler",
				"Cannot schedule: Task %zu not found or already scheduled", task_id);
		return -1;
	}

	task_list_remove(&stopped_list, task->id);
	task_list_add(&scheduled_list, task);

	return 0;
}

bx_int8 bx_ts_is_scheduled(bx_task_id task_id) {
	struct bx_task *task;

	task = task_list_search(scheduled_list, task_id);
	if (task != NULL) {
		return 1;
	}

	task = task_list_search(stopped_list, task_id);
	if (task != NULL) {
		return 0;

	} else {
		BX_LOG(LOG_ERROR, "task_scheduler",
				"Cannot find: Task %zu not found", task_id);
		return -1;
	}
}

bx_int8 bx_ts_remove_task(bx_task_id task_id) {
	struct bx_task *task;

	task = task_list_remove(&stopped_list, task_id);
	if (task != NULL) {
		bx_ualloc_free(task_ualloc, task);
		return 0;
	}

	task = task_list_remove(&scheduled_list, task_id);
	if (task != NULL) {
		bx_ualloc_free(task_ualloc, task);
		return 0;
	}

	BX_LOG(LOG_ERROR, "task_scheduler",
			"Cannot remove: Task %zu not found", task_id);
	return -1;
}

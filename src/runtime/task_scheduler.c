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
#include "runtime/critical_section.h"

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
static struct bx_task *running;
static struct bx_task *scheduled_list;
static struct bx_task *stopped_list;

/**
 * Adds a new task to the list
 *
 * @param task_list Target list
 * @param task Task to add
 */
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

/**
 * Searches a task inside the task list
 *
 * @param task_list Target list
 * @param task_id Id of the task to find
 *
 * @return Task instance, NULL if not found
 */
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

/**
 * Removes a task from the task list
 *
 * @param task_list Target list
 * @param task_id Id of the task to remove
 *
 * @return Pointer to the removed task, NULL the task is not found
 */
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

/**
 * Removes the first element of the list and returns it.
 *
 * @param task_list Target list
 *
 * @return Head of the list, NULL if the list is empty
 */
static struct bx_task *task_list_extract_head(struct bx_task **task_list) {
	struct bx_task *head;

	head = *task_list;
	if (head != NULL) {
		*task_list = (*task_list)->next;
	}

	return head;
}

bx_int8 bx_sched_init() {

	task_ualloc = bx_ualloc_init(task_storage,
			EV_HANDLER_STORAGE_SIZE, sizeof (struct bx_task));
	if (task_ualloc == NULL) {
		return -1;
	}

	current_id = 0;
	scheduled_list = NULL;
	stopped_list = NULL;
	running = NULL;

	return 0;
}

/**
 * Frees the memory occupied by a task
 *
 * @param task Task to remove
 */
void free_task(struct bx_task *task) {

	if (task->task_type == BX_HANDLER_PCODE) {
		bx_pcode_remove(task->task.pcode);
	}

	bx_ualloc_free(task_ualloc, task);
}

void bx_sched_scheduler_loop(bx_boolean stop_if_empty) {

	while (1) {
		bx_critical_enter();
		running = task_list_extract_head(&scheduled_list);
		bx_critical_exit();

		if (running == NULL && stop_if_empty == BX_BOOLEAN_TRUE) {
			break;
		} else if (running == NULL && stop_if_empty == BX_BOOLEAN_FALSE) {
			continue; //TODO: Is this busy waiting the best way to do it? I don't think so...
		}

		switch (running->task_type) {
		case BX_HANDLER_NATIVE:
			running->task.native_function();
			break;
		case BX_HANDLER_PCODE:
			bx_pcode_execute(running->task.pcode);
		}

		bx_critical_enter();
		task_list_add(&stopped_list, running);
		running = NULL;
		bx_critical_exit();
	}
}

bx_task_id bx_sched_add_native_task(native_function function) {
	struct bx_task *native_task;

	if (function == NULL) {
		return -1;
	}

	native_task = bx_ualloc_alloc(task_ualloc);
	native_task->id = current_id++;
	native_task->task_type = BX_HANDLER_NATIVE;
	native_task->task.native_function = function;
	native_task->scheduled = BX_BOOLEAN_FALSE;

	bx_critical_enter();
	task_list_add(&stopped_list, native_task);
	bx_critical_exit();

	return native_task->id;
}

bx_task_id bx_sched_add_pcode_task(void *buffer, bx_size buffer_size) {
	struct bx_pcode *pcode;
	struct bx_task *pcode_task;

	if (buffer == NULL) {
		return -1;
	}

	pcode = bx_pcode_add(buffer, buffer_size);
	if (pcode == NULL) {
		return -1;
	}

	pcode_task = bx_ualloc_alloc(task_ualloc);
	pcode_task->id = current_id++;
	pcode_task->task_type = BX_HANDLER_PCODE;
	pcode_task->task.pcode = pcode;
	pcode_task->scheduled = BX_BOOLEAN_FALSE;

	bx_critical_enter();
	task_list_add(&stopped_list, pcode_task);
	bx_critical_exit();

	return pcode_task->id;
}

bx_int8 bx_sched_schedule_task(bx_task_id task_id) {
	struct bx_task *task;

	bx_critical_enter();

	task = task_list_search(stopped_list, task_id);
	if (task == NULL) {
		BX_LOG(LOG_ERROR, "task_scheduler",
				"Cannot schedule: Task %zu not found or already scheduled", task_id);
		return -1;
	}

	task_list_remove(&stopped_list, task->id);
	task_list_add(&scheduled_list, task);

	bx_critical_exit();

	return 0;
}

bx_int8 bx_sched_is_scheduled(bx_task_id task_id) {
	struct bx_task *task_scheduled;
	struct bx_task *task_stopped;

	bx_critical_enter();
	task_scheduled = task_list_search(scheduled_list, task_id);
	task_stopped = task_list_search(stopped_list, task_id);
	bx_critical_exit();

	if (task_scheduled != NULL) {
		return 1;
	} else if (task_stopped != NULL) {
		return 0;
	} else {
		BX_LOG(LOG_ERROR, "task_scheduler",
				"Cannot find: Task %zu not found", task_id);
		return -1;
	}
}

bx_int8 bx_sched_remove_task(bx_task_id task_id) {
	struct bx_task *task_scheduled;
	struct bx_task *task_stopped;

	bx_critical_enter();
	task_scheduled = task_list_remove(&stopped_list, task_id);
	task_stopped = task_list_remove(&scheduled_list, task_id);
	bx_critical_exit();

	if (task_scheduled != NULL) {
		free_task(task_scheduled);
		return 0;
	}

	if (task_stopped != NULL) {
		free_task(task_stopped);
		return 0;
	}

	BX_LOG(LOG_ERROR, "task_scheduler",
			"Cannot remove: Task %zu not found", task_id);
	return -1;
}

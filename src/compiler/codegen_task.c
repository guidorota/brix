/*
 * codegen_task.c
 * Created on: Mar 17, 2014
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

#include "utils/linked_list.h"
#include "utils/memory_utils.h"
#include "compiler/codegen_task.h"

struct bx_comp_task *bx_cgtk_create_task() {
	struct bx_comp_task *task;

	task = BX_MALLOC(struct bx_comp_task);

	return task;
}

bx_int8 bx_cgtk_add_at_execution_condition(struct bx_comp_task *task, struct bx_comp_code *execution_condition) {

	if (task == NULL || execution_condition == NULL) {
		return -1;
	}

	task->at_execution_condition = bx_cgco_copy(execution_condition);
	if (task->at_execution_condition == NULL) {
		return -1;
	}

	return 0;
}

bx_int8 bx_cgtk_add_on_execution_condition(struct bx_comp_task *task, struct bx_comp_code *execution_condition) {
	//TODO: This may not be the final design of the on execution condition
	if (task == NULL || execution_condition == NULL) {
		return -1;
	}

	task->on_execution_condition = bx_cgco_copy(execution_condition);
	if (task->on_execution_condition == NULL) {
		return -1;
	}

	return 0;
}

bx_int8 bx_cgtk_destroy_task(struct bx_comp_task *task) {
	struct bx_comp_task *child_task;

	if (task == NULL) {
		return -1;
	}

	if (task->at_execution_condition != NULL) {
		bx_cgco_destroy(task->at_execution_condition);
	}

	if (task->on_execution_condition != NULL) {
		bx_cgco_destroy(task->on_execution_condition);
	}

	bx_cgco_destroy(task->code);

	while (task->child_task_list != NULL) {
		child_task = (struct bx_comp_task *) bx_llist_remove_head(&task->child_task_list);
		bx_cgtk_destroy_task(child_task);
	}

	return 0;
}

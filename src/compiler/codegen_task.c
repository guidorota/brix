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

#include <string.h>
#include <stdlib.h>
#include "logging.h"
#include "utils/linked_list.h"
#include "compiler/codegen_expression_cast.h"
#include "compiler/codegen_task.h"

struct bx_comp_task *bx_cgtk_create_task() {
	struct bx_comp_task *task;

	task = malloc(sizeof *task);
	memset((void *) task, 0, sizeof (struct bx_comp_task));

	task->pcode = bx_cgpc_create();
	if (task->pcode == NULL) {
		goto cleanup_task;
	}

	task->symbol_table = bx_cgsy_create_symbol_table();
	if (task->symbol_table == NULL) {
		goto cleanup_pcode;
	}
	return task;

cleanup_pcode:
	free(task->pcode);
cleanup_task:
	free(task);
	return NULL;
}

bx_int8 bx_cgtk_add_at_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition) {
	bx_int8 error;
	struct bx_comp_expr *boolean_condition;

	if (task == NULL || execution_condition == NULL) {
		return -1;
	}

	boolean_condition = bx_cgex_cast_to_bool(execution_condition);
	if (boolean_condition == NULL) {
		return -1;
	}
	error = bx_cgex_convert_to_binary(boolean_condition);
	if (error != 0) {
		return -1;
	}

	//TODO: Optimize: if the condition is always false there's no need to create the task
	task->at_execution_condition = bx_cgpc_copy(boolean_condition->value.pcode);
	if (task->at_execution_condition == NULL) {
		return -1;
	}

	bx_cgex_destroy_expression(boolean_condition);

	return 0;
}


bx_int8 bx_cgtk_add_on_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition) {
	return -1; //TODO: Stub
}

bx_int8 bx_cgtk_add_every_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition) {
	return -1; //TODO: Stub
}

struct bx_comp_task *bx_cgtk_create_child_task(struct bx_comp_task *task) {
	struct bx_comp_task *child_task;
	struct bx_linked_list *new_node;

	if (task == NULL) {
		return NULL;
	}

	child_task = bx_cgtk_create_task();
	if (child_task == NULL) {
		return NULL;
	}

	child_task->parent = task;
	new_node = bx_llist_add(&task->child_task_list, (void *) child_task);
	if (new_node == NULL) {
		bx_cgtk_destroy_task(child_task);
		return NULL;
	}

	return child_task;
}

bx_int8 bx_cgtk_destroy_task(struct bx_comp_task *task) {
	struct bx_comp_task *child_task;

	if (task == NULL) {
		return -1;
	}

	if (task->at_execution_condition != NULL) {
		bx_cgpc_destroy(task->at_execution_condition);
	}

	if (task->on_execution_condition != NULL) {
		bx_cgpc_destroy(task->on_execution_condition);
	}

	if (task->pcode != NULL) {
		bx_cgpc_destroy(task->pcode);
	}

	while (task->child_task_list != NULL) {
		child_task = (struct bx_comp_task *) bx_llist_remove_head(&task->child_task_list);
		bx_cgtk_destroy_task(child_task);
	}

	return 0;
}

/*
 * codegen_code_block.h
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

#ifndef CODEGEN_TASK_H_
#define CODEGEN_TASK_H_

#include "types.h"
#include "compiler/codegen_pcode.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

struct bx_comp_task {
	struct bx_comp_symbol_table *symbol_table;
	struct bx_comp_pcode *at_execution_condition;
	struct bx_comp_pcode *on_execution_condition;
	struct bx_comp_pcode *every_execution_condition;
	struct bx_comp_pcode *pcode;
	struct bx_linked_list *child_task_list;
	struct bx_comp_task *parent;
};

/**
 * Creates a new empty task
 *
 * @return Pointer to the new task, NULL on failure
 */
struct bx_comp_task *bx_cgtk_create_task(void);

/**
 * Adds the @ execution condition to the task passed as parameter.
 * The expression passed as parameter is converted as needed to the data type
 * boolean. Conversion errors may arise if the execution condition cannot be
 * converted to the target type.
 *
 * @param task Target task
 * @param execution_condition Execution condition to add to the target task
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgtk_add_at_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition);

//TODO: Interface stub
bx_int8 bx_cgtk_add_on_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition);

/**
 * Adds the every execution condition to the task passed as parameter.
 * The expression passed as parameter is converted as needed to the data type
 * int. Conversion errors may arise if the execution condition cannot be
 * converted to the target type.
 *
 * @param task Target task
 * @param period Period between task invocations
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgtk_add_every_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *period_expression);

/**
 * Creates a new empty child task and returns its pointer.
 * The task passed as parameter is set as the parent of the newly created
 * child task
 *
 * @param task Parent task
 *
 * @result Pointer to the new created child task, NULL on failure
 */
struct bx_comp_task *bx_cgtk_create_child_task(struct bx_comp_task *task);

/**
 * Destroys the task and reclaims the occupied memory.
 * Child tasks are destroyed recursively.
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgtk_destroy_task(struct bx_comp_task *task);

#endif /* CODEGEN_TASK_H_ */

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
#include "compiler/codegen_code.h"
#include "compiler/codegen_expression.h"

struct bx_comp_task {
	struct bx_comp_code *at_execution_condition;
	struct bx_comp_code *on_execution_condition;
	struct bx_comp_code *code;
	struct bx_linked_list *child_task_list;
};

struct bx_comp_task *bx_cgtk_create_task(void);

bx_int8 bx_cgtk_add_at_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition);

bx_int8 bx_cgtk_add_on_execution_condition(struct bx_comp_task *task, struct bx_comp_expr *execution_condition);

bx_int8 bx_cgtk_destroy_task(struct bx_comp_task *task);

#endif /* CODEGEN_TASK_H_ */

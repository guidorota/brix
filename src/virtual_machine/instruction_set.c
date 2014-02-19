/*
 * instruction_set.c
 * Created on: Feb 16, 2014
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
#include "core/stack.h"
#include "virtual_machine/instruction_set.h"

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define MOD 4

bx_instruction instruction_array[256];

static inline bx_int8 bx_integer_arithmetic_function(struct bx_stack *execution_stack, bx_int8 operation) {
	bx_int32 operand1;
	bx_int32 operand2;
	bx_int32 result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(execution_stack, operand1);
	if (error != 0) {
		return -1;
	}
	error = BX_STACK_POP_VARIABLE(execution_stack, operand2);
	if (error != 0) {
		return -1;
	}

	switch (operation) {
		case ADD:
			result = operand1 + operand2;
			break;
		case SUB:
			result = operand1 - operand2;
			break;
		case MUL:
			result = operand1 * operand2;
			break;
		case DIV:
			result = operand1 / operand2;
			break;
		case MOD:
			result = operand1 % operand2;
			break;
		default:
			return -1;
		}
	error = BX_STACK_PUSH_VARIABLE(execution_stack, result);
	if (error != 0) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_iadd_function(struct bx_stack *execution_stack) {
	return bx_integer_arithmetic_function(execution_stack, ADD);
}

static bx_int8 bx_isub_function(struct bx_stack *execution_stack) {
	return bx_integer_arithmetic_function(execution_stack, SUB);
}

static bx_int8 bx_imul_function(struct bx_stack *execution_stack) {
	return bx_integer_arithmetic_function(execution_stack, MUL);
}

static bx_int8 bx_idiv_function(struct bx_stack *execution_stack) {
	return bx_integer_arithmetic_function(execution_stack, DIV);
}

static bx_int8 bx_imod_function(struct bx_stack *execution_stack) {
	return bx_integer_arithmetic_function(execution_stack, MOD);
}

bx_int8 bx_instr_init() {

	BX_LOG(LOG_INFO, "instruction_set", "Initializing instructions...");
	instruction_array[BX_IADD] = &bx_iadd_function;
	instruction_array[BX_ISUB] = &bx_isub_function;
	instruction_array[BX_IMUL] = &bx_imul_function;
	instruction_array[BX_IDIV] = &bx_idiv_function;
	instruction_array[BX_IMOD] = &bx_imod_function;

	return 0;
}

bx_instruction bx_instr_get(instruction_identifier instruction_identifier) {
	return instruction_array[instruction_identifier];
}

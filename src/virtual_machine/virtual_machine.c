/*
 * virtual_machine.c
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

#include <strings.h>
#include "virtual_machine.h"
#include "configuration.h"
#include "utils/stack.h"
#include "logging.h"
#include "document_manager/document_manager.h"

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define MOD 4

#define BYTE_AT_PC(vm_status_pointer) (vm_status_pointer->code + vm_status_pointer->program_counter)

struct bx_vm_status {
	bx_size program_counter;
	struct bx_stack execution_stack;
	bx_uint8 *code;
	bx_size code_size;
	bx_boolean stop;
};

typedef bx_int8 (*bx_instruction)(struct bx_vm_status *);

static struct bx_vm_status vm_status;
static bx_int8 stack_byte_array[VM_STACK_SIZE];

static bx_instruction instruction_array[256];

static inline bx_int8 bx_integer_arithmetic_function(struct bx_stack *execution_stack, bx_int8 operation);
static inline bx_int8 bx_byte_arithmetic_function(struct bx_stack *execution_stack, bx_int8 operation);
static inline bx_int8 bx_float_arithmetic_function(struct bx_stack *execution_stack, bx_int8 operation);
static inline bx_int8 bx_fetch_instruction(struct bx_vm_status *vm_status, bx_uint8 *instruction_id);
static inline bx_int8 bx_fetch(struct bx_vm_status *vm_status, void *data, bx_size data_length);

//////////////////
// Instructions //
//////////////////

static bx_int8 bx_iadd_function(struct bx_vm_status *vm_status) {
	return bx_integer_arithmetic_function(&vm_status->execution_stack, ADD);
}

static bx_int8 bx_isub_function(struct bx_vm_status *vm_status) {
	return bx_integer_arithmetic_function(&vm_status->execution_stack, SUB);
}

static bx_int8 bx_imul_function(struct bx_vm_status *vm_status) {
	return bx_integer_arithmetic_function(&vm_status->execution_stack, MUL);
}

static bx_int8 bx_idiv_function(struct bx_vm_status *vm_status) {
	return bx_integer_arithmetic_function(&vm_status->execution_stack, DIV);
}

static bx_int8 bx_imod_function(struct bx_vm_status *vm_status) {
	return bx_integer_arithmetic_function(&vm_status->execution_stack, MOD);
}

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

static bx_int8 bx_fadd_function(struct bx_vm_status *vm_status) {
	return bx_float_arithmetic_function(&vm_status->execution_stack, ADD);
}

static bx_int8 bx_fsub_function(struct bx_vm_status *vm_status) {
	return bx_float_arithmetic_function(&vm_status->execution_stack, SUB);
}

static bx_int8 bx_fmul_function(struct bx_vm_status *vm_status) {
	return bx_float_arithmetic_function(&vm_status->execution_stack, MUL);
}

static bx_int8 bx_fdiv_function(struct bx_vm_status *vm_status) {
	return bx_float_arithmetic_function(&vm_status->execution_stack, DIV);
}

static inline bx_int8 bx_float_arithmetic_function(struct bx_stack *execution_stack, bx_int8 operation) {
	bx_float32 operand1;
	bx_float32 operand2;
	bx_float32 result;
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
		default:
			return -1;
		}
	error = BX_STACK_PUSH_VARIABLE(execution_stack, result);
	if (error != 0) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_push32_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	bx_uint32 data;

	error = bx_fetch(vm_status, &data, sizeof data);
	if (error == -1) {
		return -1;
	}
	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, data);
	if (error == -1) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_load32_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
	bx_uint32 data;

	error = bx_fetch(vm_status, &identifier, DM_FIELD_IDENTIFIER_LENGTH);
	if (error == -1) {
		return -1;
	}
	error = bx_dm_invoke_get(identifier, &data);
	if (error == -1) {
		return -1;
	}
	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, data);
	if (error == -1) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_store32_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
	bx_uint32 data;

	error = bx_fetch(vm_status, &identifier, DM_FIELD_IDENTIFIER_LENGTH);
	if (error == -1) {
		return -1;
	}
	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, data);
	if (error == -1) {
		return -1;
	}
	error = bx_dm_invoke_set(identifier, &data);
	if (error == -1) {
		return -1;
	}

	return 0;
}

bx_int8 bx_vm_virtual_machine_init() {

	BX_LOG(LOG_INFO, "virtual_machine", "Initializing virtual machine data structures...");
	bx_stack_init(&vm_status.execution_stack, stack_byte_array, VM_STACK_SIZE);

	BX_LOG(LOG_INFO, "virtual_machine", "Initializing instructions...");
	instruction_array[BX_INSTR_IADD] = &bx_iadd_function;
	instruction_array[BX_INSTR_ISUB] = &bx_isub_function;
	instruction_array[BX_INSTR_IMUL] = &bx_imul_function;
	instruction_array[BX_INSTR_IDIV] = &bx_idiv_function;
	instruction_array[BX_INSTR_IMOD] = &bx_imod_function;
	instruction_array[BX_INSTR_FADD] = &bx_fadd_function;
	instruction_array[BX_INSTR_FSUB] = &bx_fsub_function;
	instruction_array[BX_INSTR_FMUL] = &bx_fmul_function;
	instruction_array[BX_INSTR_FDIV] = &bx_fdiv_function;
	instruction_array[BX_INSTR_PUSH32] = &bx_push32_function;
	instruction_array[BX_INSTR_LOAD32] = &bx_load32_function;
	instruction_array[BX_INSTR_STORE32] = &bx_store32_function;

	return 0;
}

bx_int8 bx_vm_execute(bx_uint8 *code, bx_size code_size) {
	bx_int8 error;
	bx_uint8 instruction_id;

	vm_status.code = code;
	vm_status.code_size = code_size;
	vm_status.program_counter = 0;
	bx_stack_reset(&vm_status.execution_stack);
	vm_status.stop = BX_BOOLEAN_FALSE;

	do {
		error = bx_fetch_instruction(&vm_status, &instruction_id);
		if (error != 0) {
			break;
		}
		error = instruction_array[instruction_id](&vm_status);
		if (error != 0) {
			break;
		}

	} while(vm_status.stop == BX_BOOLEAN_FALSE && vm_status.program_counter < vm_status.code_size);

	if (error != 0) {
		BX_LOG(LOG_ERROR, "virtual_machine", "Abnormal virtual machine termination");
		return -1;
	}

	return 0;
}

static inline bx_int8 bx_fetch_instruction(struct bx_vm_status *vm_status, bx_uint8 *instruction_id) {

	if (vm_status->program_counter > vm_status->code_size) {
		BX_LOG(LOG_ERROR, "virtual_machine", "Error while fetching instruction: unexpected end of code");
		return -1;
	}

	*instruction_id = *BYTE_AT_PC(vm_status);
	vm_status->program_counter++;

	return 0;
}

static inline bx_int8 bx_fetch(struct bx_vm_status *vm_status, void *data, bx_size data_length) {

	if (vm_status->program_counter + data_length > vm_status->code_size) {
		BX_LOG(LOG_ERROR, "virtual_machine", "Error while fetching instruction: unexpected end of code");
		return -1;
	}

	memcpy(data, (void *) BYTE_AT_PC(vm_status), data_length);
	vm_status->program_counter += data_length;

	return 0;
}

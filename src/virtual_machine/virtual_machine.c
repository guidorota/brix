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

enum vm_operand {
	BX_VMOP_ADD,
	BX_VMOP_SUB,
	BX_VMOP_MUL,
	BX_VMOP_DIV,
	BX_VMOP_MOD,
	BX_VMOP_NOT,
	BX_VMOP_AND,
	BX_VMOP_OR,
	BX_VMOP_XOR,
	BX_VMOP_EQ,
	BX_VMOP_NE,
	BX_VMOP_LT,
	BX_VMOP_LE,
	BX_VMOP_GT,
	BX_VMOP_GE
};

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

static const bx_int32 int_const_0 = 0;
static const bx_int32 int_const_1 = 1;

static const bx_float32 float_const_0 = 0.0;
static const bx_float32 float_const_1 = 1.0;

static inline bx_int8 bx_integer_functions(struct bx_stack *execution_stack, enum vm_operand operation);
static inline bx_int8 bx_float_functions(struct bx_stack *execution_stack, enum vm_operand operation);
static inline bx_int8 bx_fetch_instruction(struct bx_vm_status *vm_status, bx_uint8 *instruction_id);
static inline bx_int8 bx_fetch(struct bx_vm_status *vm_status, void *data, bx_size data_length);
static inline bx_int8 bx_jump_functions(struct bx_vm_status *vm_status, enum vm_operand comparison);

//////////////////
// Instructions //
//////////////////

static bx_int8 bx_iadd_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_ADD);
}

static bx_int8 bx_isub_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_SUB);
}

static bx_int8 bx_imul_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_MUL);
}

static bx_int8 bx_idiv_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_DIV);
}

static bx_int8 bx_imod_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_MOD);
}

static bx_int8 bx_iand_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_AND);
}

static bx_int8 bx_ior_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_OR);
}

static bx_int8 bx_ixor_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_XOR);
}

static bx_int8 bx_ieq_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_EQ);
}

static bx_int8 bx_ine_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_NE);
}

static bx_int8 bx_igt_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_GT);
}

static bx_int8 bx_ige_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_GE);
}

static bx_int8 bx_ilt_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_LT);
}

static bx_int8 bx_ile_function(struct bx_vm_status *vm_status) {
	return bx_integer_functions(&vm_status->execution_stack, BX_VMOP_LE);
}

static inline bx_int8 bx_integer_functions(struct bx_stack *execution_stack, enum vm_operand operation) {
	bx_int32 operand1;
	bx_int32 operand2;
	bx_int32 result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(execution_stack, operand2);
	if (error != 0) {
		return -1;
	}
	error = BX_STACK_POP_VARIABLE(execution_stack, operand1);
	if (error != 0) {
		return -1;
	}

	switch (operation) {
		case BX_VMOP_ADD:
			result = operand1 + operand2;
			break;
		case BX_VMOP_SUB:
			result = operand1 - operand2;
			break;
		case BX_VMOP_MUL:
			result = operand1 * operand2;
			break;
		case BX_VMOP_DIV:
			result = operand1 / operand2;
			break;
		case BX_VMOP_MOD:
			result = operand1 % operand2;
			break;
		case BX_VMOP_AND:
			result = operand1 & operand2;
			break;
		case BX_VMOP_OR:
			result = operand1 | operand2;
			break;
		case BX_VMOP_XOR:
			result = operand1 ^ operand2;
			break;
		case BX_VMOP_EQ:
			result = operand1 == operand2;
			break;
		case BX_VMOP_NE:
			result = operand1 != operand2;
			break;
		case BX_VMOP_GT:
			result = operand1 > operand2;
			break;
		case BX_VMOP_GE:
			result = operand1 >= operand2;
			break;
		case BX_VMOP_LT:
			result = operand1 < operand2;
			break;
		case BX_VMOP_LE:
			result = operand1 <= operand2;
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

static bx_int8 bx_ineg_function(struct bx_vm_status *vm_status) {
	bx_int32 operand;
	bx_int32 result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, operand);
	if (error != 0) {
		return -1;
	}
	result = - operand;

	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, result);
	if (error != 0) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_inot_function(struct bx_vm_status *vm_status) {
	bx_int32 operand;
	bx_int32 result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, operand);
	if (error != 0) {
		return -1;
	}
	result = ~operand;

	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, result);
	if (error != 0) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_fadd_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_ADD);
}

static bx_int8 bx_fsub_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_SUB);
}

static bx_int8 bx_fmul_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_MUL);
}

static bx_int8 bx_fdiv_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_DIV);
}

static bx_int8 bx_feq_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_EQ);
}

static bx_int8 bx_fne_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_NE);
}

static bx_int8 bx_fgt_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_GT);
}

static bx_int8 bx_fge_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_GE);
}

static bx_int8 bx_flt_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_LT);
}

static bx_int8 bx_fle_function(struct bx_vm_status *vm_status) {
	return bx_float_functions(&vm_status->execution_stack, BX_VMOP_LE);
}

static inline bx_int8 bx_float_functions(struct bx_stack *execution_stack, enum vm_operand operation) {
	bx_float32 operand1;
	bx_float32 operand2;
	bx_float32 float_result;
	bx_int32 int_result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(execution_stack, operand2);
	if (error != 0) {
		return -1;
	}
	error = BX_STACK_POP_VARIABLE(execution_stack, operand1);
	if (error != 0) {
		return -1;
	}

	switch (operation) {
		case BX_VMOP_ADD:
			float_result = operand1 + operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, float_result);
			break;
		case BX_VMOP_SUB:
			float_result = operand1 - operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, float_result);
			break;
		case BX_VMOP_MUL:
			float_result = operand1 * operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, float_result);
			break;
		case BX_VMOP_DIV:
			float_result = operand1 / operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, float_result);
			break;
		case BX_VMOP_EQ:
			int_result = operand1 == operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		case BX_VMOP_NE:
			int_result = operand1 != operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		case BX_VMOP_GT:
			int_result = operand1 > operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		case BX_VMOP_GE:
			int_result = operand1 >= operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		case BX_VMOP_LT:
			int_result = operand1 < operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		case BX_VMOP_LE:
			int_result = operand1 <= operand2;
			error = BX_STACK_PUSH_VARIABLE(execution_stack, int_result);
			break;
		default:
			return -1;
		}
	if (error != 0) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_fneg_function(struct bx_vm_status *vm_status) {
	bx_float32 operand;
	bx_float32 result;
	bx_int8 error;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, operand);
	if (error != 0) {
		return -1;
	}
	result = - operand;

	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, result);
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

static bx_int8 bx_ipush_0_function(struct bx_vm_status *vm_status) {

	return BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, int_const_0);
}

static bx_int8 bx_ipush_1_function(struct bx_vm_status *vm_status) {

	return BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, int_const_1);
}

static bx_int8 bx_fpush_0_function(struct bx_vm_status *vm_status) {

	return BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, float_const_0);
}

static bx_int8 bx_fpush_1_function(struct bx_vm_status *vm_status) {

	return BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, float_const_1);
}

static bx_int8 bx_rload32_function(struct bx_vm_status *vm_status) {
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

static bx_int8 bx_rstore32_function(struct bx_vm_status *vm_status) {
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

static bx_int8 bx_dup32_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	bx_uint32 data;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, data);
	error += BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, data);
	error += BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, data);
	if (error == -1) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_jump_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	bx_uint16 address;

	error = bx_fetch(vm_status, &address, sizeof address);
	if (error == -1) {
		return -1;
	}
	if (address >= vm_status->code_size) {
		return -1;
	}
	vm_status->program_counter = address;

	return 0;
}

static bx_int8 bx_jeqz_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_EQ);
}

static bx_int8 bx_jnez_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_NE);
}

static bx_int8 bx_jgez_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_GE);
}

static bx_int8 bx_jgtz_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_GT);
}

static bx_int8 bx_jlez_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_LE);
}

static bx_int8 bx_jltz_function(struct bx_vm_status *vm_status) {
	return bx_jump_functions(vm_status, BX_VMOP_LT);
}

static inline bx_int8 bx_jump_functions(struct bx_vm_status *vm_status, enum vm_operand operand) {
	bx_int8 error;
	bx_uint16 address;
	bx_int32 data;

	error = bx_fetch(vm_status, &address, sizeof address);
	if (error == -1) {
		return -1;
	}
	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, data);
	if (error == -1) {
		return -1;
	}
	if (address >= vm_status->code_size) {
		return -1;
	}
	switch(operand) {
	case BX_VMOP_EQ:
		if (data == 0) {
			vm_status->program_counter = address;
		}
		break;
	case BX_VMOP_NE:
		if (data != 0) {
			vm_status->program_counter = address;
		}
		break;
	case BX_VMOP_LT:
		if (data < 0) {
			vm_status->program_counter = address;
		}
		break;
	case BX_VMOP_LE:
		if (data <= 0) {
			vm_status->program_counter = address;
		}
		break;
	case BX_VMOP_GT:
		if (data > 0) {
			vm_status->program_counter = address;
		}
		break;
	case BX_VMOP_GE:
		if (data >= 0) {
			vm_status->program_counter = address;
		}
		break;
	default:
		return -1;
	}

	return 0;
}

static bx_int8 bx_nop_function(struct bx_vm_status *vm_status) {
	return 0;
}

static bx_int8 bx_i2f_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	bx_int32 int_value;
	bx_float32 float_value;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, int_value);
	if (error == -1) {
		return -1;
	}
	float_value = int_value;
	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, float_value);
	if (error == -1) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_f2i_function(struct bx_vm_status *vm_status) {
	bx_int8 error;
	bx_int32 int_value;
	bx_float32 float_value;

	error = BX_STACK_POP_VARIABLE(&vm_status->execution_stack, float_value);
	if (error == -1) {
		return -1;
	}
	int_value = float_value;
	error = BX_STACK_PUSH_VARIABLE(&vm_status->execution_stack, int_value);
	if (error == -1) {
		return -1;
	}

	return 0;
}

static bx_int8 bx_halt_function(struct bx_vm_status *vm_status) {

	vm_status->stop = BX_BOOLEAN_TRUE;

	return 0;
}

static const bx_instruction instruction_array[256] = {
	&bx_iadd_function,
	&bx_isub_function,
	&bx_imul_function,
	&bx_idiv_function,
	&bx_imod_function,
	&bx_ineg_function,
	&bx_iand_function,
	&bx_ior_function,
	&bx_ixor_function,
	&bx_inot_function,
	&bx_ieq_function,
	&bx_ine_function,
	&bx_igt_function,
	&bx_ige_function,
	&bx_ilt_function,
	&bx_ile_function,
	&bx_fadd_function,
	&bx_fsub_function,
	&bx_fmul_function,
	&bx_fdiv_function,
	&bx_fneg_function,
	&bx_feq_function,
	&bx_fne_function,
	&bx_fgt_function,
	&bx_fge_function,
	&bx_flt_function,
	&bx_fle_function,
	&bx_push32_function,
	&bx_ipush_0_function,
	&bx_ipush_1_function,
	&bx_fpush_0_function,
	&bx_fpush_1_function,
	&bx_rload32_function,
	&bx_rstore32_function,
	&bx_dup32_function,
	&bx_jump_function,
	&bx_jeqz_function,
	&bx_jnez_function,
	&bx_jgtz_function,
	&bx_jgez_function,
	&bx_jltz_function,
	&bx_jlez_function,
	&bx_nop_function,
	&bx_i2f_function,
	&bx_f2i_function,
	&bx_halt_function
};

bx_int8 bx_vm_virtual_machine_init() {

	BX_LOG(LOG_INFO, "virtual_machine", "Initializing virtual machine data structures...");
	bx_stack_init(&vm_status.execution_stack, stack_byte_array, VM_STACK_SIZE);

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

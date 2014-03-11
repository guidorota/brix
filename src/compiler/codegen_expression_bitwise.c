/*
 * codegen_expression_bitwise.c
 * Created on: Mar 10, 2014
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

#include <stdlib.h>
#include "logging.h"
#include "compiler/codegen_expression_bitwise.h"

static struct bx_comp_expr *bitwise_or_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *bitwise_xor_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *bitwise_and_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/////////////////////////
// BITWISE OR OPERATOR //
/////////////////////////

struct bx_comp_expr *bx_cgex_bitwise_or_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_FLOAT || operand2->data_type == BX_FLOAT ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '|'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return bitwise_or_int(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '|'.");
		return NULL;
	}
}

static struct bx_comp_expr *bitwise_or_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value | operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IOR);

	return result;
}

/////////////////////////
// BITWISE XOR OPERATOR //
/////////////////////////

struct bx_comp_expr *bx_cgex_bitwise_xor_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_FLOAT || operand2->data_type == BX_FLOAT ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '||'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return bitwise_xor_int(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '||'.");
		return NULL;
	}
}

static struct bx_comp_expr *bitwise_xor_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value ^ operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IXOR);

	return result;
}

//////////////////////////
// BITWISE AND OPERATOR //
//////////////////////////

struct bx_comp_expr *bx_cgex_bitwise_and_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_FLOAT || operand2->data_type == BX_FLOAT ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '&'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return bitwise_and_int(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '&'.");
		return NULL;
	}
}

static struct bx_comp_expr *bitwise_and_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value & operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IAND);

	return result;
}
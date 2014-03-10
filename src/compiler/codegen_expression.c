/*
 * codegen_expression.c
 * Created on: Mar 3, 2014
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
#include "logging.h"
#include "memory_utils.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_expression_arithmetics.h"
#include "compiler/codegen_expression_comparison.h"
#include "compiler/codegen_expression_bitwise.h"
#include "compiler/codegen_expression_logical.h"
#include "compiler/codegen_expression_cast.h"
#include "compiler/codegen_symbol_table.h"
#include "compiler/codegen_code.h"
#include "virtual_machine/virtual_machine.h"

static void constant_bool_to_binary(struct bx_comp_code *code, bx_int32 bool_value);
static void constant_int_to_binary(struct bx_comp_code *code, bx_int32 int_value);

struct bx_comp_expr *bx_cgex_create_int_constant(bx_int32 value) {
	struct bx_comp_expr *expression;

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	expression->data_type = BX_INT;
	expression->type = BX_COMP_CONSTANT;
	expression->bx_value.int_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_float_constant(bx_float32 value) {
	struct bx_comp_expr *expression;

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	expression->data_type = BX_FLOAT;
	expression->type = BX_COMP_CONSTANT;
	expression->bx_value.float_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_bool_constant(bx_boolean value) {
	struct bx_comp_expr *expression;

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	expression->data_type = BX_BOOL;
	expression->type = BX_COMP_CONSTANT;
	expression->bx_value.bool_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_variable(char *identifier) {
	struct bx_comp_symbol *symbol;
	struct bx_comp_expr *expression;
	struct bx_comp_code *code;

	if (identifier == NULL) {
		return NULL;
	}

	symbol = bx_cgsy_get(identifier);
	if (symbol == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Variable %s has not been declared", identifier);
		return NULL;
	}

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	code = bx_cgco_create();
	if (code == NULL) {
		return NULL;
	}
	bx_cgco_add_instruction(code, BX_INSTR_LOAD32);
	bx_cgco_add_identifier(code, identifier);

	expression->type = BX_COMP_BINARY;
	expression->data_type = symbol->data_type;
	expression->bx_value.code = code;

	return expression;
}

struct bx_comp_expr *bx_cgex_cast(struct bx_comp_expr *expression, enum bx_builtin_type type) {

	if (expression == NULL) {
		return NULL;
	}

	switch (type) {
	case BX_INT:
		return bx_cgex_cast_to_int(expression);
	case BX_FLOAT:
		return bx_cgex_cast_to_float(expression);
	case BX_BOOL:
		return bx_cgex_cast_to_bool(expression);
	case BX_STRING:
		return bx_cgex_cast_to_string(expression);
	case BX_SUBNET:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'subnet' is not allowed.");
		return NULL;
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'subnet' is not allowed.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_cast", "Unknown type encountered in function bx_cgca_cast.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operation operation) {

	if (operand1 == NULL || operand2 == NULL) {
		return NULL;
	}

	switch(operation) {
	case BX_COMP_OP_ADD:
		return bx_cgex_addition_operator(operand1, operand2);
		break;
	case BX_COMP_OP_SUB:
		return bx_cgex_subtraction_operator(operand1, operand2);
		break;
	case BX_COMP_OP_MUL:
		return bx_cgex_multiplication_operator(operand1, operand2);
		break;
	case BX_COMP_OP_DIV:
		return bx_cgex_division_operator(operand1, operand2);
		break;
	case BX_COMP_OP_MOD:
		return bx_cgex_modulo_operator(operand1, operand2);
		break;
	case BX_COMP_OP_EQ:
		return bx_cgex_equality_operator(operand1, operand2);
		break;
	case BX_COMP_OP_NE:
		return bx_cgex_inequality_operator(operand1, operand2);
		break;
	case BX_COMP_OP_GT:
		return bx_cgex_greater_than_operator(operand1, operand2);
		break;
	case BX_COMP_OP_GE:
		return bx_cgex_greater_or_equal_operator(operand1, operand2);
		break;
	case BX_COMP_OP_LT:
		return bx_cgex_less_than_operator(operand1, operand2);
		break;
	case BX_COMP_OP_LE:
		return bx_cgex_less_or_equal_operator(operand1, operand2);
		break;
	case BX_COMP_OP_BITWISE_OR:
		return bx_cgex_bitwise_or_operator(operand1, operand2);
		break;
	case BX_COMP_OP_BITWISE_XOR:
		return bx_cgex_bitwise_xor_operator(operand1, operand2);
		break;
	case BX_COMP_OP_BITWISE_AND:
		return bx_cgex_bitwise_and_operator(operand1, operand2);
		break;
	case BX_COMP_OP_OR:
		return bx_cgex_logical_or_operator(operand1, operand2);
		break;
	case BX_COMP_OP_AND:
		return bx_cgex_logical_and_operator(operand1, operand2);
		break;
	default:
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_int_to_float(struct bx_comp_expr *value) {

	switch(value->type) {
	case BX_COMP_BINARY:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_I2F);
		value->data_type = BX_FLOAT;
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.float_value = (bx_float32) value->bx_value.int_value;
		value->data_type = BX_FLOAT;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_float_to_int(struct bx_comp_expr *value) {

	switch(value->type) {
	case BX_COMP_BINARY:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_F2I);
		value->data_type = BX_INT;
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.int_value = (bx_int32) value->bx_value.float_value;
		value->data_type = BX_INT;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_constant_to_binary(struct bx_comp_expr *value) {
	struct bx_comp_code *code;

	code = bx_cgco_create();

	switch(value->data_type) {
	case BX_INT:
		constant_int_to_binary(code, value->bx_value.int_value);
		break;

	case BX_FLOAT:
		bx_cgco_add_instruction(code, BX_INSTR_PUSH32);
		bx_cgco_add_float_constant(code, value->bx_value.float_value);
		break;

	case BX_BOOL:
		constant_bool_to_binary(code, value->bx_value.bool_value);
		break;

	default:
		BX_LOG(LOG_WARNING, "codegen_expression",
				"Unexpected data type encountered in constant_to_binary conversion.");
		return NULL;
	}

	value->type = BX_COMP_BINARY;
	value->bx_value.code = code;
	return value;
}

static void constant_bool_to_binary(struct bx_comp_code *code, bx_int32 bool_value) {

	if (bool_value == 0) {
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_0);
	} else {
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
	}
}

static void constant_int_to_binary(struct bx_comp_code *code, bx_int32 int_value) {

	switch (int_value) {
	case 0:
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_0);
		break;
	case 1:
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
		break;
	default:
		bx_cgco_add_instruction(code, BX_INSTR_PUSH32);
		bx_cgco_add_int_constant(code, int_value);
		break;
	}
}

struct bx_comp_expr *bx_cgex_create_code_expression() {
	struct bx_comp_expr *expression;

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}
	expression->bx_value.code = bx_cgco_create();
	if (expression->bx_value.code == NULL) {
		free(expression);
		return NULL;
	}
	expression->type = BX_COMP_BINARY;

	return expression;
}

char *bx_cgex_get_type_name(enum bx_builtin_type type) {

	switch (type) {
	case BX_INT:
		return "int";
	case BX_FLOAT:
		return "float";
	case BX_STRING:
		return "string";
	case BX_BOOL:
		return "boolean";
	case BX_SUBNET:
		return "subnet";
	case BX_STREAM:
		return "stream";
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unrecognized type in function bx_cgex_get_type_name");
		return NULL;
	}
}

void bx_cgex_destroy_expression(struct bx_comp_expr *expression) {

	if (expression == NULL) {
		return;
	}

	if (expression->type == BX_COMP_BINARY) {
		bx_cgco_destroy(expression->bx_value.code);
	}

	free(expression);
}

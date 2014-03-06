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
#include "compiler/codegen_symbol_table.h"
#include "compiler/codegen_code.h"
#include "virtual_machine/virtual_machine.h"

static struct bx_comp_expr *plus_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *add_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *add_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *concat_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *int_to_float(struct bx_comp_expr *value);
static struct bx_comp_expr *float_to_int(struct bx_comp_expr *value);
static struct bx_comp_expr *constant_to_binary(struct bx_comp_expr *value);

struct bx_comp_expr *create_code_expression();

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

struct bx_comp_expr *bx_cgex_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operation operation) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (operand1 == NULL || operand2 == NULL) {
		return NULL;
	}

	switch(operation) {
	case BX_COMP_ADD:
		return plus_operator(operand1, operand2);
		break;
	default:
		return NULL;
	}
}

///////////////////
// PLUS OPERATOR //
///////////////////

static struct bx_comp_expr *plus_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '+'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return add_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return add_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = int_to_float(operand1);
		return add_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = int_to_float(operand2);
		return add_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return concat_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in function plus_operator.");
		return NULL;
	}
}

static struct bx_comp_expr *add_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value + operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = constant_to_binary(operand2);
	}

	result = create_code_expression();
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IADD);

	return result;
}

static struct bx_comp_expr *add_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.float_value + operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = constant_to_binary(operand2);
	}

	result = create_code_expression();
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FADD);

	return result;
}

static struct bx_comp_expr *concat_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	return NULL; //TODO: Stub
}

////////////////
// CONVERSION //
////////////////

static struct bx_comp_expr *int_to_float(struct bx_comp_expr *value) {
	bx_float32 float_value;

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

static struct bx_comp_expr *float_to_int(struct bx_comp_expr *value) {
	bx_float32 float_value;

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

static struct bx_comp_expr *constant_to_binary(struct bx_comp_expr *value) {

	value->bx_value.code = bx_cgco_create();
	value->type = BX_COMP_BINARY;

	switch(value->data_type) {
	case BX_INT:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_PUSH32);
		bx_cgco_add_int_constant(value->bx_value.code, value->bx_value.int_value);
		return value;

	case BX_FLOAT:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_PUSH32);
		bx_cgco_add_float_constant(value->bx_value.code, value->bx_value.float_value);
		return value;

	case BX_BOOL:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_PUSH32);
		bx_cgco_add_bool_constant(value->bx_value.code, (bx_uint32) value->bx_value.bool_value);
		return NULL;

	default:
		BX_LOG(LOG_WARNING, "codegen_expression",
				"Unexpected data type encountered in constant_to_binary conversion.");
		return NULL;
	}
}

///////////
// UTILS //
///////////

struct bx_comp_expr *create_code_expression() {
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

void bx_cgex_destroy_expression(struct bx_comp_expr *expression) {

	if (expression == NULL) {
		return;
	}

	if (expression->type == BX_COMP_BINARY) {
		bx_cgco_destroy(expression->bx_value.code);
	}

	free(expression);
}

/*
 * codegen_expression_cast.c
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
#include "compiler/codegen_expression_cast.h"

struct bx_comp_expr *bx_cgex_cast_to_int(struct bx_comp_expr *expression) {
	enum bx_builtin_type expression_type;

	expression_type = expression->data_type;
	if (expression_type == BX_STREAM || expression_type == BX_SUBNET ||
			expression_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "compiler", "Type '%s' cannot be converted to 'int'.",
						bx_cgex_get_type_name(expression_type));
	}

	switch (expression_type) {
	case BX_INT:
		return expression;
	case BX_FLOAT:
		return bx_cgex_float_to_int(expression);
	case BX_BOOL:
		return bx_cgex_bool_to_int(expression);
	default:
		BX_LOG(LOG_ERROR, "codegen_expression_cast", "Unrecognized type in function bx_cgex_cast_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_cast_to_float(struct bx_comp_expr *expression) {
	enum bx_builtin_type expression_type;

	expression_type = expression->data_type;
	if (expression_type == BX_STREAM || expression_type == BX_SUBNET ||
			expression_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "compiler", "Type '%s' cannot be converted to 'float'.",
				bx_cgex_get_type_name(expression_type));
	}

	switch (expression_type) {
	case BX_INT:
		return bx_cgex_int_to_float(expression);
	case BX_FLOAT:
		return expression;
	case BX_BOOL:
		return bx_cgex_bool_to_float(expression);
	default:
		BX_LOG(LOG_ERROR, "codegen_expression_cast", "Unrecognized type in function bx_cgex_cast_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_cast_to_bool(struct bx_comp_expr *expression) {
	enum bx_builtin_type expression_type;

	expression_type = expression->data_type;
	if (expression_type == BX_STREAM || expression_type == BX_SUBNET ||
			expression_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "compiler", "Type '%s' cannot be converted to 'bool'.",
						bx_cgex_get_type_name(expression_type));
	}

	switch (expression_type) {
	case BX_INT:
		return bx_cgex_int_to_bool(expression);
	case BX_FLOAT:
		return bx_cgex_float_to_bool(expression);
	case BX_BOOL:
		return expression;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression_cast", "Unrecognized type in function bx_cgex_cast_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_cast_to_string(struct bx_comp_expr *expression) {
	enum bx_builtin_type expression_type;

	expression_type = expression->data_type;
	if (expression_type == BX_STREAM || expression_type == BX_SUBNET) {
		BX_LOG(LOG_ERROR, "compiler", "Type '%s' cannot be converted to 'string'.",
						bx_cgex_get_type_name(expression_type));
	}

	switch (expression_type) {
	case BX_INT:
		return NULL; //TODO: Stub
	case BX_FLOAT:
		return NULL; //TODO: Stub
	case BX_BOOL:
		return NULL; //TODO: Stub
	case BX_STRING:
		return expression;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression_cast", "Unrecognized type in function bx_cgex_cast_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_int_to_bool(struct bx_comp_expr *value) {
	bx_comp_label false_label, true_label;
	bx_ssize false_jump_address, true_jump_address;
	struct bx_comp_code *code;

	switch(value->type) {
	case BX_COMP_BINARY:
		value->data_type = BX_BOOL;
		code = value->bx_value.code;
		bx_cgco_add_instruction(code, BX_INSTR_JEQZ);
		false_label = bx_cgco_create_address_label(code);
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
		bx_cgco_add_instruction(code, BX_INSTR_JUMP);
		true_label = bx_cgco_create_address_label(code);
		false_jump_address = bx_cgco_add_instruction(code, BX_INSTR_IPUSH_0);
		true_jump_address = bx_cgco_add_instruction(code, BX_INSTR_NOP);
		bx_cgco_set_address_label(code, false_label, false_jump_address);
		bx_cgco_set_address_label(code, true_label, true_jump_address);
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.bool_value =
				value->bx_value.int_value == 0 ? BX_BOOLEAN_FALSE : BX_BOOLEAN_TRUE;
		value->data_type = BX_BOOL;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_int_to_bool.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_float_to_bool(struct bx_comp_expr *value) {
	bx_comp_label false_label, true_label;
	bx_ssize false_jump_address, true_jump_address;
	struct bx_comp_code *code;

	switch(value->type) {
	case BX_COMP_BINARY:
		value->data_type = BX_BOOL;
		code = value->bx_value.code;
		bx_cgco_add_instruction(code, BX_INSTR_FPUSH_0);
		bx_cgco_add_instruction(code, BX_INSTR_FSUB);
		bx_cgco_add_instruction(code, BX_INSTR_JEQZ);
		false_label = bx_cgco_create_address_label(code);
		bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
		bx_cgco_add_instruction(code, BX_INSTR_JUMP);
		true_label = bx_cgco_create_address_label(code);
		false_jump_address = bx_cgco_add_instruction(code, BX_INSTR_IPUSH_0);
		true_jump_address = bx_cgco_add_instruction(code, BX_INSTR_NOP);
		bx_cgco_set_address_label(code, false_label, false_jump_address);
		bx_cgco_set_address_label(code, true_label, true_jump_address);
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.bool_value =
				value->bx_value.float_value == 0.0 ? BX_BOOLEAN_FALSE : BX_BOOLEAN_TRUE;
		value->data_type = BX_BOOL;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_float_to_bool.");
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
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_int_to_float.");
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
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_float_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_bool_to_int(struct bx_comp_expr *value) {

	switch(value->type) {
	case BX_COMP_BINARY:
		value->data_type = BX_INT;
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.bool_value = (bx_int32) value->bx_value.int_value;
		value->data_type = BX_INT;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_bool_to_int.");
		return NULL;
	}
}

struct bx_comp_expr *bx_cgex_bool_to_float(struct bx_comp_expr *value) {

	switch(value->type) {
	case BX_COMP_BINARY:
		bx_cgco_add_instruction(value->bx_value.code, BX_INSTR_I2F);
		value->data_type = BX_FLOAT;
		return value;

	case BX_COMP_CONSTANT:
		value->bx_value.float_value = (bx_int32) value->bx_value.bool_value;
		value->data_type = BX_FLOAT;
		return value;

	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type in function bx_cgex_bool_to_int.");
		return NULL;
	}
}
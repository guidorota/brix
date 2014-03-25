/*
 * codegen_expression_assignment.c
 * Created on: Mar 13, 2014
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
#include "compiler/codegen_code.h"
#include "compiler/codegen_expression_cast.h"
#include "compiler/codegen_expression_assignment.h"

static struct bx_comp_expr *assign_to_int(struct bx_comp_expr *destination, struct bx_comp_expr *expression);
static struct bx_comp_expr *assign_to_float(struct bx_comp_expr *destination, struct bx_comp_expr *expression);
static struct bx_comp_expr *assign_to_boolean(struct bx_comp_expr *destination, struct bx_comp_expr *expression);
static bx_int8 add_32bit_assign_code(struct bx_comp_expr *result, struct bx_comp_expr *destination);

struct bx_comp_expr *bx_cgex_assignment_expression(struct bx_comp_expr *destination, struct bx_comp_expr *expression) {

	if (destination->expression_type != BX_COMP_VARIABLE) {
		BX_LOG(LOG_ERROR, "variable_assignment",
				"Non-assignable destination passed to function 'variable_assignment'");
		return NULL;
	}

	switch (destination->data_type) {
	case BX_INT:
		return assign_to_int(destination, expression);
	case BX_FLOAT:
		return assign_to_float(destination, expression);
	case BX_BOOL:
		return assign_to_boolean(destination, expression);
	case BX_STRING:
		return NULL; //TODO: Stub
	case BX_SUBNET:
		return NULL; //TODO: Stub
	case BX_STREAM:
		return NULL; //TODO: Stub
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected data type "
				"encountered in function 'variable_assignment'");
		return NULL;
	}
}

static struct bx_comp_expr *assign_to_int(struct bx_comp_expr *destination, struct bx_comp_expr *expression) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (expression->data_type == BX_BOOL || expression->data_type == BX_STRING ||
			expression->data_type == BX_SUBNET || expression->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "compiler", "Unable to assign '%s' expression to '%s' variable.",
				bx_cgex_get_type_name(expression->data_type), bx_cgex_get_type_name(destination->data_type));
		return NULL;
	}

	result = bx_cgex_cast_to_int(expression);
	if (result == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Error while casting expression to "
				"destination data type in function 'assign_to_int'");
		return NULL;
	}
	error = bx_cgex_convert_to_binary(result);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Error while converting expression to "
				"binary in function 'assign_to_int'");
		return NULL;
	}

	error = add_32bit_assign_code(result, destination);
	if (error != 0) {
		return NULL;
	}

	return result;
}

static struct bx_comp_expr *assign_to_float(struct bx_comp_expr *destination, struct bx_comp_expr *expression) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (expression->data_type == BX_BOOL || expression->data_type == BX_STRING ||
			expression->data_type == BX_SUBNET || expression->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "compiler", "Unable to assign '%s' expression to '%s' variable.",
				bx_cgex_get_type_name(expression->data_type), bx_cgex_get_type_name(destination->data_type));
		return NULL;
	}

	result = bx_cgex_cast_to_float(expression);
	if (result == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Error while casting expression to "
				"destination data type in function 'assign_to_float'");
		return NULL;
	}
	error = bx_cgex_convert_to_binary(result);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Error while converting expression to "
				"binary in function 'assign_to_float'");
		return NULL;
	}

	error = add_32bit_assign_code(result, destination);
	if (error != 0) {
		return NULL;
	}

	return result;
}

static struct bx_comp_expr *assign_to_boolean(struct bx_comp_expr *destination, struct bx_comp_expr *expression) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (expression->data_type != BX_BOOL) {
		BX_LOG(LOG_ERROR, "compiler", "Unable to assign '%s' expression to '%s' variable.",
				bx_cgex_get_type_name(expression->data_type), bx_cgex_get_type_name(destination->data_type));
		return NULL;
	}

	result = bx_cgex_copy_expression(expression);
	error = bx_cgex_convert_to_binary(result);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Error while converting expression to "
				"binary in function 'assign_to_boolean'");
		return NULL;
	}

	error = add_32bit_assign_code(result, destination);
	if (error != 0) {
		return NULL;
	}

	return result;
}

static bx_int8 add_32bit_assign_code(struct bx_comp_expr *result, struct bx_comp_expr *destination) {

	bx_cgco_replace_code(result->side_effect_code, result->value.code);

	switch (destination->value.variable->symbol_type) {
	case BX_COMP_FIELD_SYMBOL:
		bx_cgco_add_instruction(result->value.code, BX_INSTR_DUP32);
		bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
		bx_cgco_add_identifier(result->value.code, destination->value.variable->identifier);

		bx_cgco_add_instruction(result->side_effect_code, BX_INSTR_RSTORE32);
		bx_cgco_add_identifier(result->side_effect_code, destination->value.variable->identifier);
		break;
	case BX_COMP_VARIABLE_SYMBOL:
		bx_cgco_add_instruction(result->value.code, BX_INSTR_DUP32);
		bx_cgco_add_instruction(result->value.code, BX_INSTR_VSTORE32);
		bx_cgco_add_address(result->value.code, destination->value.variable->symbol_data.variable_number);

		bx_cgco_add_instruction(result->side_effect_code, BX_INSTR_VSTORE32);
		bx_cgco_add_address(result->side_effect_code, destination->value.variable->symbol_data.variable_number);
		break;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Unexpected symbol type in function 'assign_32bit_value'");
		return -1;
	}

	return 0;
}

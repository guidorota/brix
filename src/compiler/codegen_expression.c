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
#include <stdlib.h>
#include "logging.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_expression_assignment.h"
#include "compiler/codegen_expression_arithmetics.h"
#include "compiler/codegen_expression_comparison.h"
#include "compiler/codegen_expression_bitwise.h"
#include "compiler/codegen_expression_logical.h"
#include "compiler/codegen_expression_cast.h"
#include "compiler/codegen_symbol_table.h"
#include "compiler/codegen_pcode.h"
#include "virtual_machine/virtual_machine.h"

static void combine_side_effect_pcode(struct bx_comp_expr *source1,
		struct bx_comp_expr *source2, struct bx_comp_expr *destination);
static void copy_side_effect_pcode(struct bx_comp_expr *source, struct bx_comp_expr *destination);

static void constant_bool_to_binary(struct bx_comp_pcode *pcode, bx_int32 bool_value);
static void constant_int_to_binary(struct bx_comp_pcode *pcode, bx_int32 int_value);
static void constant_float_to_binary(struct bx_comp_pcode *pcode, bx_float32 float_value);
static bx_int8 constant_to_binary(struct bx_comp_expr *expression);
static bx_int8 variable_to_binary(struct bx_comp_expr *expression);

static struct bx_comp_expr *create_empty_expression() {
	struct bx_comp_expr *expression;

	expression = malloc(sizeof *expression);
	if (expression == NULL) {
		return NULL;
	}

	memset((void *) expression, 0, sizeof (struct bx_comp_expr));
	expression->side_effect_pcode = bx_cgpc_create();
	if (expression->side_effect_pcode == NULL) {
		free(expression);
		return NULL;
	}

	return expression;
}

struct bx_comp_expr *bx_cgex_create_int_constant(bx_int32 value) {
	struct bx_comp_expr *expression;

	expression = create_empty_expression();
	if (expression == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error instantiating memory in function 'bx_cgex_create_int_constant'");
		return NULL;
	}

	expression->data_type = BX_INT;
	expression->expression_type = BX_COMP_CONSTANT;
	expression->value.int_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_float_constant(bx_float32 value) {
	struct bx_comp_expr *expression;

	expression = create_empty_expression();;
	if (expression == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error instantiating memory in function 'bx_cgex_create_float_constant'");
		return NULL;
	}

	expression->data_type = BX_FLOAT;
	expression->expression_type = BX_COMP_CONSTANT;
	expression->value.float_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_bool_constant(bx_boolean value) {
	struct bx_comp_expr *expression;

	expression = create_empty_expression();;
	if (expression == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error instantiating memory in function 'bx_cgex_create_bool_constant'");
		return NULL;
	}

	expression->data_type = BX_BOOL;
	expression->expression_type = BX_COMP_CONSTANT;
	expression->value.bool_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_variable(struct bx_comp_symbol_table *symbol_table, char *identifier) {
	struct bx_comp_symbol *symbol;
	struct bx_comp_expr *expression;

	if (identifier == NULL) {
		return NULL;
	}

	symbol = bx_cgsy_get_symbol(symbol_table, identifier);
	if (symbol == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Variable %s has not been declared", identifier);
		return NULL;
	}

	expression = create_empty_expression();;
	if (expression == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error instantiating memory in function 'bx_cgex_create_variable'");
		return NULL;
	}

	expression->expression_type = BX_COMP_VARIABLE;
	expression->data_type = symbol->data_type;
	expression->value.variable = symbol;

	return expression;
}

static void combine_side_effect_pcode(struct bx_comp_expr *source1,
		struct bx_comp_expr *source2, struct bx_comp_expr *destination) {

	if (source1->side_effect_pcode != NULL &&
			source2->side_effect_pcode != NULL) {
		return;
	}

	if (source1->side_effect_pcode != NULL) {
		bx_cgpc_replace_pcode(destination->side_effect_pcode, source1->side_effect_pcode);
	}
	if (source1->side_effect_pcode != NULL) {
		bx_cgpc_append_pcode(destination->side_effect_pcode, source1->side_effect_pcode);
	}
}

static void copy_side_effect_pcode(struct bx_comp_expr *source, struct bx_comp_expr *destination) {

	bx_cgpc_replace_pcode(destination->side_effect_pcode, source->side_effect_pcode);
}

struct bx_comp_expr *bx_cgex_cast(struct bx_comp_expr *expression, enum bx_builtin_type type) {
	struct bx_comp_expr *result;

	if (expression == NULL) {
		return NULL;
	}

	switch (type) {
	case BX_INT:
		result = bx_cgex_cast_to_int(expression);
		break;
	case BX_FLOAT:
		result = bx_cgex_cast_to_float(expression);
		break;
	case BX_BOOL:
		result = bx_cgex_cast_to_bool(expression);
		break;
	case BX_STRING:
		result = bx_cgex_cast_to_string(expression);
		break;
	case BX_SUBNET:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'subnet' is not allowed.");
		return NULL;
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'stream' is not allowed.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_cast", "Unknown type encountered in function bx_cgca_cast.");
		return NULL;
	}

	copy_side_effect_pcode(expression, result);

	return result;
}

struct bx_comp_expr *bx_cgex_binary_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operator operator) {
	struct bx_comp_expr *result;

	if (operand1 == NULL || operand2 == NULL) {
		return NULL;
	}

	switch(operator) {
	case BX_COMP_OP_ADD:
		result = bx_cgex_addition_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_SUB:
		result = bx_cgex_subtraction_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_MUL:
		result = bx_cgex_multiplication_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_DIV:
		result = bx_cgex_division_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_MOD:
		result = bx_cgex_modulo_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_EQ:
		result = bx_cgex_equality_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_NE:
		result = bx_cgex_inequality_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_GT:
		result = bx_cgex_greater_than_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_GE:
		result = bx_cgex_greater_or_equal_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_LT:
		result = bx_cgex_less_than_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_LE:
		result = bx_cgex_less_or_equal_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_BITWISE_OR:
		result = bx_cgex_bitwise_or_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_BITWISE_XOR:
		result = bx_cgex_bitwise_xor_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_BITWISE_AND:
		result = bx_cgex_bitwise_and_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_OR:
		result = bx_cgex_logical_or_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_AND:
		result = bx_cgex_logical_and_operator(operand1, operand2);
		combine_side_effect_pcode(operand1, operand2, result);
		break;
	case BX_COMP_OP_ASSIGNMENT:
		result = bx_cgex_assignment_expression(operand1, operand2);
		break;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operator "
				"encountered in function 'bx_cgex_binary_expression'");
		return NULL;
	}

	return result;
}

struct bx_comp_expr *bx_cgex_unary_expression(struct bx_comp_expr *operand1, enum bx_comp_operator operator) {
	struct bx_comp_expr *result;

	if (operand1 == NULL) {
		return NULL;
	}

	switch (operator) {
	case BX_COMP_OP_UNARY_PLUS:
		result = bx_cgex_unary_plus_operator(operand1);
		copy_side_effect_pcode(operand1, result);
		break;
	case BX_COMP_OP_UNARY_MINUS:
		result = bx_cgex_unary_minus_operator(operand1);
		copy_side_effect_pcode(operand1, result);
		break;
	case BX_COMP_OP_NOT:
		result = bx_cgex_logical_not_operator(operand1);
		copy_side_effect_pcode(operand1, result);
		break;
	case BX_COMP_OP_BITWISE_COMPLEMENT:
		result = bx_cgex_bitwise_complement_operator(operand1);
		copy_side_effect_pcode(operand1, result);
		break;
	case BX_COMP_OP_PREFIX_INC:
		result = bx_cgex_prefix_inc_operator(operand1);
		break;
	case BX_COMP_OP_PREFIX_DEC:
		result = bx_cgex_prefix_dec_operator(operand1);
		break;
	case BX_COMP_OP_POSTFIX_INC:
		result = bx_cgex_postfix_inc_operator(operand1);
		break;
	case BX_COMP_OP_POSTFIX_DEC:
		result = bx_cgex_postfix_dec_operator(operand1);
		break;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operator "
						"encountered in function 'bx_cgex_unary_expression'");
		return NULL;
	}

	return result;
}

bx_int8 bx_cgex_convert_to_binary(struct bx_comp_expr *expression) {

	switch (expression->expression_type) {
	case BX_COMP_BINARY:
		return 0;
	case BX_COMP_CONSTANT:
		return constant_to_binary(expression);
	case BX_COMP_VARIABLE:
		return variable_to_binary(expression);
	default:
			BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected expression type "
					"encountered in function 'bx_cgex_convert_to_binary'");
		return -1;
	}
}

struct bx_comp_pcode *bx_cgex_side_effect_pcode(struct bx_comp_expr *expression) {

	if (expression == NULL) {
		return NULL;
	}

	if (expression->side_effect_pcode == NULL) {
		return bx_cgpc_create();
	}

	return expression->side_effect_pcode;
}

static bx_int8 constant_to_binary(struct bx_comp_expr *expression) {
	struct bx_comp_pcode *pcode;

	pcode = bx_cgpc_create();

	switch(expression->data_type) {
	case BX_INT:
		constant_int_to_binary(pcode, expression->value.int_value);
		break;

	case BX_FLOAT:
		constant_float_to_binary(pcode, expression->value.float_value);
		break;

	case BX_BOOL:
		constant_bool_to_binary(pcode, expression->value.bool_value);
		break;

	default:
		BX_LOG(LOG_WARNING, "codegen_expression",
				"Unexpected data type encountered in constant_to_binary conversion.");
		return -1;
	}

	expression->expression_type = BX_COMP_BINARY;
	expression->value.pcode = pcode;
	return 0;
}

static void constant_bool_to_binary(struct bx_comp_pcode *pcode, bx_int32 bool_value) {

	if (bool_value == 0) {
		bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_0);
	} else {
		bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_1);
	}
}

static void constant_int_to_binary(struct bx_comp_pcode *pcode, bx_int32 int_value) {

	switch (int_value) {
	case 0:
		bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_0);
		break;
	case 1:
		bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_1);
		break;
	default:
		bx_cgpc_add_instruction(pcode, BX_INSTR_PUSH32);
		bx_cgpc_add_int_constant(pcode, int_value);
		break;
	}
}

static void constant_float_to_binary(struct bx_comp_pcode *pcode, bx_float32 float_value) {

	if (float_value == 0.0) {
		bx_cgpc_add_instruction(pcode, BX_INSTR_FPUSH_0);

	} else if (float_value == 1.0) {
		bx_cgpc_add_instruction(pcode, BX_INSTR_FPUSH_1);

	} else {
		bx_cgpc_add_instruction(pcode, BX_INSTR_PUSH32);
		bx_cgpc_add_float_constant(pcode, float_value);
	}
}

static bx_int8 variable_to_binary(struct bx_comp_expr *expression) {
	struct bx_comp_pcode *pcode;

	pcode = bx_cgpc_create();
	if (pcode == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error creating bx_comp_code struct in 'variable_to_binary'");
		return -1;
	}

	switch (expression->value.variable->symbol_type) {
	case BX_COMP_FIELD_SYMBOL:
		bx_cgpc_add_instruction(pcode, BX_INSTR_RLOAD32);
		bx_cgpc_add_identifier(pcode, expression->value.variable->identifier);
		break;
	case BX_COMP_VARIABLE_SYMBOL:
		bx_cgpc_add_instruction(pcode, BX_INSTR_VLOAD32);
		bx_cgpc_add_address(pcode, expression->value.variable->symbol_data.variable_number);
		break;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Unexpected variable type in function 'variable_to_binary'");
		return -1;
	}

	expression->expression_type = BX_COMP_BINARY;
	expression->value.pcode = pcode;

	return 0;
}

struct bx_comp_expr *bx_cgex_create_binary_expression(enum bx_builtin_type data_type) {
	struct bx_comp_expr *expression;

	expression = create_empty_expression();
	if (expression == NULL) {
		return NULL;
	}
	expression->value.pcode = bx_cgpc_create();
	if (expression->value.pcode == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error creating bx_comp_code struct in 'bx_cgex_create_binary_expression'");
		free(expression);
		return NULL;
	}
	expression->expression_type = BX_COMP_BINARY;
	expression->data_type = data_type;

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
		return "unknown type";
	}
}

struct bx_comp_expr *bx_cgex_copy_expression(struct bx_comp_expr *expression) {
	struct bx_comp_expr *copy;

	if (expression == NULL) {
		return NULL;
	}

	copy = create_empty_expression();;
	if (copy == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error instantiating memory in function 'bx_cgex_copy_expression'");
		return NULL;
	}

	copy->data_type = expression->data_type;
	copy->expression_type = expression->expression_type;
	if (expression->expression_type == BX_COMP_BINARY) {
		copy->value.pcode = bx_cgpc_copy(expression->value.pcode);
		if (copy->value.pcode == NULL) {
			BX_LOG(LOG_ERROR, "codegen_expression",
					"Error creating bx_comp_code struct in 'bx_cgex_create_binary_expression'");
			free(copy);
			return NULL;
		}

	} else if (expression->expression_type == BX_COMP_VARIABLE) {
		copy->value.variable = expression->value.variable;

	} else {
		copy->value = expression->value;
	}

	if (expression->side_effect_pcode != NULL) {
		bx_cgpc_replace_pcode(copy->side_effect_pcode, expression->side_effect_pcode);
	}

	return copy;
}

void bx_cgex_destroy_expression(struct bx_comp_expr *expression) {

	if (expression == NULL) {
		return;
	}

	if (expression->expression_type == BX_COMP_BINARY) {
		bx_cgpc_destroy(expression->value.pcode);
	}

	if (expression->side_effect_pcode != NULL) {
		bx_cgpc_destroy(expression->side_effect_pcode);
	}

	free(expression);
}

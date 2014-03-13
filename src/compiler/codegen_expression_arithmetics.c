/*
 * codegen_expression_arithmetics.c
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
#include "compiler/codegen_expression_arithmetics.h"
#include "compiler/codegen_expression_cast.h"

struct bx_comp_expr *prefix_inc_int(struct bx_comp_expr *operand1);
struct bx_comp_expr *prefix_inc_float(struct bx_comp_expr *operand1);

static struct bx_comp_expr *unary_plus(struct bx_comp_expr *operand1);

static struct bx_comp_expr *unary_minus_int(struct bx_comp_expr *operand1);
static struct bx_comp_expr *unary_minus_float(struct bx_comp_expr *operand1);

static struct bx_comp_expr *add_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *add_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *concat_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *sub_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *sub_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *mul_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *mul_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *div_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *div_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *mod_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/////////////////////////////////
// PREFIXED INCREMENT OPERATOR //
/////////////////////////////////

struct bx_comp_expr *bx_cgex_prefix_inc_operator(struct bx_comp_expr *operand1) {

	if (operand1->type != BX_COMP_VARIABLE) {
		BX_LOG(LOG_ERROR, "compiler",
				"Unable to apply prefixed increment operator to non-assignable expression");
		return NULL;
	}

	switch (operand1->data_type) {
	case BX_INT:
		return prefix_inc_int(operand1);
	case BX_FLOAT:
		return prefix_inc_float(operand1);
	case BX_BOOL:
	case BX_SUBNET:
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Operand not compatible with operator '++'.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected data type encountered "
				"in funciton bx_cgex_prefix_inc_operator.");
		return NULL;
	}
}

struct bx_comp_expr *prefix_inc_int(struct bx_comp_expr *operand1) {
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	result = bx_cgex_create_binary_expression(BX_INT);
	if (result == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error creating binary expression in functin 'prefix_inc_int'");
		return NULL;
	}
	code = result->bx_value.code;
	bx_cgco_add_instruction(code, BX_INSTR_LOAD32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);
	bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
	bx_cgco_add_instruction(code, BX_INSTR_IADD);
	bx_cgco_add_instruction(code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);
	bx_cgco_add_instruction(code, BX_INSTR_LOAD32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);

	return result;
}

struct bx_comp_expr *prefix_inc_float(struct bx_comp_expr *operand1) {
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	if (result == NULL) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error creating binary expression in functin 'prefix_inc_int'");
		return NULL;
	}
	code = result->bx_value.code;
	bx_cgco_add_instruction(code, BX_INSTR_LOAD32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);
	bx_cgco_add_instruction(code, BX_INSTR_FPUSH_1);
	bx_cgco_add_instruction(code, BX_INSTR_FADD);
	bx_cgco_add_instruction(code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);
	bx_cgco_add_instruction(code, BX_INSTR_LOAD32);
	bx_cgco_add_identifier(code, operand1->bx_value.identifier);

	return result;
}

/////////////////////////
// UNARY PLUS OPERATOR //
/////////////////////////

struct bx_comp_expr *bx_cgex_unary_plus_operator(struct bx_comp_expr *operand1) {

	switch (operand1->data_type) {
	case BX_INT:
	case BX_FLOAT:
		return unary_plus(operand1);
	case BX_BOOL:
	case BX_SUBNET:
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Operand not compatible with unary operator '+'.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected data type encountered "
				"in funciton bx_cgex_unary_plus_operator.");
		return NULL;
	}
}

static struct bx_comp_expr *unary_plus(struct bx_comp_expr *operand1) {
	bx_int8 error;
	struct bx_comp_expr *result;

	result = bx_cgex_copy_expression(operand1);

	if (result->type == BX_COMP_CONSTANT) {
		return result;
	}

	error = bx_cgex_convert_to_binary(result);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'unary_plus'");
		return NULL;
	}

	return result;
}

//////////////////////////
// UNARY MINUS OPERATOR //
//////////////////////////

struct bx_comp_expr *bx_cgex_unary_minus_operator(struct bx_comp_expr *operand1) {

	switch (operand1->data_type) {
	case BX_INT:
		return unary_minus_int(operand1);
	case BX_FLOAT:
		return unary_minus_float(operand1);
	case BX_BOOL:
	case BX_SUBNET:
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Operand not compatible with unary operator '+'.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected data type encountered "
				"in funciton bx_cgex_unary_minus_operator.");
		return NULL;
	}
}

static struct bx_comp_expr *unary_minus_int(struct bx_comp_expr *operand1) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(- operand1->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'unary_minus_int'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_INEG);

	return result;
}

static struct bx_comp_expr *unary_minus_float(struct bx_comp_expr *operand1) {
	bx_int8 error;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(- operand1->bx_value.float_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'unary_minus_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FNEG);

	return result;
}

///////////////////////
// ADDITION OPERATOR //
///////////////////////

struct bx_comp_expr *bx_cgex_addition_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

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
		operand1 = bx_cgex_int_to_float(operand1);
		return add_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return add_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return concat_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '+'.");
		return NULL;
	}
}

static struct bx_comp_expr *add_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value + operand2->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'add_int'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IADD);

	return result;
}

static struct bx_comp_expr *add_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.float_value + operand2->bx_value.float_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'add_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FADD);

	return result;
}

static struct bx_comp_expr *concat_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

//////////////////////////
// SUBTRACTION OPERATOR //
//////////////////////////

struct bx_comp_expr *bx_cgex_subtraction_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '-'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return sub_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return sub_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return sub_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return sub_float(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '-'.");
		return NULL;
	}
}

static struct bx_comp_expr *sub_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value - operand2->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'sub_int'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_ISUB);

	return result;
}

static struct bx_comp_expr *sub_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.float_value - operand2->bx_value.float_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'sub_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FSUB);

	return result;
}

/////////////////////////////
// MULTIPLICATION OPERATOR //
/////////////////////////////

struct bx_comp_expr *bx_cgex_multiplication_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '*'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return mul_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return mul_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return mul_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return mul_float(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '*'.");
		return NULL;
	}
}

static struct bx_comp_expr *mul_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value * operand2->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'mul_int'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IMUL);

	return result;
}

static struct bx_comp_expr *mul_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.float_value * operand2->bx_value.float_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'mul_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FMUL);

	return result;
}

///////////////////////
// DIVISION OPERATOR //
///////////////////////

struct bx_comp_expr *bx_cgex_division_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '/'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return div_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return div_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return div_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return div_float(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '/'.");
		return NULL;
	}
}

static struct bx_comp_expr *div_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value / operand2->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'div_int'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IDIV);

	return result;
}

static struct bx_comp_expr *div_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.float_value / operand2->bx_value.float_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'div_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_FLOAT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FDIV);

	return result;
}

/////////////////////
// MODULO OPERATOR //
/////////////////////

struct bx_comp_expr *bx_cgex_modulo_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL ||
			operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '%'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return mod_int(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '%'.");
		return NULL;
	}
}

static struct bx_comp_expr *mod_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	bx_int8 error = 0;
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_int_constant(operand1->bx_value.int_value % operand2->bx_value.int_value);
	}

	error = bx_cgex_convert_to_binary(operand1);
	error += bx_cgex_convert_to_binary(operand2);
	if (error != 0) {
		BX_LOG(LOG_ERROR, "codegen_expression",
				"Error converting expression to binary in function 'div_float'");
		return NULL;
	}

	result = bx_cgex_create_binary_expression(BX_INT);
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IMOD);

	return result;
}

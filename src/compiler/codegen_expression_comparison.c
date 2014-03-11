/*
 * codegen_expression_comparison.c
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
#include "compiler/codegen_expression_comparison.h"
#include "compiler/codegen_expression_cast.h"

static struct bx_comp_expr *equals_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *equals_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *equals_bool(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *equals_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *not_equals_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *not_equals_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *not_equals_bool(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *not_equals_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *greater_than_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *greater_than_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *greater_than_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *greater_or_equal_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *greater_or_equal_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *greater_or_equal_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *less_than_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *less_than_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *less_than_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

static struct bx_comp_expr *less_or_equal_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *less_or_equal_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);
static struct bx_comp_expr *less_or_equal_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

///////////////////////
// EQUALITY OPERATOR //
///////////////////////

struct bx_comp_expr *bx_cgex_equality_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '=='.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return equals_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return equals_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return equals_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return equals_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return equals_strings(operand1, operand2);

	} else if (operand1->data_type == BX_BOOL && operand2->data_type == BX_BOOL) {
		return equals_bool(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '=='.");
		return NULL;
	}
}

static struct bx_comp_expr *equals_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value == operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IEQ);

	return result;
}

static struct bx_comp_expr *equals_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value == operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FEQ);

	return result;
}

static struct bx_comp_expr *equals_bool(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.bool_value == operand2->bx_value.bool_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IEQ);

	return result;
}

static struct bx_comp_expr *equals_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

/////////////////////////
// INEQUALITY OPERATOR //
/////////////////////////

struct bx_comp_expr *bx_cgex_inequality_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '!='.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return not_equals_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return not_equals_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return not_equals_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return not_equals_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return not_equals_strings(operand1, operand2);

	} else if (operand1->data_type == BX_BOOL && operand2->data_type == BX_BOOL) {
		return not_equals_bool(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '!='.");
		return NULL;
	}
}

static struct bx_comp_expr *not_equals_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value != operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_INE);

	return result;
}

static struct bx_comp_expr *not_equals_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value != operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FNE);

	return result;
}

static struct bx_comp_expr *not_equals_bool(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_float_constant(operand1->bx_value.bool_value != operand2->bx_value.bool_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_INE);

	return result;
}

static struct bx_comp_expr *not_equals_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

///////////////////////////
// GREATER THAN OPERATOR //
///////////////////////////

struct bx_comp_expr *bx_cgex_greater_than_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '>'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return greater_than_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return greater_than_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return greater_than_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return greater_than_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return greater_than_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '>'.");
		return NULL;
	}
}

static struct bx_comp_expr *greater_than_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value > operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IGT);

	return result;
}

static struct bx_comp_expr *greater_than_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value > operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FGT);

	return result;
}

static struct bx_comp_expr *greater_than_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

///////////////////////////////
// GREATER OR EQUAL OPERATOR //
///////////////////////////////

struct bx_comp_expr *bx_cgex_greater_or_equal_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '>='.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return greater_or_equal_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return greater_or_equal_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return greater_or_equal_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return greater_or_equal_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return greater_or_equal_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '>='.");
		return NULL;
	}
}

static struct bx_comp_expr *greater_or_equal_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value >= operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_IGE);

	return result;
}

static struct bx_comp_expr *greater_or_equal_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value >= operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FGE);

	return result;
}

static struct bx_comp_expr *greater_or_equal_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

////////////////////////
// LESS THAN OPERATOR //
////////////////////////

struct bx_comp_expr *bx_cgex_less_than_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '<'.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return less_than_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return less_than_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return less_than_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return less_than_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return less_than_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '<'.");
		return NULL;
	}
}

static struct bx_comp_expr *less_than_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value < operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_ILT);

	return result;
}

static struct bx_comp_expr *less_than_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value < operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FLT);

	return result;
}

static struct bx_comp_expr *less_than_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

///////////////////////////////
// LESS OR EQUAL OPERATOR //
///////////////////////////////

struct bx_comp_expr *bx_cgex_less_or_equal_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	if (operand1->data_type == BX_SUBNET || operand2->data_type == BX_SUBNET ||
			operand1->data_type == BX_STREAM || operand2->data_type == BX_STREAM ||
			operand1->data_type == BX_BOOL || operand2->data_type == BX_BOOL) {
		BX_LOG(LOG_ERROR, "codegen_expression", "Operands not compatible with operator '<='.");
		return NULL;

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_INT) {
		return less_or_equal_int(operand1, operand2);

	} else if (operand1->data_type == BX_FLOAT && operand2->data_type == BX_FLOAT) {
		return less_or_equal_float(operand1, operand2);

	} else if (operand1->data_type == BX_INT && operand2->data_type == BX_FLOAT) {
		operand1 = bx_cgex_int_to_float(operand1);
		return less_or_equal_float(operand1, operand2);

	} else if (operand2->data_type == BX_INT && operand1->data_type == BX_FLOAT) {
		operand2 = bx_cgex_int_to_float(operand2);
		return less_or_equal_float(operand1, operand2);

	} else if (operand1->data_type == BX_STRING || operand2->data_type == BX_STRING) {
		return less_or_equal_strings(operand1, operand2);

	} else {
		BX_LOG(LOG_ERROR, "codegen_expression", "Unexpected operand type in expression '<='.");
		return NULL;
	}
}

static struct bx_comp_expr *less_or_equal_int(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.int_value <= operand2->bx_value.int_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_ILE);

	return result;
}

static struct bx_comp_expr *less_or_equal_float(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {
	struct bx_comp_expr *result;

	if (operand1->type == BX_COMP_CONSTANT && operand2->type == BX_COMP_CONSTANT) {
		return bx_cgex_create_bool_constant(operand1->bx_value.float_value <= operand2->bx_value.float_value);
	}

	if (operand1->type == BX_COMP_CONSTANT) {
		operand1 = bx_cgex_constant_to_binary(operand1);
	}

	if (operand2->type == BX_COMP_CONSTANT) {
		operand2 = bx_cgex_constant_to_binary(operand2);
	}

	result = bx_cgex_create_code_expression();
	result->data_type = BX_BOOL;
	bx_cgco_append_code(result->bx_value.code, operand1->bx_value.code);
	bx_cgco_append_code(result->bx_value.code, operand2->bx_value.code);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_FLE);

	return result;
}

static struct bx_comp_expr *less_or_equal_strings(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2) {

	return NULL; //TODO: Stub
}

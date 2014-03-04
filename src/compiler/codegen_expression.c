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
#include "compiler/memory_utils.h"
#include "compiler/codegen_expression.h"

struct bx_comp_expr *bx_cgex_create_int_constant(bx_int32 value) {
	struct bx_comp_expr *expression;

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	expression->data_type = BX_INT;
	expression->qualifier = BX_COMP_CONSTANT;
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
	expression->qualifier = BX_COMP_CONSTANT;
	expression->bx_value.float_value = value;

	return expression;
}

struct bx_comp_expr *bx_cgex_create_variable(char *identifier) {
	struct bx_comp_expr *expression;

	if (identifier == NULL) {
		return NULL;
	}

	expression = BX_MALLOC(struct bx_comp_expr);
	if (expression == NULL) {
		return NULL;
	}

	//TODO: Lookup in symbol table
	expression->qualifier = BX_COMP_VARIABLE;
	strncpy((void *) expression->bx_value.identifier, (void *) identifier, DM_FIELD_IDENTIFIER_LENGTH);

	return expression;
}

struct bx_comp_expr *bx_cgex_arithmetic_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operation operation) {
	struct bx_comp_expr *result;

	if (operand1 == NULL || operand2 == NULL) {
		return NULL;
	}

	result = BX_MALLOC(struct bx_comp_expr);
	if (result == NULL) {
		return NULL;
	}

	//TODO: Some additional checks on the data type (avoid int and string, etc)
	//TODO: For the time being the compiler will not perform any computation, even when there's static data
	// (e.g., 5 + 4 * 3 will not be substituted by 17)

	return NULL;
}

void bx_cgex_free_expression(struct bx_comp_expr *expression) {

	if (expression == NULL) {
		return;
	}

	free(expression);
}

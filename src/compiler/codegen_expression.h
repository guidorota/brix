/*
 * codegen_expression.h
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

#ifndef CODEGEN_EXPRESSION_H_
#define CODEGEN_EXPRESSION_H_

#include "types.h"
#include "configuration.h"

enum bx_comp_operation {
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	NOT,
	AND,
	OR,
	XOR
};

enum bx_comp_qualifier {
	BX_COMP_CONSTANT,	// Constant value
	BX_COMP_VARIABLE,	// Variable corresponding to the identifier
	BX_COMP_EXPRESSION	// An expression in binary form
};

struct bx_comp_expr {
	enum bx_builtin_type data_type;
	enum bx_comp_qualifier qualifier;
	union bx_value {
		bx_int32 int_value;
		bx_float32 float_value;
		char identifier[DM_FIELD_IDENTIFIER_LENGTH];
	} bx_value;
};

struct bx_comp_expr *bx_cgen_create_int_constant(bx_int32 value);

struct bx_comp_expr *bx_cgen_create_float_constant(bx_float32 value);

struct bx_comp_expr *bx_cgen_create_variable(char *identifier);

struct bx_comp_expr *bx_cgen_arithmetic_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operation operation);

void bx_cgen_free_expression(struct bx_comp_expr *expression);

#endif /* CODEGEN_EXPRESSION_H_ */

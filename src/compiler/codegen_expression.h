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
#include "compiler/codegen_code.h"

enum bx_comp_operator {
	BX_COMP_OP_ADD,
	BX_COMP_OP_SUB,
	BX_COMP_OP_MUL,
	BX_COMP_OP_DIV,
	BX_COMP_OP_MOD,
	BX_COMP_OP_UNARY_PLUS,
	BX_COMP_OP_UNARY_MINUS,
	BX_COMP_OP_PREFIX_INC,
	BX_COMP_OP_PREFIX_DEC,
	BX_COMP_OP_POSTFIX_INC,
	BX_COMP_OP_POSTFIX_DEC,
	BX_COMP_OP_AND,
	BX_COMP_OP_OR,
	BX_COMP_OP_NOT,
	BX_COMP_OP_BITWISE_AND,
	BX_COMP_OP_BITWISE_OR,
	BX_COMP_OP_BITWISE_XOR,
	BX_COMP_OP_BITWISE_COMPLEMENT,
	BX_COMP_OP_EQ,
	BX_COMP_OP_NE,
	BX_COMP_OP_GT,
	BX_COMP_OP_GE,
	BX_COMP_OP_LT,
	BX_COMP_OP_LE
};

enum bx_comp_exp_type {
	BX_COMP_CONSTANT,	// Constant value
	BX_COMP_BINARY		// An expression in binary form
};

struct bx_comp_expr {
	enum bx_builtin_type data_type;
	enum bx_comp_exp_type type;
	union bx_value {
		bx_int32 int_value;
		bx_float32 float_value;
		bx_boolean bool_value;
		struct bx_comp_code *code;
	} bx_value;
};

struct bx_comp_expr *bx_cgex_create_int_constant(bx_int32 value);

struct bx_comp_expr *bx_cgex_create_float_constant(bx_float32 value);

struct bx_comp_expr *bx_cgex_create_bool_constant(bx_boolean value);

struct bx_comp_expr *bx_cgex_create_variable(char *identifier);

struct bx_comp_expr *bx_cgex_cast(struct bx_comp_expr *expression, enum bx_builtin_type type);

struct bx_comp_expr *bx_cgex_binary_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operator operator);

struct bx_comp_expr *bx_cgex_unary_expression(struct bx_comp_expr *operand1, enum bx_comp_operator operator);

struct bx_comp_expr *bx_cgex_constant_to_binary(struct bx_comp_expr *value);

struct bx_comp_expr *bx_cgex_create_code_expression(enum bx_builtin_type data_type);

char *bx_cgex_get_type_name(enum bx_builtin_type type);

struct bx_comp_expr *bx_cgex_copy_expression(struct bx_comp_expr *expression);

void bx_cgex_destroy_expression(struct bx_comp_expr *expression);

#endif /* CODEGEN_EXPRESSION_H_ */

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
#include "compiler/codegen_symbol_table.h"

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
	BX_COMP_OP_LE,
	BX_COMP_OP_ASSIGNMENT
};

enum bx_comp_exp_type {
	BX_COMP_CONSTANT,	// Constant value
	BX_COMP_VARIABLE,	// A variable
	BX_COMP_BINARY		// An expression in binary form
};

struct bx_comp_expr {
	enum bx_builtin_type data_type;
	enum bx_comp_exp_type type;
	union bx_expr_value {
		bx_int32 int_value;
		bx_float32 float_value;
		bx_boolean bool_value;
		struct bx_comp_symbol variable;
		struct bx_comp_code *code;
	} value;
};


/**
 * Creates a new constant int expression
 *
 * @param value Integer value of the constant
 *
 * @return Constant int expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_create_int_constant(bx_int32 value);

/**
 * Creates a new constant flaot expression
 *
 * @param value Float value of the constant
 *
 * @return Constant float expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_create_float_constant(bx_float32 value);

/**
 * Creates a new constant boolean expression
 *
 * @param value Boolean value of the constant
 *
 * @return Constant boolean expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_create_bool_constant(bx_boolean value);

/**
 * Creates a new variable expression.
 * This function checks if the identifier passed as parameter has been declared
 * by performing a lookup on the symbol table. The variable data type is inherited
 * by the expression.
 *
 * @param symbol_table Current symbol_table
 * @param identifier Name of the variable
 *
 * @return Constant int expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_create_variable(struct bx_comp_symbol_table *symbol_table, char *identifier);

/**
 * Casts the expression passed as a parameter to the specified type.
 * This function creates a new bx_comp_expr struct. The original
 * expression passed as a parameter must be deallocated manually.
 *
 * @param expression Expression to cast
 * @param type Cast data type
 *
 * @return Casted expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_cast(struct bx_comp_expr *expression, enum bx_builtin_type type);

/**
 * Performs a binary operation.
 * This function creates a new bx_comp-expr struct. The original operands
 * passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 * @param operator Arithmetic operator
 *
 * @return Expression corresponding to the binary operation, NULL on failure
 */
struct bx_comp_expr *bx_cgex_binary_expression(struct bx_comp_expr *operand1,
		struct bx_comp_expr *operand2, enum bx_comp_operator operator);

/**
 * Performs a unary operation.
 * This function creates a new bx_comp-expr struct. The original operand
 * passed as parameter must be deallocated manually.
 *
 * @param operand1 Operand
 * @param operator Arithmetic operator
 *
 * @return Expression corresponding to the unary operation, NULL on failure
 */
struct bx_comp_expr *bx_cgex_unary_expression(struct bx_comp_expr *operand1, enum bx_comp_operator operator);

/**
 * Converts the expression passed as a parameter into a binary expression.
 * This function is for INTERNAL USE ONLY. No sanity checks are performed on the parameter.
 *
 * @param expression The constant expression to convert
 *
 * @return 0 if successful, -1 otherwise
 */
bx_int8 bx_cgex_convert_to_binary(struct bx_comp_expr *expression);

/**
 * Creates an empty code expression.
 * This function is for INTERNAL USE ONLY.
 *
 * @param data_type Data type of the code expression
 *
 * @return Empty code expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_create_binary_expression(enum bx_builtin_type data_type);

/**
 * Returns a descriptive name of the type passed as parameter.
 * This function is for INTERNAL USE ONLY.
 *
 * @parameter type Type name
 *
 * @return Name of the type passed as parameter, 'Unknown type' if the type is not known
 */
char *bx_cgex_get_type_name(enum bx_builtin_type type);

/**
 * Creates a copy of the expression passed as parameter
 * This function is for INTERNAL USE ONLY.
 *
 * @param expression Expression to copy
 *
 * @return Copy of the expression passed as parameter, NULL on failure
 */
struct bx_comp_expr *bx_cgex_copy_expression(struct bx_comp_expr *expression);

/**
 * Destoys an expression, reclaiming memory.
 *
 * @param expression Expression to destroy
 */
void bx_cgex_destroy_expression(struct bx_comp_expr *expression);

#endif /* CODEGEN_EXPRESSION_H_ */

/*
 * codegen_expression_cast.h
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

#ifndef CODEGEN_EXPRESSION_CAST_H_
#define CODEGEN_EXPRESSION_CAST_H_

#include "compiler/codegen_expression.h"

/**
 * Convert an expression to integer type
 * This function converts the expression passed as parameter to integer type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return integer cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_cast_to_int(struct bx_comp_expr *expression);

/**
 * Convert an expression to float type
 * This function converts the expression passed as parameter to float type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return float cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_cast_to_float(struct bx_comp_expr *expression);

/**
 * Convert an expression to boolean type
 * This function converts the expression passed as parameter to boolean type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return boolean cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_cast_to_bool(struct bx_comp_expr *expression);

/**
 * Convert an expression to string type
 * This function converts the expression passed as parameter to string type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return string cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_cast_to_string(struct bx_comp_expr *expression);

/**
 * Convert an integer expression to boolean type
 * This function converts the integer expression passed as parameter to
 * boolean type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return boolean cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_int_to_bool(struct bx_comp_expr *value);

/**
 * Convert a float expression to boolean type
 * This function converts the float expression passed as parameter to
 * boolean type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return boolean cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_float_to_bool(struct bx_comp_expr *value);

/**
 * Convert an integer expression to float type
 * This function converts the integer expression passed as parameter to
 * float type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return float cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_int_to_float(struct bx_comp_expr *value);

/**
 * Convert a boolean expression to float type
 * This function converts the boolean expression passed as parameter to
 * float type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return float cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bool_to_float(struct bx_comp_expr *value);

/**
 * Convert a float expression to integer type
 * This function converts the float expression passed as parameter to
 * integer type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return integer cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_float_to_int(struct bx_comp_expr *value);

/**
 * Convert a boolean expression to integer type
 * This function converts the boolean expression passed as parameter to
 * integer type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return integer cast of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bool_to_int(struct bx_comp_expr *value);

/**
 * Creates a copy of the expression passed as parameter.
 * If the expression is of type BX_COMP_VARIABLE, the expression gets converted
 * to BX_COMP_BINARY type.
 * The original expression passed as parameter must be deallocated manually.
 *
 * @param operand1 expression
 *
 * return copy of the expression, NULL on failure
 */
struct bx_comp_expr *bx_cgex_same_type_cast(struct bx_comp_expr *expression);

#endif /* CODEGEN_EXPRESSION_CAST_H_ */

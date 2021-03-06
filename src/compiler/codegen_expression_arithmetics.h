/*
 * codegen_expression_arithmetics.h
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

#ifndef CODEGEN_EXPRESSION_ARITHMETICS_H_
#define CODEGEN_EXPRESSION_ARITHMETICS_H_

#include "compiler/codegen_expression.h"

/**
 * Postfixed decrement operator
 * This function decreases the value of the variable expression and places the
 * old value in the stack.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return operand1 --, NULL on failure
 */
struct bx_comp_expr *bx_cgex_postfix_dec_operator(struct bx_comp_expr *operand1);

/**
 * Postfixed increment operator
 * This function increases the value of the variable expression and places the
 * old value in the stack.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return operand1 ++, NULL on failure
 */
struct bx_comp_expr *bx_cgex_postfix_inc_operator(struct bx_comp_expr *operand1);

/**
 * Prefixed decrement operator
 * This function decreses the value of the variable expression and places the
 * new value in the stack.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return -- operand1, NULL on failure
 */
struct bx_comp_expr *bx_cgex_prefix_dec_operator(struct bx_comp_expr *operand1);

/**
 * Prefixed increment operator
 * This function increases the value of the variable expression and places the
 * new value in the stack.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return ++ operand1, NULL on failure
 */
struct bx_comp_expr *bx_cgex_prefix_inc_operator(struct bx_comp_expr *operand1);

/**
 * Unary plus
 * This function basically returns a copy of the expression passed as parameter.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return operand1 copy, NULL on failure
 */
struct bx_comp_expr *bx_cgex_unary_plus_operator(struct bx_comp_expr *operand1);

/**
 * Unary minus
 * This function returns a new expression corresponding to the negation
 * of the operand passed as parameter
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return Negation of operand1, NULL on failure
 */
struct bx_comp_expr *bx_cgex_unary_minus_operator(struct bx_comp_expr *operand1);

/**
 * Addition
 * This function returns a new expression corresponding to the arithmetic
 * addition of the operands.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 + operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_addition_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Subtraction
 * This function returns a new expression corresponding to the arithmetic
 * subtraction of the operands.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 - operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_subtraction_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Multiplication
 * This function returns a new expression corresponding to the arithmetic
 * multiplication of the operands.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 * operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_multiplication_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Division
 * This function returns a new expression corresponding to the arithmetic
 * division of the operands.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 / operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_division_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Modulo
 * This function returns a new expression corresponding to the integer
 * remainder of the division operation between the operands.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 % operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_modulo_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

#endif /* CODEGEN_EXPRESSION_ARITHMETICS_H_ */

/*
 * codegen_expression_comparison.h
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

#ifndef CODEGEN_EXPRESSION_COMPARISON_H_
#define CODEGEN_EXPRESSION_COMPARISON_H_

#include "compiler/codegen_expression.h"

/**
 * Equality comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 == operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 == opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_equality_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Inequality comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 != operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 != opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_inequality_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Greater than comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 > operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 > opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_greater_than_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Greater or equal comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 >= operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 >= opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_greater_or_equal_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Less than comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 < operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 < opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_less_than_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Less than or equal comparison
 * This function returns a new boolean expression whose value is TRUE
 * if operand1 <= operand2, FALSE otherwise.
 * The original operands passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return New boolean expression with value operand1 <= opernad2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_less_or_equal_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

#endif /* CODEGEN_EXPRESSION_COMPARISON_H_ */

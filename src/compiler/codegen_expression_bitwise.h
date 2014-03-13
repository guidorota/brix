/*
 * codegen_expression_bitwise.h
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

#ifndef CODEGEN_EXPRESSION_BITWISE_H_
#define CODEGEN_EXPRESSION_BITWISE_H_

#include "compiler/codegen_expression.h"

/**
 * Bitwise complement
 * This function returns the bitwise complement of the integer expression
 * passed as parameter.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 *
 * return ~operand1, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bitwise_complement_operator(struct bx_comp_expr *operand1);

/**
 * Bitwise or
 * This function returns the bitwise or of the integer expressions
 * passed as parameter.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 | operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bitwise_or_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Bitwise xor
 * This function returns the bitwise xor of the integer expressions
 * passed as parameter.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 ^ operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bitwise_xor_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Bitwise and
 * This function returns the bitwise and of the integer expressions
 * passed as parameter.
 * The original operand passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return operand1 & operand2, NULL on failure
 */
struct bx_comp_expr *bx_cgex_bitwise_and_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

#endif /* CODEGEN_EXPRESSION_BITWISE_H_ */

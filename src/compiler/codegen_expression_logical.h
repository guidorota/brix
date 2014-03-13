/*
 * codegen_expression_logical.h
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

#ifndef CODEGEN_EXPRESSION_LOGICAL_H_
#define CODEGEN_EXPRESSION_LOGICAL_H_

#include "compiler/codegen_expression.h"

/**
 * Logical negation.
 * This function creates a new expression corresponding to the negation
 * of the expression passed as a parameter. The original operand
 * passed as parameter must be deallocated manually.
 *
 * @param operand1 Operand to negate
 *
 * return Logical not of the operand, NULL on failure
 */
struct bx_comp_expr *bx_cgex_logical_not_operator(struct bx_comp_expr *operand1);

/**
 * Logical or.
 * This function creates a new expression corresponding to the or
 * of the two expression operands passed as a parameter. The original operands
 * passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return Logical or of the operands, NULL on failure
 */
struct bx_comp_expr *bx_cgex_logical_or_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

/**
 * Logical and.
 * This function creates a new expression corresponding to the and
 * of the two expression operands passed as a parameter. The original operands
 * passed as parameter must be deallocated manually.
 *
 * @param operand1 First operand
 * @param operand2 Second operand
 *
 * return Logical and of the operands, NULL on failure
 */
struct bx_comp_expr *bx_cgex_logical_and_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

#endif /* CODEGEN_EXPRESSION_LOGICAL_H_ */

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

struct bx_comp_expr *bx_cgex_addition_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

struct bx_comp_expr *bx_cgex_subtraction_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

struct bx_comp_expr *bx_cgex_multiplication_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

struct bx_comp_expr *bx_cgex_division_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

struct bx_comp_expr *bx_cgex_modulo_operator(struct bx_comp_expr *operand1, struct bx_comp_expr *operand2);

#endif /* CODEGEN_EXPRESSION_ARITHMETICS_H_ */

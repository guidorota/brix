/*
 * test_codegen_expression.c
 * Created on: Mar 6, 2014
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

#include "test_codegen_expression.h"
#include "compiler/codegen_expression.h"

START_TEST (create_constant) {
	struct bx_comp_expr *expression;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;
	bx_boolean bool_value = BX_BOOLEAN_TRUE;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(bool_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, bool_value);
	bx_cgex_destroy_expression(expression);
} END_TEST

Suite *test_codegen_expression_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("create_constant");
	tcase_add_test(tcase, create_constant);
	suite_add_tcase(suite, tcase);

	return suite;
}

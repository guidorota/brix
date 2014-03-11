/*
 * test_codegen_expression_cast.c
 * Created on: Mar 11, 2014
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

#include "test_codegen_expression_cast.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

#define INT_TEST_FIELD "int_test_field"
#define FLOAT_TEST_FIELD "float_test_field"
#define BOOLEAN_TEST_FIELD "boolean_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static struct bx_document_field float_test_field;
static struct bx_test_field_data float_test_field_data;

static struct bx_document_field boolean_test_field;
static struct bx_test_field_data boolean_test_field_data;

START_TEST (init_test) {
	bx_int8 error;

	// Init virtual machine
	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);

	// Init document manager
	error = bx_dm_document_manager_init();
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&int_test_field, &int_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&float_test_field, &float_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&boolean_test_field, &boolean_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&float_test_field, FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&boolean_test_field, BOOLEAN_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Setup compiler symbol table
	error = bx_cgsy_init();
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add(INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	bx_cgsy_add(FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	bx_cgsy_add(BOOLEAN_TEST_FIELD, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (cast_to_int) {
	struct bx_comp_expr *expression;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value);
	bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value);
	bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, (bx_int32) float_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, (bx_int32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, (bx_int32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
} END_TEST

START_TEST (cast_to_float) {
	struct bx_comp_expr *expression;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value);
	bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, (bx_float32) int_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value);
	bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, (bx_float32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, (bx_float32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
} END_TEST

START_TEST (cast_to_bool) {
	struct bx_comp_expr *expression;
	bx_int32 int_value = 93;
	bx_int32 int_value_0 = 0;
	bx_float32 float_value = 78.349;
	bx_float32 float_value_0 = 0.0;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_int_constant(int_value_0);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->bx_value.int_value, int_value_0);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value_0);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->bx_value.float_value, float_value_0);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->bx_value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
} END_TEST

Suite *test_codegen_expression_cast_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_to_int");
	tcase_add_test(tcase, cast_to_int);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_to_float");
	tcase_add_test(tcase, cast_to_float);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_to_bool");
	tcase_add_test(tcase, cast_to_bool);
	suite_add_tcase(suite, tcase);

	return suite;
}

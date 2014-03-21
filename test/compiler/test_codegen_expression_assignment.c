/*
 * test_codegen_expression_assignment.c
 * Created on: Mar 13, 2014
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

#include "test_codegen_expression_assignment.h"

#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"
#include "compiler/codegen_expression_assignment.h"

#define INT_TEST_FIELD "int_test_field"
#define OUTPUT_INT_TEST_FIELD "output_int_test_field"
#define FLOAT_TEST_FIELD "float_test_field"
#define OUTPUT_FLOAT_TEST_FIELD "output_float_test_field"
#define BOOL_TEST_FIELD "bool_test_field"
#define OUTPUT_BOOL_TEST_FIELD "output_bool_test_field"

#define LOCAL_VARIABLE_TEST "local_variable"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static struct bx_document_field output_int_test_field;
static struct bx_test_field_data output_int_test_field_data;

static struct bx_document_field float_test_field;
static struct bx_test_field_data float_test_field_data;

static struct bx_document_field output_float_test_field;
static struct bx_test_field_data output_float_test_field_data;

static struct bx_document_field bool_test_field;
static struct bx_test_field_data bool_test_field_data;

static struct bx_document_field output_bool_test_field;
static struct bx_test_field_data output_bool_test_field_data;

static struct bx_comp_symbol_table *symbol_table;

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
	error = bx_dm_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&output_int_test_field, &output_int_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&output_int_test_field, OUTPUT_INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	error = bx_test_field_init(&float_test_field, &float_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&float_test_field, FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&output_float_test_field, &output_float_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&output_float_test_field, OUTPUT_FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	error = bx_test_field_init(&bool_test_field, &bool_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&bool_test_field, BOOL_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&output_bool_test_field, &output_bool_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&output_bool_test_field, OUTPUT_BOOL_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Setup compiler symbol table
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, NULL);
	error = bx_cgsy_add_field(symbol_table, INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, OUTPUT_INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	error = bx_cgsy_add_field(symbol_table, FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, OUTPUT_FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	error = bx_cgsy_add_field(symbol_table, BOOL_TEST_FIELD, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, OUTPUT_BOOL_TEST_FIELD, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	error = bx_cgsy_add_variable(symbol_table, LOCAL_VARIABLE_TEST, BX_INT);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (int_assignment) {
	bx_int8 error;
	bx_int32 value = 12;
	struct bx_comp_expr *destination;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	bx_test_field_set_int(&int_test_field, 0);
	destination = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(destination, NULL);
	expression = bx_cgex_create_int_constant(value);
	result = bx_cgex_assignment_expression(destination, expression);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	code = result->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, OUTPUT_INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), value);
	ck_assert_int_eq(bx_test_field_get_int(&output_int_test_field), value);
} END_TEST

START_TEST (float_assignment) {
	bx_int8 error;
	bx_float32 value = 12.98;
	struct bx_comp_expr *destination;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	bx_test_field_set_float(&float_test_field, 0);
	destination = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(destination, NULL);
	expression = bx_cgex_create_float_constant(value);
	result = bx_cgex_assignment_expression(destination, expression);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	code = result->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, OUTPUT_FLOAT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), value);
	ck_assert_int_eq(bx_test_field_get_float(&output_float_test_field), value);
} END_TEST

START_TEST (bool_assignment) {
	bx_int8 error;
	struct bx_comp_expr *destination;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	bx_test_field_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	destination = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(destination, NULL);
	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	result = bx_cgex_assignment_expression(destination, expression);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	code = result->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, OUTPUT_BOOL_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&bool_test_field), BX_BOOLEAN_TRUE);
	ck_assert_int_eq(bx_test_field_get_bool(&output_bool_test_field), BX_BOOLEAN_TRUE);
} END_TEST

START_TEST (local_variable_assignment) {
	bx_int8 error;
	bx_int32 value = 84;
	struct bx_comp_expr *destination;
	struct bx_comp_expr *constant;
	struct bx_comp_expr *result;
	struct bx_comp_code *code;

	bx_test_field_set_int(&int_test_field, 0);
	destination = bx_cgex_create_variable(symbol_table, LOCAL_VARIABLE_TEST);
	constant = bx_cgex_create_int_constant(value);
	result =  bx_cgex_assignment_expression(destination, constant);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	code = result->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, OUTPUT_INT_TEST_FIELD);
	bx_cgco_add_instruction(code, BX_INSTR_VLOAD32);
	bx_cgco_add_address(code, 0);
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), value);
	ck_assert_int_eq(bx_test_field_get_int(&output_int_test_field), value);
} END_TEST

START_TEST (cleanup) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_expression_assignment_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("int_assignment");
	tcase_add_test(tcase, int_assignment);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("float_assignment");
	tcase_add_test(tcase, float_assignment);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bool_assignment");
	tcase_add_test(tcase, bool_assignment);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("local_variable_assignment");
	tcase_add_test(tcase, local_variable_assignment);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cleanup");
	tcase_add_test(tcase, cleanup);
	suite_add_tcase(suite, tcase);

	return suite;
}

/*
 * test_codegen_expression_logical.c
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

#include "test_codegen_expression_logical.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

#define BOOL_TEST_FIELD "bool_test_field"

static struct bx_document_field bool_test_field;
static struct bx_test_field_data bool_test_field_data;

static struct bx_comp_symbol_table *symbol_table;

START_TEST (init_test) {
	bx_int8 error;

	// Init virtual machine
	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);

	// Init document manager
	error = bx_docman_init();
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&bool_test_field, &bool_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&bool_test_field, BOOL_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Setup compiler symbol table
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, 0);
	error = bx_cgsy_add_field(symbol_table, BOOL_TEST_FIELD, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (logical_or_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;

	// FALSE constant, FALSE constant
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// TRUE binary, TRUE binary
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_TRUE);
	operand1 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&bool_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// TRUE constant, FALSE binary
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// FALSE binary, TRUE constant
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (logical_and_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;

	// FALSE constant, FALSE constant
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// TRUE binary, TRUE binary
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_TRUE);
	operand1 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&bool_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// TRUE constant, FALSE binary
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// FALSE binary, TRUE constant
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	operand2 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (logical_not_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;

	// FALSE constant
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_NOT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// TRUE constant
	operand1 = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_NOT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// FALSE binary
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_FALSE);
	operand1 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_NOT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// TRUE binary
	bx_tfield_set_bool(&bool_test_field, BX_BOOLEAN_TRUE);
	operand1 = bx_cgex_create_variable(symbol_table, BOOL_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_NOT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_BOOL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, BOOL_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_bool(&bool_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (cleanup) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_expression_logical_create_suite(void) {
	Suite *suite = suite_create("codegen_expression_logical");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("logical_or_test");
	tcase_add_test(tcase, logical_or_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("logical_and_test");
	tcase_add_test(tcase, logical_and_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("logical_not_test");
	tcase_add_test(tcase, logical_not_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cleanup");
	tcase_add_test(tcase, cleanup);
	suite_add_tcase(suite, tcase);

	return suite;
}

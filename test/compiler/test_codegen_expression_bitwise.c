/*
 * test_codegen_expression_bitwise.c
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

#include "test_codegen_expression_bitwise.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

#define CODE_BUFFER_LENGTH 128
#define INT_TEST_FIELD "int_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

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

	// Setup compiler symbol table
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, NULL);
	error = bx_cgsy_add_field(symbol_table, INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (bitwise_or_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 63;
	bx_int32 int_operand2 = -89;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 | int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 | int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_test_field_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 | int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int binary, Operand2 Int constant
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	operand1 = bx_cgex_create_int_constant(int_operand2);
		ck_assert_ptr_ne(operand1, NULL);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_OR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand2 | int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (bitwise_xor_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 63;
	bx_int32 int_operand2 = -89;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_XOR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 ^ int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_XOR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 ^ int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_test_field_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_XOR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 ^ int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int binary, Operand2 Int constant
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	operand1 = bx_cgex_create_int_constant(int_operand2);
		ck_assert_ptr_ne(operand1, NULL);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_XOR);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand2 ^ int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (bitwise_and_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 63;
	bx_int32 int_operand2 = -89;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 & int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 & int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_test_field_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 & int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int binary, Operand2 Int constant
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	operand1 = bx_cgex_create_int_constant(int_operand2);
		ck_assert_ptr_ne(operand1, NULL);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_BITWISE_AND);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand2 & int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (bitwise_not_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 int_value0 = 0;
	bx_int32 int_value1 = 63;
	bx_int32 int_value2 = -56;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_value0);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, ~int_value0);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_value1);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, ~int_value1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_value2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, ~int_value2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, binary
	bx_test_field_set_int(&int_test_field, int_value0);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), ~int_value0);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, binary
	bx_test_field_set_int(&int_test_field, int_value1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), ~int_value1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, binary
	bx_test_field_set_int(&int_test_field, int_value2);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_BITWISE_COMPLEMENT);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.code, NULL);
	bx_cgco_add_instruction(result->value.code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(result->value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.code->data, result->value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), ~int_value2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (cleanup) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_expression_bitwise_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bitwise_or_test");
	tcase_add_test(tcase, bitwise_or_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bitwise_xor_test");
	tcase_add_test(tcase, bitwise_xor_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bitwise_and_test");
	tcase_add_test(tcase, bitwise_and_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bitwise_not_test");
	tcase_add_test(tcase, bitwise_not_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cleanup");
	tcase_add_test(tcase, cleanup);
	suite_add_tcase(suite, tcase);

	return suite;
}

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
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

#define CODE_BUFFER_LENGTH 128
#define INT_TEST_FIELD "int_test_field"
#define FLOAT_TEST_FIELD "float_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static struct bx_document_field float_test_field;
static struct bx_test_field_data float_test_field_data;

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
	error = bx_dm_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&float_test_field, FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Setup compiler symbol table
	error = bx_cgsy_init();
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add(INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	bx_cgsy_add(FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

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

START_TEST (create_variable) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_code *code;
	bx_int32 int_value = 90;

	bx_test_field_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_BINARY);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_ptr_ne(expression->bx_value.code, NULL);
	code = expression->bx_value.code;
	bx_cgco_add_instruction(code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_value);
	bx_cgex_destroy_expression(expression);
} END_TEST

START_TEST (plus_operator) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 87;
	bx_int32 int_operand2 = -12;
	bx_float32 float_operand1 = 87.0029;
	bx_float32 float_operand2 = 23.877;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->bx_value.int_value, int_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->bx_value.float_value, float_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->bx_value.float_value, int_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->bx_value.float_value, float_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_test_field_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 2 * int_operand1);

	// Operand1 Float, Operand2 Float, binary
	bx_test_field_set_float(&float_test_field, float_operand1);
	operand1 = bx_cgex_create_variable(FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), 2 * float_operand1);

	// Operand1 Int, Operand2 Float, binary
	bx_test_field_set_int(&int_test_field, int_operand1);
	bx_test_field_set_float(&float_test_field, float_operand2);
	operand1 = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), int_operand1 + float_operand2);

	// Operand1 Float, Operand2 Int, binary
	bx_test_field_set_float(&float_test_field, float_operand1);
	bx_test_field_set_int(&int_test_field, int_operand2);
	operand1 = bx_cgex_create_variable(FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), float_operand1 + int_operand2);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_test_field_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, INT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_operand1 + int_operand2);

	// Operand1 Float constant, Operand2 Float binary
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_test_field_set_float(&float_test_field, float_operand2);
	operand2 = bx_cgex_create_variable(FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_expression(operand1, operand2, BX_COMP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->bx_value.code, NULL);
	bx_cgco_add_instruction(result->bx_value.code, BX_INSTR_STORE32);
	bx_cgco_add_identifier(result->bx_value.code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->bx_value.code->data, result->bx_value.code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), float_operand1 + float_operand2);

} END_TEST

Suite *test_codegen_expression_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("create_constant");
	tcase_add_test(tcase, create_constant);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("create_variable");
	tcase_add_test(tcase, create_variable);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("plus_operator");
	tcase_add_test(tcase, plus_operator);
	suite_add_tcase(suite, tcase);

	return suite;
}

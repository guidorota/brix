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

#include "test_codegen_expression_arithmetics.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_symbol_table.h"

#define INT_TEST_FIELD "int_test_field"
#define FLOAT_TEST_FIELD "float_test_field"

#define INT_OUTPUT_TEST_FIELD "int_output_test_field"
#define FLOAT_OUTPUT_TEST_FIELD "float_output_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static struct bx_document_field int_output_test_field;
static struct bx_test_field_data int_output_test_field_data;

static struct bx_document_field float_test_field;
static struct bx_test_field_data float_test_field_data;

static struct bx_document_field float_output_test_field;
static struct bx_test_field_data float_output_test_field_data;

static struct bx_comp_symbol_table *symbol_table;

START_TEST (init_test) {
	bx_int8 error;

	// Init virtual machine
	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);

	// Init document manager
	error = bx_docman_init();
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&int_test_field, &int_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&float_test_field, &float_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&float_test_field, FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&int_output_test_field, &int_output_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&float_output_test_field, &float_output_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&int_output_test_field, INT_OUTPUT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&float_output_test_field, FLOAT_OUTPUT_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Setup compiler symbol table
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, NULL);
	error = bx_cgsy_add_field(symbol_table, INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (create_constant) {
	struct bx_comp_expr *expression;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;
	bx_boolean bool_value = BX_BOOLEAN_TRUE;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->value.int_value, int_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->value.float_value, float_value);
	bx_cgex_destroy_expression(expression);

	expression = bx_cgex_create_bool_constant(bool_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, bool_value);
	bx_cgex_destroy_expression(expression);
} END_TEST

START_TEST (create_variable) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_pcode *pcode;
	bx_int32 int_value = 90;

	bx_tfield_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->expression_type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	error = bx_cgex_convert_to_binary(expression);
	ck_assert_int_eq(error, 0);
	ck_assert_ptr_ne(expression->value.pcode, NULL);
	pcode = expression->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, INT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_value);
	bx_cgex_destroy_expression(expression);
} END_TEST

START_TEST (copy_expression) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *pcopy;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;
	bx_boolean bool_value = BX_BOOLEAN_TRUE;

	expression = bx_cgex_create_int_constant(int_value);
	pcopy = bx_cgex_copy_expression(expression);
	ck_assert_ptr_ne(pcopy, NULL);
	ck_assert_ptr_ne(pcopy, expression);
	ck_assert_int_eq(pcopy->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(pcopy->data_type, BX_INT);
	ck_assert_int_eq(pcopy->value.int_value, int_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(pcopy);

	expression = bx_cgex_create_float_constant(float_value);
	pcopy = bx_cgex_copy_expression(expression);
	ck_assert_ptr_ne(pcopy, NULL);
	ck_assert_ptr_ne(pcopy, expression);
	ck_assert_int_eq(pcopy->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(pcopy->data_type, BX_FLOAT);
	ck_assert_int_eq(pcopy->value.float_value, float_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(pcopy);

	expression = bx_cgex_create_bool_constant(bool_value);
	pcopy = bx_cgex_copy_expression(expression);
	ck_assert_ptr_ne(pcopy, NULL);
	ck_assert_ptr_ne(pcopy, expression);
	ck_assert_int_eq(pcopy->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(pcopy->data_type, BX_BOOL);
	ck_assert_int_eq(pcopy->value.bool_value, bool_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(pcopy);

	bx_tfield_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->expression_type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_ptr_ne(expression->value.pcode, NULL);
	error = bx_cgex_convert_to_binary(expression);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(expression->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_ptr_ne(expression->value.pcode, NULL);
	pcopy = bx_cgex_copy_expression(expression);
	ck_assert_ptr_ne(pcopy, NULL);
	ck_assert_ptr_ne(pcopy, expression);
	ck_assert_int_eq(pcopy->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(pcopy->data_type, BX_INT);
	ck_assert_int_eq(pcopy->value.pcode->capacity, expression->value.pcode->capacity);
	ck_assert_int_eq(pcopy->value.pcode->size, expression->value.pcode->size);
	ck_assert_int_eq(memcmp(expression->value.pcode->data,
	pcopy->value.pcode->data, pcopy->value.pcode->capacity), 0);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(pcopy);
} END_TEST

START_TEST (addition_operator) {
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
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, int_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), 2 * int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Float, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), 2 * float_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), int_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 + int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float constant, Operand2 Float binary
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_ADD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 + float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

} END_TEST

START_TEST (subtraction_operator) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 63;
	bx_int32 int_operand2 = -89;
	bx_float32 float_operand1 = 12.0029;
	bx_float32 float_operand2 = 88.877;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 - int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 - float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, int_operand1 - float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 - int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), 0);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Float, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), 0);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), int_operand1 - float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 - int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 - int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float constant, Operand2 Float binary
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_SUB);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 - float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

} END_TEST

START_TEST (multiplication_operator) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 25;
	bx_int32 int_operand2 = -93;
	bx_float32 float_operand1 = 19.2029;
	bx_float32 float_operand2 = 28.87;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 * int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 * float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, int_operand1 * float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 * int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 * int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Float, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 * float_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), int_operand1 * float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 * int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 * int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float constant, Operand2 Float binary
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MUL);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 * float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (division_operator) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 34;
	bx_int32 int_operand2 = -7;
	bx_float32 float_operand1 = 2;
	bx_float32 float_operand2 = 28.87;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 / int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 / float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_float_constant(float_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, int_operand1 / float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, constant
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, float_operand1 / int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 / int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Float, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 / float_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Float, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), int_operand1 / float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float, Operand2 Int, binary
	bx_tfield_set_float(&float_test_field, float_operand1);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 / int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 / int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Float constant, Operand2 Float binary
	operand1 = bx_cgex_create_float_constant(float_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_float(&float_test_field, float_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_DIV);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_operand1 / float_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (modulo_operator) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *operand2;
	struct bx_comp_expr *result;
	bx_int32 int_operand1 = 34;
	bx_int32 int_operand2 = -7;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_int_constant(int_operand2);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MOD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, int_operand1 % int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int, Operand2 Int, binary
	bx_tfield_set_int(&int_test_field, int_operand1);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MOD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 % int_operand1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);

	// Operand1 Int constant, Operand2 Int binary
	operand1 = bx_cgex_create_int_constant(int_operand1);
	ck_assert_ptr_ne(operand1, NULL);
	bx_tfield_set_int(&int_test_field, int_operand2);
	operand2 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand2, NULL);
	result = bx_cgex_binary_expression(operand1, operand2, BX_COMP_OP_MOD);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_operand1 % int_operand2);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(operand2);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (unary_plus_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 positive_int = 34;
	bx_float32 positive_float = 93.45;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(positive_int);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_PLUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, positive_int);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, binary
	bx_tfield_set_int(&int_test_field, positive_int);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_PLUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), positive_int);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(positive_float);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_PLUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, positive_float);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Float operand, binary
	bx_tfield_set_float(&float_test_field, positive_float);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_PLUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), positive_float);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST


START_TEST (negation_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 positive_int = 34;
	bx_float32 positive_float = 93.45;

	// Int operand, constant
	operand1 = bx_cgex_create_int_constant(positive_int);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_MINUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->value.int_value, - positive_int);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Int operand, binary
	bx_tfield_set_int(&int_test_field, positive_int);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_MINUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_INT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, INT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), - positive_int);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Float operand, constant
	operand1 = bx_cgex_create_float_constant(positive_float);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_MINUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_CONSTANT);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->value.float_value, - positive_float);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	// Float operand, binary
	bx_tfield_set_float(&float_test_field, positive_float);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_UNARY_MINUS);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_ptr_ne(result->value.pcode, NULL);
	bx_cgpc_add_instruction(result->value.pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(result->value.pcode, FLOAT_TEST_FIELD);
	error = bx_vm_execute(result->value.pcode->data, result->value.pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), - positive_float);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (prefix_increment_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 int_value = 34;
	bx_float32 float_value = 93.45;
	struct bx_comp_pcode *pcode;

	bx_tfield_set_int(&int_test_field, int_value);
	bx_tfield_set_int(&int_output_test_field, 0);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_INT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_PREFIX_INC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, INT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_output_test_field), int_value + 1);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_value + 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	bx_tfield_set_float(&float_test_field, float_value);
	bx_tfield_set_float(&float_output_test_field, 0.0);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_FLOAT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_PREFIX_INC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, FLOAT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_output_test_field), float_value + 1);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_value + 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (prefix_decrement_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 int_value = 34;
	bx_float32 float_value = 93.45;
	struct bx_comp_pcode *pcode;

	bx_tfield_set_int(&int_test_field, int_value);
	bx_tfield_set_int(&int_output_test_field, 0);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_INT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_PREFIX_DEC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, INT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_output_test_field), int_value - 1);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_value - 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	bx_tfield_set_float(&float_test_field, float_value);
	bx_tfield_set_float(&float_output_test_field, 0.0);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_FLOAT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_PREFIX_DEC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, FLOAT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_output_test_field), float_value - 1);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_value - 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (postfix_increment_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 int_value = 34;
	bx_float32 float_value = 93.45;
	struct bx_comp_pcode *pcode;

	bx_tfield_set_int(&int_test_field, int_value);
	bx_tfield_set_int(&int_output_test_field, 0);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_INT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_POSTFIX_INC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, INT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_output_test_field), int_value);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_value + 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	bx_tfield_set_float(&float_test_field, float_value);
	bx_tfield_set_float(&float_output_test_field, 0.0);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_FLOAT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_POSTFIX_INC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, FLOAT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_output_test_field), float_value);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_value + 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (postfix_decrement_test) {
	bx_int8 error;
	struct bx_comp_expr *operand1;
	struct bx_comp_expr *result;
	bx_int32 int_value = 34;
	bx_float32 float_value = 93.45;
	struct bx_comp_pcode *pcode;

	bx_tfield_set_int(&int_test_field, int_value);
	bx_tfield_set_int(&int_output_test_field, 0);
	operand1 = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_INT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_POSTFIX_DEC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_INT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, INT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&int_output_test_field), int_value);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), int_value - 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);

	bx_tfield_set_float(&float_test_field, float_value);
	bx_tfield_set_float(&float_output_test_field, 0.0);
	operand1 = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(operand1, NULL);
	ck_assert_int_eq(operand1->data_type, BX_FLOAT);
	ck_assert_int_eq(operand1->expression_type, BX_COMP_VARIABLE);
	result = bx_cgex_unary_expression(operand1, BX_COMP_OP_POSTFIX_DEC);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(result->expression_type, BX_COMP_BINARY);
	ck_assert_int_eq(result->data_type, BX_FLOAT);
	pcode = result->value.pcode;
	bx_cgpc_add_instruction(pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(pcode, FLOAT_OUTPUT_TEST_FIELD);
	error = bx_vm_execute(pcode->data, pcode->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_float(&float_output_test_field), float_value);
	ck_assert_int_eq(bx_tfield_get_float(&float_test_field), float_value - 1);
	bx_cgex_destroy_expression(operand1);
	bx_cgex_destroy_expression(result);
} END_TEST

START_TEST (cleanup) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_expression_arithmetics_create_suite(void) {
	Suite *suite = suite_create("codegen_expression_arithmetics");
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

	tcase = tcase_create("copy_expression");
	tcase_add_test(tcase, copy_expression);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("addition_operator");
	tcase_add_test(tcase, addition_operator);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("subtraction_operator");
	tcase_add_test(tcase, subtraction_operator);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("multiplication_operator");
	tcase_add_test(tcase, multiplication_operator);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("division_operator");
	tcase_add_test(tcase, division_operator);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("modulo_operator");
	tcase_add_test(tcase, modulo_operator);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("unary_plus_test");
	tcase_add_test(tcase, unary_plus_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("negation_test");
	tcase_add_test(tcase, negation_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("prefix_increment_test");
	tcase_add_test(tcase, prefix_increment_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("prefix_decrement_test");
	tcase_add_test(tcase, prefix_decrement_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("postfix_increment_test");
	tcase_add_test(tcase, postfix_increment_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("postfix_decrement_test");
	tcase_add_test(tcase, postfix_decrement_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cleanup");
	tcase_add_test(tcase, cleanup);
	suite_add_tcase(suite, tcase);

	return suite;
}

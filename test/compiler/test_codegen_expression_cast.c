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
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, 0);
	error = bx_cgsy_add_field(symbol_table, INT_TEST_FIELD, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	bx_cgsy_add_field(symbol_table, FLOAT_TEST_FIELD, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	bx_cgsy_add_field(symbol_table, BOOLEAN_TEST_FIELD, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (cast_constant_to_int) {
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->value.int_value, int_value);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_INT);
	ck_assert_int_eq(cast->value.int_value, int_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->value.float_value, float_value);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_INT);
	ck_assert_int_eq(cast->value.int_value, (bx_int32) float_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_TRUE);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_INT);
	ck_assert_int_eq(cast->value.int_value, (bx_int32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_FALSE);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_INT);
	ck_assert_int_eq(cast->value.int_value, (bx_int32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cast_binary_to_int) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	struct bx_comp_code *code;
	bx_int32 int_value = 12;
	bx_float32 float_value = 87.34;

	bx_test_field_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_INT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), int_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_float(&float_test_field, float_value);
	expression = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_INT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), (bx_int32) float_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_FALSE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_INT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), (bx_int32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_TRUE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_INT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_INT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, INT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), (bx_int32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cast_constant_to_float) {
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	bx_int32 int_value = 93;
	bx_float32 float_value = 78.349;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->value.int_value, int_value);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	ck_assert_int_eq(cast->value.float_value, (bx_float32) int_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->value.float_value, float_value);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	ck_assert_int_eq(cast->value.float_value, float_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_TRUE);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	ck_assert_int_eq(cast->value.float_value, (bx_float32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_FALSE);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	ck_assert_int_eq(cast->value.float_value, (bx_float32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cast_binary_to_float) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	struct bx_comp_code *code;
	bx_int32 int_value = 12;
	bx_float32 float_value = 87.34;

	bx_test_field_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), (bx_float32) int_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_float(&float_test_field, float_value);
	expression = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), float_value);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_FALSE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), (bx_float32) BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_TRUE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_FLOAT);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_FLOAT);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, FLOAT_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&float_test_field), (bx_float32) BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cast_constant_to_bool) {
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	bx_int32 int_value = 93;
	bx_int32 int_value_0 = 0;
	bx_float32 float_value = 78.349;
	bx_float32 float_value_0 = 0.0;

	expression = bx_cgex_create_int_constant(int_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->value.int_value, int_value);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_int_constant(int_value_0);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_INT);
	ck_assert_int_eq(expression->value.int_value, int_value_0);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_float_constant(float_value);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->value.float_value, float_value);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_float_constant(float_value_0);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	ck_assert_int_eq(expression->value.float_value, float_value_0);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_TRUE);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	expression = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	ck_assert_int_eq(expression->value.bool_value, BX_BOOLEAN_FALSE);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_CONSTANT);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	ck_assert_int_eq(cast->value.bool_value, BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cast_binary_to_bool) {
	bx_int8 error;
	struct bx_comp_expr *expression;
	struct bx_comp_expr *cast;
	struct bx_comp_code *code;
	bx_int32 int_value = 12;
	bx_int32 int_value_0 = 0;
	bx_float32 float_value = 87.34;
	bx_float32 float_value_0 = 0.0;

	bx_test_field_set_int(&int_test_field, int_value);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_int(&int_test_field, int_value_0);
	expression = bx_cgex_create_variable(symbol_table, INT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_INT);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_float(&float_test_field, float_value);
	expression = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_float(&float_test_field, float_value_0);
	expression = bx_cgex_create_variable(symbol_table, FLOAT_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_FLOAT);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_FALSE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_FALSE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);

	bx_test_field_set_bool(&boolean_test_field, BX_BOOLEAN_TRUE);
	expression = bx_cgex_create_variable(symbol_table, BOOLEAN_TEST_FIELD);
	ck_assert_ptr_ne(expression, NULL);
	ck_assert_int_eq(expression->type, BX_COMP_VARIABLE);
	ck_assert_int_eq(expression->data_type, BX_BOOL);
	cast = bx_cgex_cast(expression, BX_BOOL);
	ck_assert_int_eq(cast->type, BX_COMP_BINARY);
	ck_assert_int_eq(cast->data_type, BX_BOOL);
	code = cast->value.code;
	bx_cgco_add_instruction(code, BX_INSTR_RSTORE32);
	bx_cgco_add_identifier(code, BOOLEAN_TEST_FIELD);
	error = bx_vm_execute(code->data, code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_TRUE);
	bx_cgex_destroy_expression(expression);
	bx_cgex_destroy_expression(cast);
} END_TEST

START_TEST (cleanup) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_expression_cast_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_constant_to_int");
	tcase_add_test(tcase, cast_constant_to_int);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_binary_to_int");
	tcase_add_test(tcase, cast_binary_to_int);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_constant_to_float");
	tcase_add_test(tcase, cast_constant_to_float);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_binary_to_float");
	tcase_add_test(tcase, cast_binary_to_float);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_constant_to_bool");
	tcase_add_test(tcase, cast_constant_to_bool);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_binary_to_bool");
	tcase_add_test(tcase, cast_binary_to_bool);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cleanup");
	tcase_add_test(tcase, cleanup);
	suite_add_tcase(suite, tcase);

	return suite;
}

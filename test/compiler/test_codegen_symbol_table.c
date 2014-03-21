/*
 * test_codegen_symbol_table.c
 * Created on: Mar 4, 2014
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

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "test_codegen_symbol_table.h"
#include "compiler/codegen_symbol_table.h"
#include "compiler/codegen_expression.h"
#include "utils/linked_list.h"

#define FIELD_SYMBOL_ID_1 "field_symbol1"
#define FIELD_SYMBOL_ID_2 "field_symbol2"
#define FIELD_SYMBOL_ID_3 "field_symbol3"

#define VARIABLE_SYMBOL_ID_1 "variable_symbol1"
#define VARIABLE_SYMBOL_ID_2 "variable_symbol2"
#define VARIABLE_SYMBOL_ID_3 "variable_symbol3"

#define NOT_IN_TABLE "symbol4"

struct bx_comp_symbol_table *symbol_table;

START_TEST (init_test) {
	symbol_table = bx_cgsy_create_symbol_table();
	ck_assert_ptr_ne(symbol_table, NULL);
	ck_assert_int_eq(symbol_table->current_variable_number, 0);
	ck_assert_ptr_eq(symbol_table->field_list, NULL);
	ck_assert_ptr_ne(symbol_table->current_scope, NULL);
	ck_assert_ptr_ne(symbol_table->scope_list, NULL);
	ck_assert_int_eq(bx_llist_size(symbol_table->scope_list), 1);
	ck_assert_ptr_eq(symbol_table->current_scope, symbol_table->scope_list->element);
} END_TEST

START_TEST (add_field_symbols) {
	bx_int8 error;
	struct bx_comp_symbol *symbol;

	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_1, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_2, BX_FLOAT, BX_COMP_NEW);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_3, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	symbol = bx_cgsy_get_field(symbol_table, FIELD_SYMBOL_ID_1);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_INT);
	ck_assert_int_eq(symbol->symbol_type, BX_COMP_FIELD_SYMBOL);
	ck_assert_int_eq(symbol->symbol_data.creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_1, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(symbol_table, FIELD_SYMBOL_ID_2);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_FLOAT);
	ck_assert_int_eq(symbol->symbol_type, BX_COMP_FIELD_SYMBOL);
	ck_assert_int_eq(symbol->symbol_data.creation_modifier, BX_COMP_NEW);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_2, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(symbol_table, FIELD_SYMBOL_ID_3);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_BOOL);
	ck_assert_int_eq(symbol->symbol_type, BX_COMP_FIELD_SYMBOL);
	ck_assert_int_eq(symbol->symbol_data.creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_3, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(symbol_table, NOT_IN_TABLE);
	ck_assert_ptr_eq(symbol, NULL);
} END_TEST

START_TEST (reject_duplicate_field) {
	bx_int8 error;

	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_1, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_2, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_field(symbol_table, FIELD_SYMBOL_ID_3, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (main_scope_test) {
	bx_int8 error;
	struct bx_comp_symbol *variable;

	error = bx_cgsy_add_variable(symbol_table, VARIABLE_SYMBOL_ID_1, BX_BOOL);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(symbol_table, VARIABLE_SYMBOL_ID_2, BX_INT);
	ck_assert_int_eq(error, 0);

	variable = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(variable, NULL);
	ck_assert_int_eq(memcmp(variable->identifier, VARIABLE_SYMBOL_ID_1, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(variable->data_type, BX_BOOL);
	ck_assert_int_eq(variable->symbol_type, BX_COMP_VARIABLE_SYMBOL);
	ck_assert_int_eq(variable->symbol_data.variable_number, 0);
	variable = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_2);
	ck_assert_ptr_ne(variable, NULL);
	ck_assert_int_eq(memcmp(variable->identifier, VARIABLE_SYMBOL_ID_2, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(variable->data_type, BX_INT);
	ck_assert_int_eq(variable->symbol_type, BX_COMP_VARIABLE_SYMBOL);
	ck_assert_int_eq(variable->symbol_data.variable_number, 1);
} END_TEST

START_TEST (reject_field_variable_duplicate) {
	bx_int8 error;

	error = bx_cgsy_add_variable(symbol_table, FIELD_SYMBOL_ID_1, BX_BOOL);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_variable(symbol_table, FIELD_SYMBOL_ID_2, BX_BOOL);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_variable(symbol_table, FIELD_SYMBOL_ID_3, BX_BOOL);
	ck_assert_int_ne(error, 0);

	error = bx_cgsy_add_field(symbol_table, VARIABLE_SYMBOL_ID_1, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_field(symbol_table, VARIABLE_SYMBOL_ID_2, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (variable_scopes) {
	struct bx_comp_symbol *variable;
	struct bx_comp_symbol *child_scope_var1;
	bx_int8 error;

	error = bx_cgsy_scope_down(symbol_table);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(symbol_table, VARIABLE_SYMBOL_ID_1, BX_FLOAT);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(symbol_table, VARIABLE_SYMBOL_ID_3, BX_INT);
	ck_assert_int_eq(error, 0);

	variable = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(variable, NULL);
	ck_assert_int_eq(memcmp(variable->identifier, VARIABLE_SYMBOL_ID_1, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(variable->data_type, BX_FLOAT);
	ck_assert_int_eq(variable->symbol_type, BX_COMP_VARIABLE_SYMBOL);
	ck_assert_int_eq(variable->symbol_data.variable_number, 2);
	variable = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_3);
	ck_assert_ptr_ne(variable, NULL);
	ck_assert_int_eq(memcmp(variable->identifier, VARIABLE_SYMBOL_ID_3, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(variable->data_type, BX_INT);
	ck_assert_int_eq(variable->symbol_type, BX_COMP_VARIABLE_SYMBOL);
	ck_assert_int_eq(variable->symbol_data.variable_number, 3);

	child_scope_var1 = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_1);
	error = bx_cgsy_scope_up(symbol_table);
	ck_assert_int_eq(error, 0);
	variable = bx_cgsy_get_variable(symbol_table, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(variable, NULL);
	ck_assert_int_ne(variable->symbol_data.variable_number, child_scope_var1->symbol_data.variable_number);
} END_TEST


START_TEST (symbol_table_destroy) {
	bx_int8 error;

	error = bx_cgsy_destroy_symbol_table(symbol_table);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_symbol_table_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_field_symbols");
	tcase_add_test(tcase, add_field_symbols);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_duplicate_field");
	tcase_add_test(tcase, reject_duplicate_field);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("main_scope_test");
	tcase_add_test(tcase, main_scope_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_field_variable_duplicate");
	tcase_add_test(tcase, reject_field_variable_duplicate);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("variable_scopes");
	tcase_add_test(tcase, variable_scopes);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("symbol_table_destroy");
	tcase_add_test(tcase, symbol_table_destroy);
	suite_add_tcase(suite, tcase);

	return suite;
}

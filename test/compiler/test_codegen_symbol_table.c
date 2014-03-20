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

#define FIELD_SYMBOL_ID_1 "field_symbol1"
#define FIELD_SYMBOL_ID_2 "field_symbol2"
#define FIELD_SYMBOL_ID_3 "field_symbol3"

#define VARIABLE_SYMBOL_ID_1 "variable_symbol1"
#define VARIABLE_SYMBOL_ID_2 "variable_symbol2"
#define VARIABLE_SYMBOL_ID_3 "variable_symbol3"

#define NOT_IN_TABLE "symbol4"

static struct bx_comp_variable_scope *main_scope;
static struct bx_comp_variable_scope *child_scope;

START_TEST (init_test) {
	bx_int8 error;

	error = bx_cgsy_init();
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (add_field_symbols) {
	bx_int8 error;
	struct bx_comp_field_symbol *symbol;

	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_1, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_2, BX_FLOAT, BX_COMP_NEW);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_3, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_1);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_INT);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_1, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_2);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_FLOAT);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_NEW);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_2, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_3);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_BOOL);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(FIELD_SYMBOL_ID_3, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get_field(NOT_IN_TABLE);
	ck_assert_ptr_eq(symbol, NULL);
} END_TEST

START_TEST (reject_duplicate_field) {
	bx_int8 error;

	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_1, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_2, BX_FLOAT, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add_field(FIELD_SYMBOL_ID_3, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (create_scope) {

	main_scope = bx_cgsy_create_variable_scope(NULL);
	ck_assert_ptr_ne(main_scope, NULL);
	ck_assert_ptr_eq(main_scope->parent, NULL);
	ck_assert_ptr_eq(main_scope->variable_list, NULL);
	child_scope = bx_cgsy_create_variable_scope(main_scope);
	ck_assert_ptr_ne(child_scope, NULL);
	ck_assert_ptr_eq(child_scope->parent, main_scope);
	ck_assert_ptr_eq(child_scope->variable_list, NULL);
} END_TEST

START_TEST (add_variable_symbol) {
	struct bx_comp_variable_symbol *main_scope_symbol;
	struct bx_comp_variable_symbol *child_scope_symbol;
	bx_int8 error;

	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, main_scope);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_2, BX_INT, main_scope);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, child_scope);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_3, BX_FLOAT, child_scope);
	ck_assert_int_eq(error, 0);

	main_scope_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(main_scope_symbol, NULL);
	ck_assert_int_eq(memcmp(main_scope_symbol->identifier, VARIABLE_SYMBOL_ID_1, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(main_scope_symbol->data_type, BX_BOOL);
	ck_assert_int_eq(main_scope_symbol->variable_number, 0);
	main_scope_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_2);
	ck_assert_ptr_ne(main_scope_symbol, NULL);
	ck_assert_int_eq(memcmp(main_scope_symbol->identifier, VARIABLE_SYMBOL_ID_2, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(main_scope_symbol->data_type, BX_INT);
	ck_assert_int_eq(main_scope_symbol->variable_number, 1);
	main_scope_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_3);
	ck_assert_ptr_eq(main_scope_symbol, NULL);

	child_scope_symbol = bx_cgsy_get_variable(child_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(child_scope_symbol, NULL);
	ck_assert_int_eq(memcmp(child_scope_symbol->identifier, VARIABLE_SYMBOL_ID_1, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(child_scope_symbol->data_type, BX_BOOL);
	ck_assert_int_eq(child_scope_symbol->variable_number, 2);
	child_scope_symbol = bx_cgsy_get_variable(child_scope, VARIABLE_SYMBOL_ID_3);
	ck_assert_ptr_ne(child_scope_symbol, NULL);
	ck_assert_int_eq(memcmp(child_scope_symbol->identifier, VARIABLE_SYMBOL_ID_3, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_eq(child_scope_symbol->data_type, BX_FLOAT);
	ck_assert_int_eq(child_scope_symbol->variable_number, 3);
	child_scope_symbol = bx_cgsy_get_variable(child_scope, VARIABLE_SYMBOL_ID_2);
	ck_assert_ptr_ne(child_scope_symbol, NULL);

	main_scope_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_1);
	child_scope_symbol = bx_cgsy_get_variable(child_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_int_eq(memcmp(main_scope_symbol->identifier, child_scope_symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);
	ck_assert_int_ne(main_scope_symbol->variable_number, child_scope_symbol->variable_number);
} END_TEST

START_TEST (reject_duplicate_variable) {
	bx_int8 error;

	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, main_scope);
	ck_assert_int_ne(error, 0);

	error = bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, child_scope);
	ck_assert_int_ne(error, 0);

	error = bx_cgsy_add_variable(FIELD_SYMBOL_ID_1, BX_BOOL, child_scope);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (scope_destroy) {
	bx_int8 error;
	struct bx_comp_variable_symbol *variable_symbol;

	error = bx_cgsy_destroy_variable_scope(child_scope);
	ck_assert_int_eq(error, 0);

	variable_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_ptr_ne(variable_symbol, NULL);
	variable_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_2);
	ck_assert_ptr_ne(variable_symbol, NULL);

	error = bx_cgsy_destroy_variable_scope(main_scope);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (reset) {
	bx_int8 error;
	struct bx_comp_field_symbol *field_symbol;
	struct bx_comp_variable_symbol *variable_symbol;

	main_scope = bx_cgsy_create_variable_scope(NULL);
	bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, main_scope);
	variable_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_int_ne(variable_symbol->variable_number, 0);
	bx_cgsy_destroy_variable_scope(main_scope);

	error = bx_cgsy_reset();
	field_symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_1);
	ck_assert_ptr_eq(field_symbol, NULL);
	field_symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_2);
	ck_assert_ptr_eq(field_symbol, NULL);
	field_symbol = bx_cgsy_get_field(FIELD_SYMBOL_ID_3);
	ck_assert_ptr_eq(field_symbol, NULL);

	main_scope = bx_cgsy_create_variable_scope(NULL);
	bx_cgsy_add_variable(VARIABLE_SYMBOL_ID_1, BX_BOOL, main_scope);
	variable_symbol = bx_cgsy_get_variable(main_scope, VARIABLE_SYMBOL_ID_1);
	ck_assert_int_eq(variable_symbol->variable_number, 0);
	bx_cgsy_destroy_variable_scope(main_scope);
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

	tcase = tcase_create("create_scope");
	tcase_add_test(tcase, create_scope);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_variable_symbol");
	tcase_add_test(tcase, add_variable_symbol);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_duplicate_variable");
	tcase_add_test(tcase, reject_duplicate_variable);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("scope_destroy");
	tcase_add_test(tcase, scope_destroy);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset");
	tcase_add_test(tcase, reset);
	suite_add_tcase(suite, tcase);

	return suite;
}

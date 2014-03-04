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

#define SYMBOL_ID_1 "symbol1"
#define SYMBOL_ID_2 "symbol2"
#define SYMBOL_ID_3 "symbol3"
#define NOT_IN_TABLE "symbol4"

START_TEST (init_test) {
	bx_int8 error;

	error = bx_cgsy_init();
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (add_symbols) {
	bx_int8 error;
	struct bx_comp_symbol *symbol;

	error = bx_cgsy_add(SYMBOL_ID_1, BX_INT, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add(SYMBOL_ID_2, BX_FLOAT, BX_COMP_NEW);
	ck_assert_int_eq(error, 0);
	error = bx_cgsy_add(SYMBOL_ID_3, BX_BOOL, BX_COMP_EXISTING);
	ck_assert_int_eq(error, 0);

	symbol = bx_cgsy_get(SYMBOL_ID_1);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_INT);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(SYMBOL_ID_1, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get(SYMBOL_ID_2);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_FLOAT);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_NEW);
	ck_assert_int_eq(strncmp(SYMBOL_ID_2, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get(SYMBOL_ID_3);
	ck_assert_ptr_ne(symbol, NULL);
	ck_assert_int_eq(symbol->data_type, BX_BOOL);
	ck_assert_int_eq(symbol->creation_modifier, BX_COMP_EXISTING);
	ck_assert_int_eq(strncmp(SYMBOL_ID_3, (char *) symbol->identifier, DM_FIELD_IDENTIFIER_LENGTH), 0);

	symbol = bx_cgsy_get(NOT_IN_TABLE);
	ck_assert_ptr_eq(symbol, NULL);
} END_TEST

START_TEST (reject_duplicate) {
	bx_int8 error;

	error = bx_cgsy_add(SYMBOL_ID_1, BX_INT, BX_COMP_VARIABLE);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add(SYMBOL_ID_2, BX_FLOAT, BX_COMP_VARIABLE);
	ck_assert_int_ne(error, 0);
	error = bx_cgsy_add(SYMBOL_ID_3, BX_BOOL, BX_COMP_VARIABLE);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (reset) {
	bx_int8 error;
	struct bx_comp_symbol *symbol;

	error = bx_cgsy_reset();
	symbol = bx_cgsy_get(SYMBOL_ID_1);
	ck_assert_ptr_eq(symbol, NULL);
	symbol = bx_cgsy_get(SYMBOL_ID_2);
	ck_assert_ptr_eq(symbol, NULL);
	symbol = bx_cgsy_get(SYMBOL_ID_3);
	ck_assert_ptr_eq(symbol, NULL);
} END_TEST

Suite *test_codegen_symbol_table_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_symbols");
	tcase_add_test(tcase, add_symbols);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_duplicate");
	tcase_add_test(tcase, reject_duplicate);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset");
	tcase_add_test(tcase, reset);
	suite_add_tcase(suite, tcase);

	return suite;
}

/*
 * document_manager.c
 * Created on: Feb 28, 2014
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

#include "test_document_manager.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"

#define FIELD_ID1 "test_field_1"
#define FIELD_ID2 "test_field_2"

static struct bx_document_field test_field1;
static struct bx_test_field_data test_field_data1;

static struct bx_document_field test_field2;
static struct bx_test_field_data test_field_data2;

static bx_int32 value = 52;

START_TEST (test_field_init) {
	bx_int8 error;

	error = bx_tfield_init(&test_field1, &test_field_data1);
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&test_field2, &test_field_data2);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (document_manager_init) {
	bx_int8 error;

	error = bx_docman_init();
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (add_test_field) {
	bx_int8 error;

	error = bx_docman_add_field(&test_field1, FIELD_ID1);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&test_field2, FIELD_ID2);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (reject_duplicate_id) {
	bx_int8 error;

	error = bx_docman_add_field(&test_field1, FIELD_ID1);
	ck_assert_int_ne(error, 0);
	error = bx_docman_add_field(&test_field2, FIELD_ID2);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (set_field_value) {
	bx_int8 error;

	bx_tfield_set_int(&test_field1, 0);
	ck_assert_int_ne(bx_tfield_get_int(&test_field1), value);
	error = bx_docman_invoke_set(FIELD_ID1, &value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&test_field1), value);

	bx_tfield_set_int(&test_field2, 0);
	ck_assert_int_ne(bx_tfield_get_int(&test_field2), value);
	error = bx_docman_invoke_set(FIELD_ID2, &value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_tfield_get_int(&test_field2), value);
} END_TEST

START_TEST (get_field_value) {
	bx_int8 error;
	bx_int32 out_value;

	bx_tfield_set_int(&test_field1, value);
	ck_assert_int_eq(bx_tfield_get_int(&test_field1), value);
	error = bx_docman_invoke_get(FIELD_ID1, &out_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(out_value, bx_tfield_get_int(&test_field1));

	bx_tfield_set_int(&test_field2, value);
	ck_assert_int_eq(bx_tfield_get_int(&test_field2), value);
	error = bx_docman_invoke_get(FIELD_ID2, &out_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(out_value, bx_tfield_get_int(&test_field2));
} END_TEST

Suite *test_document_manager_create_suite() {
	Suite *suite = suite_create("document_manager");
	TCase *tcase;

	tcase = tcase_create("test_field_init");
	tcase_add_test(tcase, test_field_init);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("document_manager_init");
	tcase_add_test(tcase, document_manager_init);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_test_field");
	tcase_add_test(tcase, add_test_field);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_duplicate_id");
	tcase_add_test(tcase, reject_duplicate_id);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reject_duplicate_id");
	tcase_add_test(tcase, reject_duplicate_id);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("set_field_value");
	tcase_add_test(tcase, set_field_value);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("get_field_value");
	tcase_add_test(tcase, get_field_value);
	suite_add_tcase(suite, tcase);

	return suite;
}

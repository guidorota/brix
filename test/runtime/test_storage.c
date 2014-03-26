/*
 * test_storage.c
 * Created on: Mar 25, 2014
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

#include "test_storage.h"
#include "runtime/storage.h"

static char *test_data1 = "TEST_DATA1";
static char *test_data2 = "TESTING AGAIN WITH OTHER DATA";

static bx_file_id file_id1;
static bx_file_id file_id2;

START_TEST (init_test) {
	bx_int8 error;

	error = bx_st_init(NULL);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (test_addition) {
	bx_int8 error;
	char *data;
	bx_size data_length;

	error = bx_st_persist(test_data1, strlen(test_data1), &file_id1);
	ck_assert_int_eq(error, 0);
	error = bx_st_retrieve(file_id1, (void *) &data, &data_length);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(strlen(test_data1), data_length);
	ck_assert_int_eq(strcmp(test_data1, data), 0);

	error = bx_st_persist(test_data2, strlen(test_data2), &file_id2);
	ck_assert_int_eq(error, 0);
	error = bx_st_retrieve(file_id2, (void *) &data, &data_length);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(strlen(test_data2), data_length);
	ck_assert_int_eq(strcmp(test_data2, data), 0);
} END_TEST

START_TEST (test_removal) {
	bx_int8 error;
	char *data;
	bx_size data_length;

	error = bx_st_delete(file_id1);
	ck_assert_int_eq(error, 0);
	error = bx_st_retrieve(file_id1, (void *) &data, &data_length);
	ck_assert_int_ne(error, 0);
	error = bx_st_retrieve(file_id2, (void *) &data, &data_length);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(strlen(test_data2), data_length);
	ck_assert_int_eq(strcmp(test_data2, data), 0);

	error = bx_st_delete(file_id2);
	ck_assert_int_eq(error, 0);
	error = bx_st_retrieve(file_id1, (void *) &data, &data_length);
	ck_assert_int_ne(error, 0);
	error = bx_st_retrieve(file_id2, (void *) &data, &data_length);
	ck_assert_int_ne(error, 0);
} END_TEST

Suite *test_storage_create_suite() {
	Suite *suite = suite_create("test_storage");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_addition");
	tcase_add_test(tcase, test_addition);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_removal");
	tcase_add_test(tcase, test_removal);
	suite_add_tcase(suite, tcase);

	return suite;
}

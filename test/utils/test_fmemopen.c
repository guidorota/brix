/*
 * test_fmemopen.c
 * Created on: Mar 17, 2014
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
#include "test_fmemopen.h"
#include "utils/fmemopen.h"

static char *test_string = "test_string";

START_TEST (init_close_test) {
	int error;
	FILE *stream;

	stream = fmemopen(test_string, strlen(test_string), "r");
	ck_assert_ptr_ne(stream, NULL);
	error = fclose(stream);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (read_test) {
	FILE *stream;
	char output_string[strlen(test_string)];
	size_t bytes_read;

	stream = fmemopen(test_string, strlen(test_string), "r");
	ck_assert_ptr_ne(stream, NULL);
	bytes_read = fread(output_string, 1, strlen(test_string), stream);
	ck_assert_int_eq(bytes_read, strlen(test_string));
	ck_assert_int_eq(memcmp(output_string, test_string, strlen(test_string)), 0);
	fclose(stream);
} END_TEST

Suite *test_fmemopen_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_close_test");
	tcase_add_test(tcase, init_close_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("read_test");
	tcase_add_test(tcase, read_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

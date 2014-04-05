/*
 * test_byte_buffer.c
 * Created on: Mar 1, 2014
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

#include "types.h"
#include "test_byte_buffer.h"
#include "utils/byte_buffer.h"

#define STORAGE_CAPACITY 20

static struct bx_byte_buffer *buffer;
static bx_uint8 byte_buffer_storage[BX_BBUF_STORAGE_SIZE(STORAGE_CAPACITY)];

START_TEST (create_byte_buffer) {
	buffer = bx_bbuf_init(byte_buffer_storage, BX_BBUF_STORAGE_SIZE(STORAGE_CAPACITY));
	ck_assert_ptr_ne(buffer, NULL);
	ck_assert_int_eq(bx_bbuf_capacity(buffer), STORAGE_CAPACITY);
	ck_assert_int_eq(bx_bbuf_size(buffer), 0);
	ck_assert_ptr_eq(buffer, byte_buffer_storage);
} END_TEST

START_TEST (normal_usage_test) {
	bx_int8 error;
	bx_size previous_size;
	bx_uint32 value1 = 12;
	bx_int32 value2 = -15;
	bx_int8 value3 = -8;
	bx_uint32 out_value1 = 0;
	bx_int32 out_value2 = 0;
	bx_int8 out_value3 = 0;

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_append(buffer, (void *) &value1, sizeof value1);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size + sizeof value1);

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_append(buffer, (void *) &value2, sizeof value2);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size + sizeof value2);

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_append(buffer, (void *) &value3, sizeof value3);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size + sizeof value3);

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_get(buffer, (void *) &out_value1, sizeof out_value1);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size - sizeof value1);
	ck_assert_int_eq(out_value1, value1);

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_get(buffer, (void *) &out_value2, sizeof out_value2);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size - sizeof value2);
	ck_assert_int_eq(out_value1, value1);

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_get(buffer, (void *) &out_value3, sizeof out_value3);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size - sizeof value3);
	ck_assert_int_eq(out_value1, value1);
} END_TEST

START_TEST (reset_byte_buffer) {
	bx_int8 error;

	error = bx_bbuf_reset(buffer);
	ck_assert_int_eq(error, 0);
	ck_assert_int_gt(bx_bbuf_capacity(buffer), 0);
	ck_assert_int_eq(bx_bbuf_size(buffer), 0);
	ck_assert_ptr_eq(buffer, byte_buffer_storage);
} END_TEST

START_TEST (fill_test) {
	bx_int8 error;
	bx_uint8 value = 1;
	bx_size previous_size;

	while (bx_bbuf_capacity(buffer) - bx_bbuf_size(buffer) != 0) {
		error = bx_bbuf_append(buffer, (void *) &value, sizeof value);
		ck_assert_int_eq(error, 0);
		ck_assert_int_ne(bx_bbuf_size(buffer), 0);
	}

	previous_size = bx_bbuf_size(buffer);
	error = bx_bbuf_append(buffer, (void *) &value, sizeof value);
	ck_assert_int_eq(error, -1);
	ck_assert_int_eq(bx_bbuf_size(buffer), previous_size);
} END_TEST

START_TEST (write_and_read_around) {
	bx_int8 error;
	bx_uint8 source[STORAGE_CAPACITY - 1];
	bx_uint8 destination[STORAGE_CAPACITY - 1];

	error = bx_bbuf_reset(buffer);
	ck_assert_int_eq(error, 0);

	bx_size i;
	for (i = 0; i < sizeof source; i++) {
		source[i] = 1;
	}

	for (i = 0; i < 10; i++) {
		error = bx_bbuf_append(buffer, source, sizeof source);
		ck_assert_int_eq(error, 0);
		error = bx_bbuf_get(buffer, destination, sizeof destination);
		ck_assert_int_eq(error, 0);
		ck_assert_int_eq(memcmp((void *) source, (void *) destination, sizeof source), 0);
	}

} END_TEST

Suite *test_byte_buffer_create_suite() {
	Suite *suite = suite_create("byte_buffer");
	TCase *tcase;

	tcase = tcase_create("create_byte_buffer");
	tcase_add_test(tcase, create_byte_buffer);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("normal_usage_test");
	tcase_add_test(tcase, normal_usage_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset_byte_buffer");
	tcase_add_test(tcase, reset_byte_buffer);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("fill_test");
	tcase_add_test(tcase, fill_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("write_and_read_around");
	tcase_add_test(tcase, write_and_read_around);
	suite_add_tcase(suite, tcase);

	return suite;
}

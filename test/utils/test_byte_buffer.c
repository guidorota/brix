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

#include "test_byte_buffer.h"
#include "utils/byte_buffer.h"

#define BYTE_BUFFER_CAPACITY 10

static struct bx_byte_buffer buffer;
static bx_uint8 byte_buffer_storage[BYTE_BUFFER_CAPACITY];

START_TEST (create_byte_buffer) {
	bx_int8 error;

	error = bx_bbuf_init(&buffer, byte_buffer_storage, BYTE_BUFFER_CAPACITY);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(buffer.capacity, BYTE_BUFFER_CAPACITY);
	ck_assert_int_eq(buffer.size, 0);
	ck_assert_ptr_eq(buffer.storage, byte_buffer_storage);
} END_TEST

START_TEST (reset_byte_buffer) {

} END_TEST

Suite *test_list_create_suite() {
	Suite *suite = suite_create("test_list");
	TCase *tcase;

	tcase = tcase_create("create_byte_buffer");
	tcase_add_test(tcase, create_byte_buffer);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset_byte_buffer");
	tcase_add_test(tcase, reset_byte_buffer);
	suite_add_tcase(suite, tcase);

	return suite;
}

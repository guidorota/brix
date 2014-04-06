/*
 * test_memory_utils.c
 * Created on: Apr 6, 2014
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

#include <string.h>
#include "test_memory_utils.h"
#include "utils/memory_utils.h"

#define DATA_LENGTH 50

static bx_uint8 data_source[DATA_LENGTH];
static bx_uint8 data_destination[DATA_LENGTH];

static bx_int8 check_memory_swap(const void *mem1, const void *mem2, bx_size data_length) {
	int i;

	for (i = 0; i < data_length; i++) {
		if (*((bx_uint8 *) mem1 + i) != *((bx_uint8 *) mem2 + data_length - 1 - i)) {
			return 0;
		}
	}

	return 1;
}

START_TEST (init_test) {
	bx_uint8 i;

	for (i = 0; i < DATA_LENGTH; i++) {
		data_source[i] = i;
	}

} END_TEST

START_TEST (switch_byte_order) {
	bx_size data_length;

	for (data_length = 1; data_length < DATA_LENGTH; data_length++) {
		bx_mutils_byte_order_switch_copy(data_destination, data_source, data_length);
		ck_assert_int_eq(check_memory_swap(data_destination, data_source, data_length), 1);
	}

} END_TEST

START_TEST (switch_16) {
	bx_uint16 data;
	bx_uint16 swapped;

	data = 936;
	swapped = bx_mutils_swap16(data);
	ck_assert_int_eq(check_memory_swap(&data, &swapped, 2), 1);
} END_TEST

START_TEST (switch_32) {
	bx_uint32 data;
	bx_uint32 swapped;

	data = 1873647;
	swapped = bx_mutils_swap32(data);
	ck_assert_int_eq(check_memory_swap(&data, &swapped, 4), 1);
} END_TEST

Suite *test_memory_utils_create_suite(void) {
	Suite *suite = suite_create("stack");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("switch_byte_order");
	tcase_add_test(tcase, switch_byte_order);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("switch_16");
	tcase_add_test(tcase, switch_16);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("switch_32");
	tcase_add_test(tcase, switch_32);
	suite_add_tcase(suite, tcase);

	return suite;
}

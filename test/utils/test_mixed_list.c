/*
 * test_mixed_list.c
 * Created on: Feb 22, 2014
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
#include "test_mixed_list.h"
#include "utils/mixed_list.h"

#define LIST_STORAGE_SIZE 512

static bx_uint8 list_storage[LIST_STORAGE_SIZE];
static struct bx_mlist list;

static bx_uint8 array0[4] = { 4, 4, 4, 4 };
static bx_uint8 array1[12] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
static bx_uint8 array2[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };

START_TEST (init_list) {
	bx_int8 error;

	error = bx_mlist_init(&list, list_storage, LIST_STORAGE_SIZE);
	ck_assert_int_eq(error, 0);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_int_eq(list.storage_used, 0);
} END_TEST

START_TEST (get_from_empty_list) {
	bx_size element_size;
	bx_uint8 *element;

	element = bx_mlist_get(&list, 0, &element_size);
	ck_assert_ptr_eq(element, NULL);

	element = bx_mlist_get(&list, 1, &element_size);
	ck_assert_ptr_eq(element, NULL);

	element = bx_mlist_get(&list, 2, &element_size);
	ck_assert_ptr_eq(element, NULL);
} END_TEST

START_TEST (add_element) {
	void *result;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	result = bx_mlist_add(&list, (void *) array0, sizeof array0);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof array0 + sizeof (bx_size));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	result = bx_mlist_add(&list, (void *) array1, sizeof array1);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof array1 + sizeof (bx_size));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	result = bx_mlist_add(&list, (void *) array2, sizeof array2);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof array2 + sizeof (bx_size));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (get_element) {
	bx_size element_size;
	bx_uint8 *element;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	element = bx_mlist_get(&list, 0, &element_size);
	ck_assert_ptr_ne(element, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(element_size, sizeof array0);
	ck_assert_int_eq(memcmp((void *) element, (void *) array0, element_size), 0);

	previous_storage_used = list.storage_used;
	element = bx_mlist_get(&list, 1, &element_size);
	ck_assert_ptr_ne(element, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(element_size, sizeof array1);
	ck_assert_int_eq(memcmp((void *) element, (void *) array1, element_size), 0);

	previous_storage_used = list.storage_used;
	element = bx_mlist_get(&list, 2, &element_size);
	ck_assert_ptr_ne(element, NULL);
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(element_size, sizeof array2);
	ck_assert_int_eq(memcmp((void *) element, (void *) array2, element_size), 0);
} END_TEST

START_TEST (out_of_bound_get) {
	bx_uint8 *element;
	bx_size element_size;

	element = bx_mlist_get(&list, 4, &element_size);
	ck_assert_ptr_eq(element, NULL);
} END_TEST

START_TEST (remove_element) {
	bx_uint8 error;
	bx_size previous_storage_used;
	bx_size element_size;
	bx_uint8 *element;

	previous_storage_used = list.storage_used;
	error = bx_mlist_remove_element(&list, 1);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(list.storage_used, previous_storage_used - sizeof array1 - sizeof (bx_size));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);

	element = bx_mlist_get(&list, 1, &element_size);
	ck_assert_ptr_ne(element, NULL);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(element_size, sizeof array2);
	ck_assert_int_eq(memcmp((void *) element, (void *) array2, element_size), 0);
} END_TEST

START_TEST (out_of_bound_remove) {
	bx_int8 error;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	error = bx_mlist_remove_element(&list, 6);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(list.storage_used, previous_storage_used);
} END_TEST

START_TEST (reset_list) {
	bx_int8 error;

	error = bx_mlist_reset(&list);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(list.storage_used, 0);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

Suite *test_mixed_list_create_suite() {
	Suite *suite = suite_create("test_mixed_list");
	TCase *tcase;

	tcase = tcase_create("init_list");
	tcase_add_test(tcase, init_list);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("get_from_empty_list");
	tcase_add_test(tcase, get_from_empty_list);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_element");
	tcase_add_test(tcase, add_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("get_element");
	tcase_add_test(tcase, get_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("out_of_bound_get");
	tcase_add_test(tcase, out_of_bound_get);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("remove_element");
	tcase_add_test(tcase, remove_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("out_of_bound_remove");
	tcase_add_test(tcase, out_of_bound_remove);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset_list");
	tcase_add_test(tcase, reset_list);
	suite_add_tcase(suite, tcase);

	return suite;
}

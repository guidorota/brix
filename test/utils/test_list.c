/*
 * test_list.c
 * Created on: Feb 21, 2014
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
#include "test_list.h"
#include "utils/list.h"

#define LIST_STORAGE_SIZE 512

bx_uint8 list_storage[LIST_STORAGE_SIZE];
struct bx_list list;

bx_int32 element0 = 54;
bx_int32 element1 = 57;
bx_int32 element2 = 92;

bx_boolean equals_int32(bx_int32 *element, bx_int32 *comparison_element) {

	if (*element == *comparison_element) {
		return BX_BOOLEAN_TRUE;
	} else {
		return BX_BOOLEAN_FALSE;
	}
}

START_TEST (create_list) {
	bx_int8 error;

	error = bx_list_init(&list, list_storage, LIST_STORAGE_SIZE, sizeof (bx_int32));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_int_eq(list.storage_used, 0);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (empty_list_get) {
	bx_int8 error;
	bx_int32 *element_pointer;

	error = bx_list_get_element(&list, 0, (void **) &element_pointer);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (empty_list_copy) {
	bx_int8 error;
	bx_int32 element;

	error = bx_list_copy_element(&list, 0, &element);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (empty_list_remove) {
	bx_int8 error;

	error = bx_list_remove_element(&list, 0);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (add_element) {
	void *result;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	result = bx_list_add_element(&list, (void *) &element0);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof (bx_int32));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	result = bx_list_add_element(&list, (void *) &element1);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof (bx_int32));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	result = bx_list_add_element(&list, (void *) &element2);
	ck_assert_ptr_ne(result, NULL);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used + sizeof (bx_int32));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (get_element) {
	bx_int8 error;
	bx_size previous_storage_used;
	bx_int32 *element_pointer;

	previous_storage_used = list.storage_used;
	error = bx_list_get_element(&list, 0, (void **) &element_pointer);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(*element_pointer, element0);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	error = bx_list_get_element(&list, 1, (void **) &element_pointer);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(*element_pointer, element1);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);

	previous_storage_used = list.storage_used;
	error = bx_list_get_element(&list, 2, (void **) &element_pointer);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(*element_pointer, element2);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (out_of_bound_get) {
	bx_int8 error;
	bx_int32 *element_pointer;

	error = bx_list_get_element(&list, 7, (void **) &element_pointer);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (copy_element) {
	bx_int8 error;
	bx_size previous_storage_used;
	bx_int32 element_copy;

	previous_storage_used = list.storage_used;
	error = bx_list_copy_element(&list, 1, &element_copy);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(element_copy, element1);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (out_of_bound_copy) {
	bx_int8 error;
	bx_int32 element;

	error = bx_list_copy_element(&list, 7, &element);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (search_element) {
	bx_int8 error;
	bx_size previous_storage_used;
	bx_int32 *element_pointer;
	bx_int32 comparison_element = 54;

	previous_storage_used = list.storage_used;
	error = bx_list_search_element(&list, (void **) &element_pointer, &comparison_element, (equals_function) &equals_int32);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(*element_pointer, comparison_element);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (search_non_existing_element) {
	bx_int8 error;
	bx_int32 *element_pointer;
	bx_int32 non_existing_element = 105;

	error = bx_list_search_element(&list, (void **) &element_pointer, &non_existing_element, (equals_function) &equals_int32);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (indexof) {
	bx_int8 error;
	bx_size previous_storage_used;
	bx_size index;

	previous_storage_used = list.storage_used;
	error = bx_list_indexof(&list, &index, &element1, (equals_function) &equals_int32);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(index, 1);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
} END_TEST

START_TEST (indexof_non_existing_element) {
	bx_int8 error;
	bx_size index;
	bx_int32 non_existing_element = 105;

	error = bx_list_indexof(&list, &index, &non_existing_element, (equals_function) &equals_int32);
	ck_assert_int_ne(error, 0);
} END_TEST

START_TEST (remove_element) {
	bx_int8 error;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	error = bx_list_remove_element(&list, 2);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(list.storage_used, previous_storage_used - sizeof (bx_int32));
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
} END_TEST

START_TEST (remove_non_existing_element) {
	bx_int8 error;
	bx_size previous_storage_used;

	previous_storage_used = list.storage_used;
	error = bx_list_remove_element(&list, 5);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(list.storage_used, previous_storage_used);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
} END_TEST

START_TEST (reset_list) {
	bx_int8 error;

	error = bx_list_reset(&list);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(list.storage_used, 0);
	ck_assert_int_eq(list.storage_size, LIST_STORAGE_SIZE);
	ck_assert_ptr_eq(list.storage, list_storage);
	ck_assert_int_eq(list.element_size, sizeof (bx_int32));
} END_TEST

Suite *test_list_create_suite() {
	Suite *suite = suite_create("test_stack");
	TCase *tcase;

	tcase = tcase_create("create_list");
	tcase_add_test(tcase, create_list);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("empty_list_get");
	tcase_add_test(tcase, empty_list_get);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("empty_list_copy");
	tcase_add_test(tcase, empty_list_copy);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("empty_list_remove");
	tcase_add_test(tcase, empty_list_remove);
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

	tcase = tcase_create("copy_element");
	tcase_add_test(tcase, copy_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("out_of_bound_copy");
	tcase_add_test(tcase, out_of_bound_copy);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("search_element");
	tcase_add_test(tcase, search_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("search_non_existing_element");
	tcase_add_test(tcase, search_non_existing_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("indexof");
	tcase_add_test(tcase, indexof);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("indexof_non_existing_element");
	tcase_add_test(tcase, indexof_non_existing_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("remove_item");
	tcase_add_test(tcase, remove_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("remove_non_existing_element");
	tcase_add_test(tcase, remove_non_existing_element);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("reset_list");
	tcase_add_test(tcase, reset_list);
	suite_add_tcase(suite, tcase);

	return suite;
}

/*
 * test_bx_linked_list.c
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

#include "types.h"
#include "compiler/linked_list.h"
#include "test_linked_list.h"

struct bx_linked_list *linked_list = NULL;

static int element1 = 1;
static int element2 = 2;
static int element3 = 1;
static int not_in_list1 = 1;
static int not_in_list10 = 10;

bx_int8 integer_equals(int *element1, int *element2) {
	return *element1 == *element2;
}

START_TEST (bx_linked_list_create) {
	bx_llist_add(&linked_list, &element1);
	bx_llist_add(&linked_list, &element2);
	bx_llist_add(&linked_list, &element3);

	ck_assert_int_eq(bx_llist_size(linked_list), 3);
} END_TEST

START_TEST (bx_linked_list_contains) {
	ck_assert_int_eq(bx_llist_contains(linked_list, &element3), 1);
	ck_assert_int_eq(bx_llist_contains(linked_list, &not_in_list1), 0);
	ck_assert_int_eq(bx_llist_contains(linked_list, &not_in_list10), 0);
} END_TEST

START_TEST (bx_linked_list_contains_equals) {
	ck_assert_int_eq(bx_llist_contains_equals(linked_list, &element3, (bx_llist_equals) integer_equals), 1);
	ck_assert_int_eq(bx_llist_contains_equals(linked_list, &not_in_list1, (bx_llist_equals) integer_equals), 1);
	ck_assert_int_eq(bx_llist_contains_equals(linked_list, &not_in_list10, (bx_llist_equals) integer_equals), 0);
} END_TEST

START_TEST (bx_linked_list_remove) {
	ck_assert_int_eq(bx_llist_contains(linked_list, &element3), 1);
	bx_llist_remove(&linked_list, &element3);
	ck_assert_int_eq(bx_llist_contains(linked_list, &element3), 0);
} END_TEST

START_TEST (bx_linked_list_remove_equals) {
	ck_assert_int_eq(bx_llist_contains_equals(linked_list, &element2, (bx_llist_equals) integer_equals), 1);
	bx_llist_remove_equals(&linked_list, &element2, (bx_llist_equals) integer_equals);
	ck_assert_int_eq(bx_llist_contains_equals(linked_list, &element2, (bx_llist_equals) integer_equals), 0);
} END_TEST

Suite *test_linked_list_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("bx_linked_list_create");
	tcase_add_test(tcase, bx_linked_list_create);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bx_linked_list_contains");
	tcase_add_test(tcase, bx_linked_list_contains);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bx_linked_list_contains_equals");
	tcase_add_test(tcase, bx_linked_list_contains_equals);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bx_linked_list_remove");
	tcase_add_test(tcase, bx_linked_list_remove);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bx_linked_list_remove_equals");
	tcase_add_test(tcase, bx_linked_list_remove_equals);
	suite_add_tcase(suite, tcase);

	return suite;
}

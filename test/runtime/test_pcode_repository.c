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

#include <string.h>
#include "test_pcode_repository.h"
#include "runtime/pcode_repository.h"

#define DATA1 "Test data 1"
#define DATA2 "Test data number two"
#define DATA3 "This is data three test data"

#define DATA1_SIZE strlen(DATA1)
#define DATA2_SIZE strlen(DATA2)
#define DATA3_SIZE strlen(DATA3)

struct test_bx_pcode *pcode1;
struct test_bx_pcode *pcode2;
struct test_bx_pcode *pcode3;

struct test_bx_pcode {
	bx_boolean valid;
	void *instructions;
	bx_size size;
};

START_TEST (init_test) {
	bx_int8 error;

	error = bx_pr_init();
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (add_test) {
	pcode1 = (struct test_bx_pcode *) bx_pr_add((void *) DATA1, DATA1_SIZE);
	ck_assert_ptr_ne(pcode1, NULL);
	pcode2 = (struct test_bx_pcode *) bx_pr_add((void *) DATA2, DATA2_SIZE);
	ck_assert_ptr_ne(pcode2, NULL);

	ck_assert_int_eq(pcode1->valid, BX_BOOLEAN_TRUE);
	ck_assert_int_eq(pcode1->size, DATA1_SIZE);
	ck_assert_int_eq(memcmp(pcode1->instructions, DATA1, DATA1_SIZE), 0);

	ck_assert_int_eq(pcode2->valid, BX_BOOLEAN_TRUE);
	ck_assert_int_eq(pcode2->size, DATA2_SIZE);
	ck_assert_int_eq(memcmp(pcode2->instructions, DATA2, DATA2_SIZE), 0);
} END_TEST

START_TEST (remove_test) {
	bx_int8 error;
	void *data2_pcode_pointer;

	data2_pcode_pointer = pcode2->instructions;
	error = bx_pr_remove((struct bx_pcode *) pcode1);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(pcode1->valid, BX_BOOLEAN_FALSE);
	ck_assert_int_eq(pcode1->size, 0);
	ck_assert_ptr_eq(pcode1->instructions, NULL);
	ck_assert_ptr_ne(data2_pcode_pointer, pcode2->instructions);
	ck_assert_int_eq(pcode2->valid, BX_BOOLEAN_TRUE);
	ck_assert_int_eq(pcode2->size, DATA2_SIZE);
	ck_assert_int_eq(memcmp(pcode2->instructions, DATA2, DATA2_SIZE), 0);

	pcode3 = (struct test_bx_pcode *) bx_pr_add((void *) DATA3, DATA3_SIZE);
	ck_assert_ptr_ne(pcode2, NULL);
	ck_assert_int_eq(pcode3->valid, BX_BOOLEAN_TRUE);
	ck_assert_int_eq(pcode3->size, DATA3_SIZE);
	ck_assert_int_eq(memcmp(pcode3->instructions, DATA3, DATA3_SIZE), 0);
	ck_assert_ptr_eq(pcode1, pcode3);

	ck_assert_int_eq(pcode2->valid, BX_BOOLEAN_TRUE);
	ck_assert_int_eq(pcode2->size, DATA2_SIZE);
	ck_assert_int_eq(memcmp(pcode2->instructions, DATA2, DATA2_SIZE), 0);
	ck_assert_ptr_eq(pcode3->instructions, (void *) ((bx_uint8 *) pcode2->instructions + pcode2->size));
} END_TEST

Suite *test_pcode_repository_create_suite() {
	Suite *suite = suite_create("storage");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_test");
	tcase_add_test(tcase, add_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("remove_test");
	tcase_add_test(tcase, remove_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

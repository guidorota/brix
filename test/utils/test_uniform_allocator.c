/*
 * test_uniform_allocator.c
 * Created on: Mar 31, 2014
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

#include "test_uniform_allocator.h"
#include "utils/uniform_allocator.h"

#define STORAGE_SIZE 2048
#define CHUNK_SIZE 128

static bx_uint8 storage[STORAGE_SIZE];
static struct bx_ualloc *ualloc;

START_TEST (init_test) {
	ualloc = bx_ualloc_init((void *) storage, STORAGE_SIZE, CHUNK_SIZE);
	ck_assert_ptr_ne(ualloc, NULL);
	ck_assert_int_gt(bx_ualloc_remaining_capacity(ualloc), 0);
	ck_assert_int_eq(bx_ualloc_chunk_size(ualloc), CHUNK_SIZE);
} END_TEST

START_TEST (allocation_deallocation_test) {
	bx_uint8 error;
	bx_size capacity;
	void *chunk_pointer;

	capacity = bx_ualloc_remaining_capacity(ualloc);
	chunk_pointer = bx_ualloc_alloc(ualloc);
	ck_assert_ptr_ne(chunk_pointer, NULL);
	ck_assert_int_eq(bx_ualloc_remaining_capacity(ualloc), capacity - 1);
	error = bx_ualloc_free(ualloc, chunk_pointer);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_ualloc_remaining_capacity(ualloc), capacity);
} END_TEST

START_TEST (bulk_allocation_test) {
	bx_uint8 error;
	void *pointer_array[bx_ualloc_remaining_capacity(ualloc)];
	bx_size capacity;
	bx_size i;

	capacity = bx_ualloc_remaining_capacity(ualloc);
	for (i = 0; i < capacity; i++) {
		pointer_array[i] = bx_ualloc_alloc(ualloc);
		ck_assert_ptr_ne(pointer_array[i], NULL);
	}

	for (i = 0; i < capacity; i++) {
		error = bx_ualloc_free(ualloc, pointer_array[i]);
		ck_assert_int_eq(error, 0);
	}
} END_TEST

Suite *test_uniform_allocator_create_suite(void) {
	Suite *suite = suite_create("uniform_allocator");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("allocation_deallocation_test");
	tcase_add_test(tcase, allocation_deallocation_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("bulk_allocation_test");
	tcase_add_test(tcase, bulk_allocation_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

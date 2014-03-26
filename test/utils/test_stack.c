/*
 * test_stack.c
 * Created on: Feb 16, 2014
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

#include "test_stack.h"
#include "types.h"
#include "utils/stack.h"
#include <stdio.h>

#define STACK_SIZE 4

static struct bx_stack stack;
static bx_int8 byte_array[STACK_SIZE];

static bx_int8 byte_var = 12;
static bx_int32 int_value = 987364758;

START_TEST (create_stack) {
	bx_int8 error;

	error = bx_stack_init(&stack, (void *) byte_array, STACK_SIZE);

	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), 0);
	ck_assert_ptr_ne(stack.stack, NULL);
	ck_assert_int_eq(bx_stack_capacity(&stack), STACK_SIZE);
} END_TEST

START_TEST (empty_stack_pop) {
	bx_int8 error;
	bx_int8 popped_byte;
	bx_size previous_size = bx_stack_size(&stack);
	bx_int8 stack_copy[STACK_SIZE];
	memcpy((void *) stack_copy, stack.stack, STACK_SIZE);

	error = bx_stack_pop(&stack, &popped_byte, sizeof popped_byte);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size);
	ck_assert_int_eq(memcmp((void *) stack_copy, stack.stack, STACK_SIZE), 0);
} END_TEST

START_TEST (push_pop_byte) {
	bx_int8 error;
	bx_size previous_size;
	bx_int8 popped_byte;

	previous_size = bx_stack_size(&stack);
	error = bx_stack_push(&stack, &byte_var, sizeof byte_var);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size + sizeof byte_var);

	previous_size = bx_stack_size(&stack);
	error = bx_stack_pop(&stack, &popped_byte, sizeof popped_byte);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size - sizeof byte_var);
	ck_assert_int_eq(popped_byte, byte_var);
} END_TEST

START_TEST (push_pop_4_bytes) {
	bx_int8 error;
	bx_size previous_size;
	bx_int32 popped_int;

	previous_size = bx_stack_size(&stack);
	error = bx_stack_push(&stack, &int_value, sizeof int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size + sizeof int_value);

	previous_size = bx_stack_size(&stack);
	error = bx_stack_pop(&stack, &popped_int, sizeof popped_int);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size - sizeof int_value);
	ck_assert_int_eq(int_value, popped_int);
} END_TEST

START_TEST (push_pop_macro) {
	bx_int8 error;
	bx_size previous_size;
	bx_int32 popped_int;

	previous_size = bx_stack_size(&stack);
	error = BX_STACK_PUSH_VARIABLE(&stack, int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size + sizeof int_value);

	previous_size = bx_stack_size(&stack);
	error = BX_STACK_POP_VARIABLE(&stack, popped_int);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size - sizeof int_value);
	ck_assert_int_eq(int_value, popped_int);
} END_TEST

START_TEST (full_stack_push) {
	bx_int8 error;
	bx_size previous_size;
	bx_int8 stack_copy[STACK_SIZE];

	previous_size = bx_stack_size(&stack);
	error = bx_stack_push(&stack, &int_value, sizeof int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size + sizeof int_value);
	ck_assert_int_eq(bx_stack_capacity(&stack), STACK_SIZE);

	memcpy((void *) stack_copy, stack.stack, STACK_SIZE);
	previous_size = bx_stack_size(&stack);
	error = bx_stack_push(&stack, &int_value, sizeof int_value);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(memcmp((void *) &stack_copy, stack.stack, STACK_SIZE), 0);
	ck_assert_int_eq(bx_stack_size(&stack), previous_size);
} END_TEST

Suite *test_stack_create_suite(void) {
	Suite *suite = suite_create("stack");
	TCase *tcase;

	tcase = tcase_create("create_stack");
	tcase_add_test(tcase, create_stack);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("empty_stack_pop");
	tcase_add_test(tcase, empty_stack_pop);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("push_pop_byte");
	tcase_add_test(tcase, push_pop_byte);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("push_pop_4_bytes");
	tcase_add_test(tcase, push_pop_4_bytes);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("push_pop_macro");
	tcase_add_test(tcase, push_pop_macro);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("full_stack_push");
	tcase_add_test(tcase, full_stack_push);
	suite_add_tcase(suite, tcase);

	return suite;
}

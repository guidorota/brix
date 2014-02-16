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
#include "core/stack.h"
#include <stdio.h>

#define STACK_SIZE 4

struct bx_stack test_stack;
uint8_t byte_array[STACK_SIZE];

uint8_t byte_var = 12;
uint32_t int_value = 987364758;

START_TEST (create_stack) {
	int error;

	error = bx_stack_setup(&test_stack, (void *) byte_array, STACK_SIZE);

	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, 0);
	ck_assert_ptr_ne(test_stack.stack, NULL);
	ck_assert_int_eq(test_stack.size, STACK_SIZE);
} END_TEST

START_TEST (empty_stack_pop) {
	int error;
	uint8_t popped_byte;
	size_t prior_stack_top = test_stack.top;
	uint8_t stack_copy[STACK_SIZE];
	memcpy((void *) stack_copy, test_stack.stack, STACK_SIZE);

	error = bx_stack_pop(&test_stack, &popped_byte, sizeof popped_byte);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top);
	ck_assert_int_eq(memcmp((void *) stack_copy, test_stack.stack, STACK_SIZE), 0);
} END_TEST

START_TEST (push_pop_byte) {
	int error;
	size_t prior_stack_top;
	uint8_t popped_byte;

	prior_stack_top = test_stack.top;
	error = bx_stack_push(&test_stack, &byte_var, sizeof byte_var);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top + sizeof byte_var);

	prior_stack_top = test_stack.top;
	error = bx_stack_pop(&test_stack, &popped_byte, sizeof popped_byte);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top - sizeof byte_var);
	ck_assert_int_eq(popped_byte, byte_var);
} END_TEST

START_TEST (push_pop_4_bytes) {
	int error;
	size_t prior_stack_top;
	uint32_t popped_int;

	prior_stack_top = test_stack.top;
	error = bx_stack_push(&test_stack, &int_value, sizeof int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top + sizeof int_value);

	prior_stack_top = test_stack.top;
	error = bx_stack_pop(&test_stack, &popped_int, sizeof popped_int);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top - sizeof int_value);
	ck_assert_int_eq(int_value, popped_int);
} END_TEST

START_TEST (push_pop_macro) {
	int error;
	size_t prior_stack_top;
	uint32_t popped_int;

	prior_stack_top = test_stack.top;
	error = STACK_PUSH_VARIABLE(&test_stack, int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top + sizeof int_value);

	prior_stack_top = test_stack.top;
	error = STACK_POP_VARIABLE(&test_stack, popped_int);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top - sizeof int_value);
	ck_assert_int_eq(int_value, popped_int);
} END_TEST

START_TEST (full_stack_push) {
	int error;
	size_t prior_stack_top;
	uint8_t stack_copy[STACK_SIZE];

	prior_stack_top = test_stack.top;
	error = bx_stack_push(&test_stack, &int_value, sizeof int_value);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top + sizeof int_value);
	ck_assert_int_eq(test_stack.size, STACK_SIZE);

	memcpy((void *) stack_copy, test_stack.stack, STACK_SIZE);
	prior_stack_top = test_stack.top;
	error = bx_stack_push(&test_stack, &int_value, sizeof int_value);
	ck_assert_int_ne(error, 0);
	ck_assert_int_eq(memcmp((void *) &stack_copy, test_stack.stack, STACK_SIZE), 0);
	ck_assert_int_eq(test_stack.top, prior_stack_top);
} END_TEST

Suite *test_stack_create_suite(void) {
	Suite *suite = suite_create("test_stack");
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

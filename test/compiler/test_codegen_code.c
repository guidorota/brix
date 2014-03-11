/*
 * test_codegen_code.c
 * Created on: Mar 6, 2014
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
#include "test_codegen_code.h"
#include "compiler/codegen_code.h"

START_TEST (test_create_destroy) {
	struct bx_comp_code *code;

	code = bx_cgco_create();
	ck_assert_ptr_ne(code, NULL);
	ck_assert_ptr_ne(code->data, NULL);
	ck_assert_int_eq(code->size, 0);
	ck_assert_int_gt(code->capacity, 0);
	bx_cgco_destroy(code);
} END_TEST

START_TEST (data_addition) {
	bx_int8 error;
	struct bx_comp_code *code;

	code = bx_cgco_create();
	ck_assert_ptr_ne(code, NULL);

	bx_int32 i;
	for (i = 0; i < 10; i++) {
		error = bx_cgco_add_int_constant(code, i);
		ck_assert_int_ne(error, -1);
	}

	for (i = 0; i < 10; i++) {
		ck_assert_int_eq(*((bx_int32 *) code->data + i), i);
	}
} END_TEST

START_TEST (expand_size) {
	struct bx_comp_code *code;
	bx_int32 data = 12;
	bx_size initial_capacity;

	code = bx_cgco_create();
	ck_assert_ptr_ne(code, NULL);
	initial_capacity = code->capacity;

	bx_size i;
	for (i = 0; i < initial_capacity; i++) {
		bx_cgco_add_int_constant(code, data);
	}

	ck_assert_int_gt(code->capacity, initial_capacity);
	ck_assert_int_ge(code->size, initial_capacity);
	bx_cgco_destroy(code);
} END_TEST

START_TEST (copy_test) {
	struct bx_comp_code *code;
	struct bx_comp_code *copy;
	bx_int32 data = 12;
	bx_size initial_capacity;

	code = bx_cgco_create();
	ck_assert_ptr_ne(code, NULL);
	initial_capacity = code->capacity;

	bx_size i;
	for (i = 0; i < initial_capacity; i++) {
		bx_cgco_add_int_constant(code, data);
	}

	ck_assert_int_gt(code->capacity, initial_capacity);
	ck_assert_int_ge(code->size, initial_capacity);

	copy = bx_cgco_copy(code);
	ck_assert_ptr_ne(code, copy);
	ck_assert_ptr_ne(code->data, copy->data);
	ck_assert_int_eq(code->capacity, copy->capacity);
	ck_assert_int_eq(code->size, copy->size);
	ck_assert_int_eq(memcmp(code->data, copy->data, copy->capacity), 0);

	bx_cgco_destroy(code);
} END_TEST

START_TEST (address_label) {
	struct bx_comp_code *code;
	bx_comp_label false_label, true_label;
	bx_ssize false_jump_address, true_jump_address;

	code = bx_cgco_create();
	ck_assert_ptr_ne(code, NULL);

	bx_cgco_add_instruction(code, BX_INSTR_JEQZ);
	false_label = bx_cgco_create_address_label(code);
	bx_cgco_add_instruction(code, BX_INSTR_IPUSH_1);
	bx_cgco_add_instruction(code, BX_INSTR_JUMP);
	true_label = bx_cgco_create_address_label(code);
	false_jump_address = bx_cgco_add_instruction(code, BX_INSTR_IPUSH_0);
	true_jump_address = bx_cgco_add_instruction(code, BX_INSTR_NOP);
	bx_cgco_set_address_label(code, false_label, false_jump_address);
	bx_cgco_set_address_label(code, true_label, true_jump_address);

	ck_assert_int_eq(*(bx_uint16 *) ((bx_uint8 *) code->data + false_label), (bx_uint16) false_jump_address);
	ck_assert_int_eq(*(bx_uint16 *) ((bx_uint8 *) code->data + true_label), (bx_uint16) true_jump_address);
	bx_cgco_destroy(code);
} END_TEST

Suite *test_codegen_code_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("test_create_destroy");
	tcase_add_test(tcase, test_create_destroy);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("data_addition");
	tcase_add_test(tcase, data_addition);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("expand_size");
	tcase_add_test(tcase, expand_size);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("copy_test");
	tcase_add_test(tcase, copy_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("address_label");
	tcase_add_test(tcase, address_label);
	suite_add_tcase(suite, tcase);

	return suite;
}

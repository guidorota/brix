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
#include "test_codegen_pcode.h"
#include "compiler/codegen_pcode.h"
#include "utils/memory_utils.h"

START_TEST (test_create_destroy) {
	struct bx_comp_pcode *pcode;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);
	ck_assert_ptr_ne(pcode->data, NULL);
	ck_assert_int_eq(pcode->size, 0);
	ck_assert_int_gt(pcode->capacity, 0);
	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (add_integer_constant) {
	bx_int8 error;
	struct bx_comp_pcode *pcode;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);

	bx_int32 i;
	for (i = 0; i < 10; i++) {
		error = bx_cgpc_add_int_constant(pcode, i);
		ck_assert_int_ne(error, -1);
	}

	bx_int32 host_byte_order_int;
	for (i = 0; i < 10; i++) {
		host_byte_order_int = BX_MUTILS_BTH32(*((bx_int32 *) pcode->data + i));
		ck_assert_int_eq(host_byte_order_int, i);
	}

	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (add_float_constant) {
	bx_int8 error;
	struct bx_comp_pcode *pcode;
	bx_float32 float_value;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);

	bx_int32 i;
	float_value = 0;
	for (i = 0; i < 10; i += 1) {
		error = bx_cgpc_add_float_constant(pcode, float_value);
		ck_assert_int_ne(error, -1);
		float_value += 1;
	}

	bx_float32 host_byte_order_float;
	float_value = 0;
	for (i = 0; i < 10; i++) {
		BX_MUTILS_BTH_COPY(&host_byte_order_float, ((bx_float32 *) pcode->data + i), 4);
		ck_assert_int_eq(host_byte_order_float, float_value);
		float_value += 1;
	}

	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (add_boolean_constant) {
	bx_int8 error;
	bx_int32 host_byte_order_bool_value;
	struct bx_comp_pcode *pcode;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);

	error = bx_cgpc_add_bool_constant(pcode, BX_BOOLEAN_FALSE);
	ck_assert_int_ne(error, -1);
	host_byte_order_bool_value = BX_MUTILS_BTH32(*((bx_int32 *) pcode->data));
	ck_assert_int_eq(host_byte_order_bool_value, BX_BOOLEAN_FALSE);

	bx_cgpc_destroy(pcode);

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);

	error = bx_cgpc_add_bool_constant(pcode, BX_BOOLEAN_TRUE);
	ck_assert_int_ne(error, -1);
	host_byte_order_bool_value = BX_MUTILS_BTH32(*((bx_int32 *) pcode->data));
	ck_assert_int_eq(host_byte_order_bool_value, BX_BOOLEAN_TRUE);

	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (expand_size) {
	struct bx_comp_pcode *pcode;
	bx_int32 data = 12;
	bx_size initial_capacity;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);
	initial_capacity = pcode->capacity;

	bx_size i;
	for (i = 0; i < initial_capacity; i++) {
		bx_cgpc_add_int_constant(pcode, data);
	}

	ck_assert_int_gt(pcode->capacity, initial_capacity);
	ck_assert_int_ge(pcode->size, initial_capacity);
	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (append_test) {
	struct bx_comp_pcode *source;
	struct bx_comp_pcode *destination;
	bx_size capacity;
	bx_int8 error;
	bx_int32 i;
	bx_int32 host_byte_order_value;

	source = bx_cgpc_create();
	destination = bx_cgpc_create();

	for (i = 0; i < 10; i++) {
		error = bx_cgpc_add_int_constant(source, 5);
		ck_assert_int_ne(error, -1);
	}
	for (i = 0; i < 10; i++) {
		error = bx_cgpc_add_int_constant(destination, 3);
		ck_assert_int_ne(error, -1);
	}

	capacity = destination->size;
	bx_cgpc_append_pcode(destination, source);
	ck_assert_int_eq(destination->size, source->size + capacity);

	bx_int32 *value = (bx_int32 *) destination->data;
	for (i = 0; i < 10; i++) {
		host_byte_order_value = BX_MUTILS_BTH32(*(value + i));
		ck_assert_int_eq(host_byte_order_value, 3);
		host_byte_order_value = BX_MUTILS_BTH32(*(value + i + 10));
		ck_assert_int_eq(host_byte_order_value, 5);
	}

	bx_cgpc_destroy(source);
	bx_cgpc_destroy(destination);
} END_TEST

START_TEST (replace_test) {
	struct bx_comp_pcode *source;
	struct bx_comp_pcode *destination;
	bx_size capacity;
	bx_int8 error;
	bx_int32 i;
	bx_int32 host_byte_order_value;

	source = bx_cgpc_create();
	destination = bx_cgpc_create();

	for (i = 0; i < 10; i++) {
		error = bx_cgpc_add_int_constant(source, 5);
		ck_assert_int_ne(error, -1);
	}
	for (i = 0; i < 10; i++) {
		error = bx_cgpc_add_int_constant(destination, 3);
		ck_assert_int_ne(error, -1);
	}

	capacity = destination->size;
	bx_cgpc_replace_pcode(destination, source);
	ck_assert_int_eq(destination->size, source->size);

	bx_int32 *value = (bx_int32 *) destination->data;
	for (i = 0; i < 10; i++) {
		host_byte_order_value = BX_MUTILS_BTH32(*(value + i));
		ck_assert_int_eq(host_byte_order_value, 5);
	}

	bx_cgpc_destroy(source);
	bx_cgpc_destroy(destination);
} END_TEST

START_TEST (copy_test) {
	struct bx_comp_pcode *pcode;
	struct bx_comp_pcode *copy;
	bx_int32 data = 12;
	bx_size initial_capacity;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);
	initial_capacity = pcode->capacity;

	bx_size i;
	for (i = 0; i < initial_capacity; i++) {
		bx_cgpc_add_int_constant(pcode, data);
	}

	ck_assert_int_gt(pcode->capacity, initial_capacity);
	ck_assert_int_ge(pcode->size, initial_capacity);

	copy = bx_cgpc_copy(pcode);
	ck_assert_ptr_ne(pcode, copy);
	ck_assert_ptr_ne(pcode->data, copy->data);
	ck_assert_int_eq(pcode->capacity, copy->capacity);
	ck_assert_int_eq(pcode->size, copy->size);
	ck_assert_int_eq(memcmp(pcode->data, copy->data, copy->capacity), 0);

	bx_cgpc_destroy(pcode);
} END_TEST

START_TEST (address_label) {
	struct bx_comp_pcode *pcode;
	bx_comp_label false_label, true_label;
	bx_ssize false_jump_address, true_jump_address;
	bx_uint16 host_byte_order_address;

	pcode = bx_cgpc_create();
	ck_assert_ptr_ne(pcode, NULL);

	bx_cgpc_add_instruction(pcode, BX_INSTR_JEQZ);
	false_label = bx_cgpc_create_address_label(pcode);
	bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_1);
	bx_cgpc_add_instruction(pcode, BX_INSTR_JUMP);
	true_label = bx_cgpc_create_address_label(pcode);
	false_jump_address = bx_cgpc_add_instruction(pcode, BX_INSTR_IPUSH_0);
	true_jump_address = bx_cgpc_add_instruction(pcode, BX_INSTR_NOP);
	bx_cgpc_set_address_label(pcode, false_label, false_jump_address);
	bx_cgpc_set_address_label(pcode, true_label, true_jump_address);

	host_byte_order_address = BX_MUTILS_BTH16(*(bx_uint16 *) ((bx_uint8 *) pcode->data + false_label));
	ck_assert_int_eq(host_byte_order_address, (bx_uint16) false_jump_address);
	host_byte_order_address = BX_MUTILS_BTH16(*(bx_uint16 *) ((bx_uint8 *) pcode->data + true_label));
	ck_assert_int_eq(host_byte_order_address, (bx_uint16) true_jump_address);
	bx_cgpc_destroy(pcode);
} END_TEST

Suite *test_codegen_pcode_create_suite(void) {
	Suite *suite = suite_create("codegen_pcode");
	TCase *tcase;

	tcase = tcase_create("test_create_destroy");
	tcase_add_test(tcase, test_create_destroy);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_integer_constant");
	tcase_add_test(tcase, add_integer_constant);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_float_constant");
	tcase_add_test(tcase, add_float_constant);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("add_boolean_constant");
	tcase_add_test(tcase, add_boolean_constant);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("expand_size");
	tcase_add_test(tcase, expand_size);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("append_test");
	tcase_add_test(tcase, append_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("replace_test");
	tcase_add_test(tcase, replace_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("copy_test");
	tcase_add_test(tcase, copy_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("address_label");
	tcase_add_test(tcase, address_label);
	suite_add_tcase(suite, tcase);

	return suite;
}

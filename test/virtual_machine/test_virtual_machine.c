/*
 * test_instruction_set.c
 * Created on: Feb 19, 2014
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

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "test_virtual_machine.h"
#include "utils/byte_buffer.h"
#include "virtual_machine/virtual_machine.h"
#include "virtual_machine/vm_utils.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"

#define CODE_BUFFER_LENGTH 128
#define TEST_FIELD_ID "test_field"

static struct bx_document_field test_field;

static struct bx_byte_buffer buffer;
static bx_uint8 buffer_storage[CODE_BUFFER_LENGTH];

START_TEST (test_init) {
	bx_int8 error;

	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);
	error = bx_dm_document_manager_init();
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&test_field);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&test_field, TEST_FIELD_ID);
	ck_assert_int_eq(error, 0);
	bx_bbuf_init(&buffer, buffer_storage, CODE_BUFFER_LENGTH);
} END_TEST

START_TEST (store32_test) {
	bx_int8 error;
	bx_int32 test_data = 12;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, test_data);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), test_data);
} END_TEST

START_TEST (integer_arithmetics_test) {
	bx_int8 error;
	bx_int32 operand1 = 12;
	bx_int32 operand2 = 24;

	// ADDITION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), operand1 + operand2);

	// SUBTRACTION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ISUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), operand1 - operand2);

	// MULTIPLICATION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), operand1 * operand2);

	// DIVISION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IDIV);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), operand1 / operand2);

	// MODULO
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IMOD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(), operand1 % operand2);
} END_TEST

START_TEST (float_arithmetics_test) {
	bx_int8 error;
	bx_float32 operand1 = 26.987;
	bx_float32 operand2 = 0.264;

	// ADDITION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(), operand1 + operand2);

	// SUBTRACTION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FSUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(), operand1 - operand2);

	// MULTIPLICATION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(), operand1 * operand2);

	// DIVISION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FDIV);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(), operand1 / operand2);
} END_TEST

Suite *test_virtual_machine_create_suite() {
	Suite *suite = suite_create("test_virtual_machine");
	TCase *tcase;

	tcase = tcase_create("test_init");
	tcase_add_test(tcase, test_init);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("store32_test");
	tcase_add_test(tcase, store32_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("integer_arithmetics_test");
	tcase_add_test(tcase, integer_arithmetics_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("float_arithmetics_test");
	tcase_add_test(tcase, float_arithmetics_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

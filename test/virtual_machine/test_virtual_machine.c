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
static struct bx_test_field_data test_field_data;

static struct bx_byte_buffer buffer;
static bx_uint8 buffer_storage[CODE_BUFFER_LENGTH];

START_TEST (test_init) {
	bx_int8 error;

	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);
	error = bx_dm_document_manager_init();
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&test_field, &test_field_data);
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
	ck_assert_int_eq(bx_test_field_get_int(&test_field), test_data);
} END_TEST

START_TEST (load32_test) {
	bx_int8 error;
	bx_int32 operand1 = 87;
	bx_int32 operand2 = 69;

	bx_test_field_set_int(&test_field, operand2);
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_LOAD32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 + operand2);
} END_TEST

START_TEST (test_ipush_0) {
	bx_int8 error;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IPUSH_0);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), 0);
} END_TEST

START_TEST (test_ipush_1) {
	bx_int8 error;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IPUSH_1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), 1);
} END_TEST

START_TEST (test_fpush_0) {
	bx_int8 error;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FPUSH_0);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), 0.0);
} END_TEST

START_TEST (test_fpush_1) {
	bx_int8 error;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FPUSH_1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), 1.0);
} END_TEST

START_TEST (integer_arithmetics_test) {
	bx_int8 error;
	bx_int32 operand1 = 12;
	bx_int32 operand2 = 24;

	// ADDITION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 + operand2);

	// SUBTRACTION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ISUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 - operand2);

	// MULTIPLICATION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 * operand2);

	// DIVISION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IDIV);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 / operand2);

	// MODULO
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IMOD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 % operand2);
} END_TEST

START_TEST (integer_bitwise_test) {
	bx_int8 error;
	bx_int32 operand1 = 12;
	bx_int32 operand2 = 24;

	// AND
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IAND);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 & operand2);

	// OR
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IOR);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 | operand2);

	// XOR
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IXOR);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), operand1 ^ operand2);

	// NOT
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_INOT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), ~operand1);
} END_TEST

START_TEST (integer_comparison_test) {
	bx_int8 error;
	bx_int32 value1 = 28;
	bx_int32 value2 = 89;

	// EQUALITY
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IEQ);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IEQ);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);

	// INEQUALITY
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_INE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_INE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);

	// GREATER THAN
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IGT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IGT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);

	// GREATER OR EQUAL
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);

	// LESS THAN
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ILT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ILT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);

	// GREATER OR EQUAL
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ILE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ILE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ILE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), BX_BOOLEAN_FALSE);
} END_TEST

START_TEST (float_arithmetics_test) {
	bx_int8 error;
	bx_float32 operand1 = 26.987;
	bx_float32 operand2 = 0.264;

	// ADDITION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), operand1 + operand2);

	// SUBTRACTION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FSUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), operand1 - operand2);

	// MULTIPLICATION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), operand1 * operand2);

	// DIVISION
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FDIV);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), operand1 / operand2);
} END_TEST

START_TEST (float_comparison_test) {
	bx_int8 error;
	bx_float32 value1 = 2.3;
	bx_float32 value2 = 109.45;

	// EQUALITY
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FEQ);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FEQ);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);

	// INEQUALITY
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FNE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FNE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);

	// GREATER THAN
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FGT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FGT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);

	// GREATER OR EQUAL
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FGE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);

	// LESS THAN
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FLT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FLT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);

	// GREATER OR EQUAL
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FLE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FLE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_TRUE);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FLE);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_bool(&test_field), BX_BOOLEAN_FALSE);
} END_TEST

START_TEST (int_expression) {
	bx_int8 error;
	bx_int32 operand1 = 23;
	bx_int32 operand2 = 68;
	bx_int32 operand3 = 9;
	bx_int32 operand4 = 12;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand3);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand4);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_ISUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), (operand1 + operand2) * operand3 - operand4);
} END_TEST

START_TEST (float_expression) {
	bx_int8 error;
	bx_float32 operand1 = 12.398;
	bx_float32 operand2 = -98.1;
	bx_float32 operand3 = -1.12;
	bx_float32 operand4 = 12;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand3);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FMUL);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, operand4);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_FSUB);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), (operand1 + operand2) * operand3 - operand4);
} END_TEST

START_TEST (unconditional_jump) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JUMP);
	bx_vmutils_add_short(&buffer, 47);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);
} END_TEST

START_TEST (jump_eq_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JEQZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JEQZ);
	bx_vmutils_add_int(&buffer, 12);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (jump_ne_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JNEZ);
	bx_vmutils_add_int(&buffer, 5);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JNEZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (jump_gt_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGTZ);
	bx_vmutils_add_int(&buffer, 5);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGTZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGTZ);
	bx_vmutils_add_int(&buffer, -8);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (jump_ge_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGEZ);
	bx_vmutils_add_int(&buffer, 5);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGEZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JGEZ);
	bx_vmutils_add_int(&buffer, -8);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (jump_lt_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLTZ);
	bx_vmutils_add_int(&buffer, -5);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLTZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLTZ);
	bx_vmutils_add_int(&buffer, 8);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (jump_le_zero) {
	bx_int8 error;
	bx_int32 value1 = 12;
	bx_int32 value2 = 21;

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLEZ);
	bx_vmutils_add_int(&buffer, -5);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLEZ);
	bx_vmutils_add_int(&buffer, 0);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value1);

	// Jump not taken
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_JLEZ);
	bx_vmutils_add_int(&buffer, 9);
	bx_vmutils_add_short(&buffer, 51);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, value2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_NOP);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), value2);
} END_TEST

START_TEST (cast_test) {
	bx_int8 error;
	bx_int32 int_value = 14;
	bx_float32 float_value = 17.890;

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, int_value);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_I2F);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_float(&test_field), (bx_float32) int_value);

	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_float(&buffer, float_value);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_F2I);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&test_field), (bx_int32) float_value);
} END_TEST

START_TEST (test_halt) {
	bx_int8 error;
	bx_int32 operand1 = 12;
	bx_int32 operand2 = 24;

	// ADDITION
	bx_test_field_set_int(&test_field, 0);
	bx_bbuf_reset(&buffer);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand1);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_PUSH32);
	bx_vmutils_add_int(&buffer, operand2);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_HALT);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_IADD);
	bx_vmutils_add_instruction(&buffer, BX_INSTR_STORE32);
	bx_vmutils_add_identifier(&buffer, TEST_FIELD_ID);

	error = bx_vm_execute(buffer.storage, bx_bbuf_size(&buffer));
	ck_assert_int_eq(error, 0);
	ck_assert_int_ne(bx_test_field_get_int(&test_field), operand1 + operand2);
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

	tcase = tcase_create("load32_test");
	tcase_add_test(tcase, load32_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_ipush_0");
	tcase_add_test(tcase, test_ipush_0);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_ipush_1");
	tcase_add_test(tcase, test_ipush_1);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fpush_0");
	tcase_add_test(tcase, test_fpush_0);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fpush_1");
	tcase_add_test(tcase, test_fpush_1);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("integer_arithmetics_test");
	tcase_add_test(tcase, integer_arithmetics_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("integer_bitwise_test");
	tcase_add_test(tcase, integer_bitwise_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("integer_comparison_test");
	tcase_add_test(tcase, integer_comparison_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("float_arithmetics_test");
	tcase_add_test(tcase, float_arithmetics_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("float_comparison_test");
	tcase_add_test(tcase, float_comparison_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("int_expression");
	tcase_add_test(tcase, int_expression);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("float_expression");
	tcase_add_test(tcase, float_expression);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("unconditional_jump");
	tcase_add_test(tcase, unconditional_jump);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_eq_zero");
	tcase_add_test(tcase, jump_eq_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_ne_zero");
	tcase_add_test(tcase, jump_ne_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_gt_zero");
	tcase_add_test(tcase, jump_gt_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_ge_zero");
	tcase_add_test(tcase, jump_ge_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_lt_zero");
	tcase_add_test(tcase, jump_lt_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("jump_le_zero");
	tcase_add_test(tcase, jump_le_zero);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("cast_test");
	tcase_add_test(tcase, cast_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_halt");
	tcase_add_test(tcase, test_halt);
	suite_add_tcase(suite, tcase);

	return suite;
}

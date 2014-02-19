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

#include <string.h>
#include "types.h"
#include "test_instruction_set.h"
#include "core/stack.h"
#include "virtual_machine/instruction_set.h"

#define STACK_SIZE 64

struct bx_stack execution_stack;
bx_uint8 stack_byte_array[STACK_SIZE];

bx_int32 int_operand1 = 56;
bx_int32 int_operand2 = 158;

bx_float32 float_operand1 = 83.3;
bx_float32 float_operand2 = 2954.4566;

START_TEST (test_init) {
	bx_instr_init();
	bx_stack_setup(&execution_stack, stack_byte_array, STACK_SIZE);
} END_TEST

START_TEST (test_iadd_function) {
	bx_int32 result;
	bx_instruction iadd_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand1);
	iadd_instruction = bx_instr_get(BX_IADD);
	iadd_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, int_operand1 + int_operand2);

} END_TEST

START_TEST (test_isub_function) {
	bx_int32 result;
	bx_instruction isub_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand1);
	isub_instruction = bx_instr_get(BX_ISUB);
	isub_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, int_operand1 - int_operand2);

} END_TEST

START_TEST (test_imul_function) {
	bx_int32 result;
	bx_instruction imul_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	bx_stack_setup(&execution_stack, stack_byte_array, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand1);
	imul_instruction = bx_instr_get(BX_IMUL);
	imul_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, int_operand1 * int_operand2);

} END_TEST

START_TEST (test_idiv_function) {
	bx_int32 result;
	bx_int32 simulated_result;
	bx_instruction idiv_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand1);
	idiv_instruction = bx_instr_get(BX_IDIV);
	idiv_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	simulated_result = int_operand1 / int_operand2;

	ck_assert_int_eq(result, simulated_result);

} END_TEST

START_TEST (test_imod_function) {
	bx_int32 result;
	bx_instruction imod_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, int_operand1);
	imod_instruction = bx_instr_get(BX_IMOD);
	imod_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, int_operand1 % int_operand2);

} END_TEST

START_TEST (test_fadd_function) {
	bx_float32 result;
	bx_instruction fadd_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand1);
	fadd_instruction = bx_instr_get(BX_FADD);
	fadd_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, float_operand1 + float_operand2);

} END_TEST

START_TEST (test_fsub_function) {
	bx_float32 result;
	bx_instruction fsub_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand1);
	fsub_instruction = bx_instr_get(BX_FSUB);
	fsub_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, float_operand1 - float_operand2);

} END_TEST

START_TEST (test_fmul_function) {
	bx_float32 result;
	bx_instruction fmul_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand1);
	fmul_instruction = bx_instr_get(BX_FMUL);
	fmul_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, float_operand1 * float_operand2);

} END_TEST

START_TEST (test_fdiv_function) {
	bx_float32 result;
	bx_instruction fdiv_instruction;

	memset((void *) stack_byte_array, 0, STACK_SIZE);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand2);
	BX_STACK_PUSH_VARIABLE(&execution_stack, float_operand1);
	fdiv_instruction = bx_instr_get(BX_FDIV);
	fdiv_instruction(&execution_stack);
	BX_STACK_POP_VARIABLE(&execution_stack, result);

	ck_assert_int_eq(result, float_operand1 / float_operand2);

} END_TEST

Suite *test_instruction_set_create_suite() {
	Suite *suite = suite_create("test_instruction_set");
	TCase *tcase;

	tcase = tcase_create("test_init");
	tcase_add_test(tcase, test_init);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_iadd_function");
	tcase_add_test(tcase, test_iadd_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_isub_function");
	tcase_add_test(tcase, test_isub_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_imul_function");
	tcase_add_test(tcase, test_imul_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_idiv_function");
	tcase_add_test(tcase, test_idiv_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_imod_function");
	tcase_add_test(tcase, test_imod_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fadd_function");
	tcase_add_test(tcase, test_fadd_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fsub_function");
	tcase_add_test(tcase, test_fsub_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fmul_function");
	tcase_add_test(tcase, test_fmul_function);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("test_fdiv_function");
	tcase_add_test(tcase, test_fdiv_function);
	suite_add_tcase(suite, tcase);

	return suite;
}

/*
 * test_event_handler.c
 * Created on: Mar 27, 2014
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
#include "test_task_scheduler.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"
#include "runtime/task_scheduler.h"
#include "compiler/codegen_pcode.h"
#include "virtual_machine/virtual_machine.h"
#include "runtime/critical_section.h"

#define INT_TEST_FIELD "int_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static bx_uint8 native_function_value;

static void native_event_function() {
	native_function_value = 1;
}

START_TEST (init_test) {
	bx_int8 error;

	// Init critical section
	error = bx_critical_init();
	ck_assert_int_eq(error, 0);

	// Init virtual machine
	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);

	// Init document manager
	error = bx_docman_init();
	ck_assert_int_eq(error, 0);
	error = bx_tfield_init(&int_test_field, &int_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_docman_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);

	// Init event handler
	error = bx_sched_init();
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (native_handler_test) {
	bx_int8 error;
	bx_task_id native_task_id;

	native_function_value = 0;
	native_task_id = bx_sched_add_native_task(*native_event_function);
	ck_assert_int_ne(native_task_id, -1);
	ck_assert_int_ne(native_function_value, 1);
	error = bx_sched_schedule_task(native_task_id);
	ck_assert_int_eq(bx_sched_is_scheduled(native_task_id), 1);
	ck_assert_int_eq(error, 0);
	bx_sched_scheduler_loop(BX_BOOLEAN_TRUE);
	ck_assert_int_eq(native_function_value, 1);
	error = bx_sched_remove_task(native_task_id);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (pcode_handler_test) {
	bx_int8 error;
	bx_task_id pcode_task_id;
	struct bx_comp_pcode *comp_pcode;

	comp_pcode = bx_cgpc_create();
	ck_assert_ptr_ne(comp_pcode, NULL);
	bx_tfield_set_int(&int_test_field, 0);
	bx_cgpc_add_instruction(comp_pcode, BX_INSTR_IPUSH_1);
	bx_cgpc_add_instruction(comp_pcode, BX_INSTR_RSTORE32);
	bx_cgpc_add_identifier(comp_pcode, INT_TEST_FIELD);
	bx_cgpc_add_instruction(comp_pcode, BX_INSTR_HALT);

	pcode_task_id = bx_sched_add_pcode_task(comp_pcode->data, comp_pcode->size);
	ck_assert_int_ne(pcode_task_id, -1);
	ck_assert_int_ne(bx_tfield_get_int(&int_test_field), 1);
	error = bx_sched_schedule_task(pcode_task_id);
	ck_assert_int_eq(bx_sched_is_scheduled(pcode_task_id), 1);
	ck_assert_int_eq(error, 0);
	bx_sched_scheduler_loop(BX_BOOLEAN_TRUE);
	ck_assert_int_eq(bx_tfield_get_int(&int_test_field), 1);

	error = bx_sched_remove_task(pcode_task_id);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_task_scheduler_create_suite() {
	Suite *suite = suite_create("task_scheduler");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("native_handler_test");
	tcase_add_test(tcase, native_handler_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("pcode_handler_test");
	tcase_add_test(tcase, pcode_handler_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

/*
 * test_codegen_task.c
 * Created on: Mar 17, 2014
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

#include "test_codegen_task.h"
#include "utils/linked_list.h"
#include "compiler/codegen_expression.h"
#include "compiler/codegen_task.h"

struct bx_comp_task *task;

START_TEST (create) {
	task = bx_cgtk_create_task();
	ck_assert_ptr_ne(task, NULL);
} END_TEST

START_TEST (at_execution_condition) {
	bx_int8 error;
	struct bx_comp_expr *execution_condition;

	execution_condition = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	ck_assert_ptr_ne(execution_condition, NULL);
	error = bx_cgtk_add_at_execution_condition(task, execution_condition);
	ck_assert_int_eq(error, 0);
	ck_assert_ptr_ne(task->at_execution_condition, NULL);
} END_TEST

START_TEST (on_execution_condition) {
	//TODO: Stub
} END_TEST

START_TEST (create_child_task) {
	struct bx_comp_task *child_task;

	child_task = bx_cgtk_create_child_task(task);
	ck_assert_ptr_ne(child_task, NULL);
	ck_assert_ptr_eq(child_task->parent, task);
	ck_assert_ptr_ne(task->child_task_list, NULL);
	ck_assert_int_eq(bx_llist_size(task->child_task_list), 1);
	ck_assert_ptr_ne(child_task, NULL);
	child_task = bx_cgtk_create_child_task(task);
	ck_assert_ptr_eq(child_task->parent, task);
	ck_assert_ptr_ne(child_task, NULL);
	ck_assert_int_eq(bx_llist_size(task->child_task_list), 2);
} END_TEST

START_TEST (destroy) {
	bx_int8 error;

	error = bx_cgtk_destroy_task(task);
	ck_assert_int_eq(error, 0);
} END_TEST

Suite *test_codegen_task_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("create");
	tcase_add_test(tcase, create);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("at_execution_condition");
	tcase_add_test(tcase, at_execution_condition);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("on_execution_condition");
	tcase_add_test(tcase, on_execution_condition);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("create_child_task");
	tcase_add_test(tcase, create_child_task);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("destroy");
	tcase_add_test(tcase, destroy);
	suite_add_tcase(suite, tcase);

	return suite;
}

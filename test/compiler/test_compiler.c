/*
 * test_compiler.c
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

#include <stdio.h>
#include <string.h>
#include "test_compiler.h"
#include "utils/fmemopen.h"
#include "compiler/lex.yy.h"
#include "compiler/y.tab.h"
#include "compiler/codegen_task.h"
#include "virtual_machine/virtual_machine.h"
#include "document_manager/document_manager.h"
#include "document_manager/test_field.h"

extern int yyparse();
extern int init_parser(struct bx_comp_task *);
extern FILE *yyin;

#define INT_TEST_FIELD "int_test_field"
#define FLOAT_TEST_FIELD "float_test_field"
#define BOOLEAN_TEST_FIELD "boolean_test_field"

#define INT_OUTPUT_TEST_FIELD "int_output_test_field"
#define FLOAT_OUTPUT_TEST_FIELD "float_output_test_field"
#define BOOLEAN_OUTPUT_TEST_FIELD "boolean_output_test_field"

static struct bx_document_field int_test_field;
static struct bx_test_field_data int_test_field_data;

static struct bx_document_field int_output_test_field;
static struct bx_test_field_data int_output_test_field_data;

static struct bx_document_field float_test_field;
static struct bx_test_field_data float_test_field_data;

static struct bx_document_field float_output_test_field;
static struct bx_test_field_data float_output_test_field_data;

static struct bx_document_field boolean_test_field;
static struct bx_test_field_data boolean_test_field_data;

static struct bx_document_field boolean_output_test_field;
static struct bx_test_field_data boolean_output_test_field_data;

static void run_program(char *program) {
	bx_int8 error;
	int parse_result;
	struct bx_comp_task *main_task;

	main_task = bx_cgtk_create_task();
	init_parser(main_task);
	yyin = fmemopen(program, strlen(program), "r");
	ck_assert_ptr_ne(yyin, NULL);
	parse_result = yyparse();
	ck_assert_int_eq(parse_result, 0);
	fclose(yyin);

	error = bx_vm_execute(main_task->code->data, main_task->code->size);
	ck_assert_int_eq(error, 0);
	bx_cgtk_destroy_task(main_task);
}

START_TEST (init_test) {
	bx_int8 error;

	// Init virtual machine
	error = bx_vm_virtual_machine_init();
	ck_assert_int_eq(error, 0);

	// Init document manager
	error = bx_dm_document_manager_init();
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&int_test_field, &int_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&float_test_field, &float_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&boolean_test_field, &boolean_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&int_test_field, INT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&float_test_field, FLOAT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&boolean_test_field, BOOLEAN_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&int_output_test_field, &int_output_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&float_output_test_field, &float_output_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_test_field_init(&boolean_output_test_field, &boolean_output_test_field_data);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&int_output_test_field, INT_OUTPUT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&float_output_test_field, FLOAT_OUTPUT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
	error = bx_dm_add_field(&boolean_output_test_field, BOOLEAN_OUTPUT_TEST_FIELD);
	ck_assert_int_eq(error, 0);
} END_TEST

START_TEST (parser_invocation_test) {
	int parse_result;
	struct bx_comp_task *main_task;
	char *program = "field int test; test = 5 + test;";

	main_task = bx_cgtk_create_task();
	init_parser(main_task);
	yyin = fmemopen(program, strlen(program), "r");
	ck_assert_ptr_ne(yyin, NULL);
	parse_result = yyparse();
	ck_assert_int_eq(parse_result, 0);
	fclose(yyin);
	bx_cgtk_destroy_task(main_task);
} END_TEST

START_TEST (assignment_test) {
	int parse_result;
	bx_int8 error;
	struct bx_comp_task *main_task;
	char *program = "field int int_test_field; int_test_field = 5;";

	main_task = bx_cgtk_create_task();
	init_parser(main_task);
	yyin = fmemopen(program, strlen(program), "r");
	ck_assert_ptr_ne(yyin, NULL);
	parse_result = yyparse();
	ck_assert_int_eq(parse_result, 0);
	fclose(yyin);

	error = bx_vm_execute(main_task->code->data, main_task->code->size);
	ck_assert_int_eq(error, 0);
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 5);
	bx_cgtk_destroy_task(main_task);
} END_TEST

START_TEST (expression_test) {
	run_program("field int int_test_field; int_test_field = 5 + 10;");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 15);

	run_program("field int int_test_field; int_test_field = 5 * 10;");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 50);

	run_program("field int int_test_field; int_test_field = 5 * 10 + 5;");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 55);

	run_program("field int int_test_field; int_test_field = 5 * (10 + 5);");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 75);

	run_program("field int int_test_field; int_test_field = 5 * (10 + 5);");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 75);

	run_program("field int int_test_field; int_test_field = 5 * (11 / 5);");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 10);

	run_program(
			"field int int_test_field;"
			"field float float_test_field;"
			"float_test_field = 12.5;"
			"int_test_field = 5 + float_test_field; ");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 17);

	run_program(
			"field bool boolean_test_field;"
			"boolean_test_field = true;");
	ck_assert_int_eq(bx_test_field_get_bool(&boolean_test_field), BX_BOOLEAN_TRUE);

	run_program(
			"field bool boolean_test_field;"
			"field int int_test_field;"
			"boolean_test_field = true;"
			"int_test_field = (int) boolean_test_field;");
	ck_assert_int_eq(bx_test_field_get_int(&int_test_field), 1);
} END_TEST

Suite *test_compiler_create_suite(void) {
	Suite *suite = suite_create("bx_linked_list");
	TCase *tcase;

	tcase = tcase_create("init_test");
	tcase_add_test(tcase, init_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("parser_invocation_test");
	tcase_add_test(tcase, parser_invocation_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("assignment_test");
	tcase_add_test(tcase, assignment_test);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("expression_test");
	tcase_add_test(tcase, expression_test);
	suite_add_tcase(suite, tcase);

	return suite;
}

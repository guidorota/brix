/*
 * test_language.c
 *
 * Created on: Feb 15, 2014
 * Author: sidewinder
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

#include <check.h>
#include "utils/test_stack.h"
#include "utils/test_list.h"
#include "utils/test_mixed_list.h"
#include "utils/test_byte_buffer.h"
#include "utils/test_linked_list.h"
#include "document_manager/test_document_manager.h"
#include "virtual_machine/test_virtual_machine.h"
#include "compiler/test_codegen_symbol_table.h"
#include "compiler/test_codegen_expression.h"

int main(void) {
	int number_failed = 0;
	SRunner *runner = srunner_create(NULL);

	srunner_set_fork_status(runner, CK_NOFORK);
	srunner_add_suite(runner, test_stack_create_suite());
	srunner_add_suite(runner, test_list_create_suite());
	srunner_add_suite(runner, test_mixed_list_create_suite());
	srunner_add_suite(runner, test_byte_buffer_create_suite());
	srunner_add_suite(runner, test_document_manager_create_suite());
	srunner_add_suite(runner, test_virtual_machine_create_suite());
	srunner_add_suite(runner, test_linked_list_create_suite());
	srunner_add_suite(runner, test_codegen_symbol_table_create_suite());
	srunner_add_suite(runner, test_codegen_expression_create_suite());

	srunner_run_all(runner, CK_VERBOSE);
	number_failed = srunner_ntests_failed(runner);
	return (number_failed == 0) ? 0 : -1;
}

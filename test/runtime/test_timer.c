/*
 * test_timer.c
 * Created on: Mar 26, 2014
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

#include "test_timer.h"
#include <unistd.h>
#include "runtime/timer.h"
#include "configuration.h"

START_TEST (timer_init) {
	bx_int8 error;
	bx_uint64 previous_count;

	error = bx_tm_init();
	ck_assert_int_eq(error, 0);
	previous_count = bx_tm_get_tick_count();
	sleep(1);
	ck_assert_int_gt(bx_tm_get_tick_count(), previous_count);
} END_TEST

START_TEST (timer_stop) {
	bx_int8 error;
	bx_uint64 previous_count;

	error = bx_tm_destroy();
	ck_assert_int_eq(error, 0);
	previous_count = bx_tm_get_tick_count();
	sleep(1);
	ck_assert_int_eq(bx_tm_get_tick_count(), previous_count);
} END_TEST

Suite *test_timer_create_suite() {
	Suite *suite = suite_create("timer");
	TCase *tcase;

	tcase = tcase_create("timer_init");
	tcase_add_test(tcase, timer_init);
	suite_add_tcase(suite, tcase);

	tcase = tcase_create("timer_stop");
	tcase_add_test(tcase, timer_stop);
	suite_add_tcase(suite, tcase);

	return suite;
}

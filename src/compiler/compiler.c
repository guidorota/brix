/*
 * compiler.c
 * Created on: Feb 15, 2014
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
#include "compiler/lex.yy.h"
#include "compiler/y.tab.h"
#include "compiler/codegen_task.h"

extern int yyparse();
extern int init_parser(struct bx_comp_task *);
extern FILE *yyin;

int main(int argc, char* argv[]) {
	int parse_result;
	struct bx_comp_task *main_task;

	if (argc < 2) {
		printf("Usage: compiler <file_name>\n");
		return -1;
	}

	main_task = bx_cgtk_create_task();
	if (main_task == NULL) {
		printf("Error creating main task\n");
		return -1;
	}

	yyin = fopen(argv[1], "r");
	init_parser(main_task);
	parse_result = yyparse();
	if (parse_result == 1) {
		printf("Error while parsing %s\n", argv[1]);
		return -1;
	}
	fclose(yyin);

	bx_cgtk_destroy_task(main_task);

	return 0;
}

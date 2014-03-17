/*
 * language.l
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


DIGIT		[0-9]
LETTER		[a-zA-Z_]

%top {
	#include <stdio.h>
	#include "types.h"
	#include "codegen_symbol_table.h"
	#include "codegen_expression.h"
	#include "codegen_code.h"
	#include "codegen_task.h"
	#include "y.tab.h"
}

%option nounput
%option noinput

%{
	void count_column();
%}

%%

"from"			{ count_column(); return FROM; }
"filter"		{ count_column(); return FILTER; }
"get"			{ count_column(); return GET; }
"every"			{ count_column(); return EVERY; }
"queue"			{ count_column(); return QUEUE; }
"window"		{ count_column(); return WINDOW; }
"each"			{ count_column(); return EACH; }
"local"			{ count_column(); return LOCAL; }
"parent"		{ count_column(); return PARENT; }
"@"				{ count_column(); return AT; }
"on"			{ count_column(); return ON; }
"new"			{ count_column(); return NEW; }
"change"		{ count_column(); return CHANGE; }
"if"			{ count_column(); return IF; }
"else"			{ count_column(); return ELSE; }
"allow"			{ count_column(); return ALLOW; }
"resample"		{ count_column(); return RESAMPLE; }
"do"			{ count_column(); return DO; }
"while"			{ count_column(); return WHILE; }
"for"			{ count_column(); return FOR; }

"existing"		{ count_column(); return EXISTING; }
"int"			{ count_column(); return INT; }
"float"			{ count_column(); return FLOAT; }
"bool"			{ count_column(); return BOOL; }
"string"		{ count_column(); return STRING; }
"stream"		{ count_column(); return STREAM; }
"subnet"		{ count_column(); return SUBNET; }

"++"			{ count_column(); return INC_OP; }
"--"			{ count_column(); return DEC_OP; }
"&&"			{ count_column(); return AND_OP; }
"||"			{ count_column(); return OR_OP; }
"=="			{ count_column(); return EQ_OP; }
"!="			{ count_column(); return NEQ_OP; }
"<="			{ count_column(); return LE_OP; }
">="			{ count_column(); return GE_OP; }
"<"				{ count_column(); return '<'; }
"+"				{ count_column(); return '+'; }
"-"				{ count_column(); return '-'; }
"*"				{ count_column(); return '*'; }
"/"				{ count_column(); return '/'; }
"%"				{ count_column(); return '%'; }
"|"				{ count_column(); return '|'; }
"&"				{ count_column(); return '&'; }
"^"				{ count_column(); return '^'; }
"="				{ count_column(); return '='; }
";"				{ count_column(); return ';'; }
"("				{ count_column(); return '('; }
")"				{ count_column(); return ')'; }
"{"				{ count_column(); return '{'; }
"}"				{ count_column(); return '}'; }
":"				{ count_column(); return ':'; }
"?"				{ count_column(); return '?'; }
"."				{ count_column(); return '.'; }
","				{ count_column(); return ','; }

{DIGIT}+				{ count_column(); yylval.int_val = atoi(yytext); return INT_CONSTANT; }
{DIGIT}*"."{DIGIT}+		{ count_column(); yylval.float_val = atof(yytext); return FLOAT_CONSTANT; }

LETTER?\"(\\.|[^\\"])*\"		{ count_column(); yylval.string_val = strdup(yytext); return STRING_LITERAL; }

{LETTER}({LETTER}|{DIGIT})*		{ count_column(); yylval.string_val = strdup(yytext); return IDENTIFIER; }

%%

int yywrap() {
	return(1);
}

int column = 0;

void count_column() {
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	ECHO;
}
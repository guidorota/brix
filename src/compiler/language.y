/*
 * language.y
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

%{
#include "types.h"
#include "codegen_symbol_table.h"
#include "codegen_expression.h"
#include "codegen_code.h"
#include "codegen_task.h"
#include "codegen_while_statement.h"

#define YYDEBUG 1
int yylex(void);
int yyerror(char *);
int init_parser(struct bx_comp_task *);

static struct bx_comp_task *current_task;

%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT AT DOCUMENT
%token ON CHANGE ALLOW RESAMPLE EXISTING NEW INC_OP DEC_OP FIELD IF
%token AND_OP OR_OP EQ_OP NEQ_OP LE_OP GE_OP TRUE_CONSTANT FALSE_CONSTANT
%token INT FLOAT BOOL STRING STREAM SUBNET

%token <int_val> INT_CONSTANT
%token <float_val> FLOAT_CONSTANT
%token <string_val> IDENTIFIER
%token <string_val> STRING_LITERAL

%token <jump_label> ELSE
%token <instruction_address> DO
%token <while_statement> FOR
%token <while_statement> WHILE

%type <expression> expression
%type <expression> conditional_expression
%type <expression> logical_or_expression
%type <expression> logical_and_expression
%type <expression> inclusive_or_expression
%type <expression> exclusive_or_expression
%type <expression> and_expression
%type <expression> equality_expression
%type <expression> relational_expression
%type <expression> additive_expression
%type <expression> multiplicative_expression
%type <expression> postfix_expression
%type <expression> primary_expression
%type <expression> cast_expression
%type <expression> unary_expression
%type <expression> assignment_expression
%type <operator> unary_operator
%type <data_type> type_name
%type <creation_modifier> creation_modifier

%type <jump_label> if_statement

%union {
	enum bx_comp_creation_modifier creation_modifier;
	enum bx_builtin_type data_type;
	enum bx_comp_operator operator;
	bx_int32 int_val;
	bx_float32 float_val;
	char *string_val;
	struct bx_comp_expr *expression;
	bx_comp_label jump_label;
	struct bx_comp_while *while_statement;
	bx_uint16 instruction_address;
}

%start network_definition

%%

network_definition
	: statement_list
	{
		bx_cgco_add_instruction(current_task->code, BX_INSTR_HALT);
	}
	;

statement_list
	: statement
	| statement_list statement
	;

statement
	: compound_statement
	| declaration_statement
	| selection_statement
	| iteration_statement
	| expression_statement
	| conditional_execution_statement
	;
	
compound_statement
	: '{' '}'
	{
		bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
	}
	| '{' statement_list '}'
	{

	}
	;
	
declaration_statement
	: creation_modifier FIELD type_name IDENTIFIER ';'
	{
		bx_cgsy_add_field(current_task->symbol_table, $4, $3, $1);
	}
	| creation_modifier FIELD type_name IDENTIFIER '=' expression ';'
	{
	
	}
	| type_name IDENTIFIER ';'
	{
		bx_cgsy_add_variable(current_task->symbol_table, $2, $1);
	}
	| type_name IDENTIFIER '=' expression ';'
	{
	
	}
	;
	
type_name
	: INT
	{
		$$ = BX_INT;
	}
	| FLOAT
	{
		$$ = BX_FLOAT;
	}
	| BOOL
	{
		$$ = BX_BOOL;
	}
	| STRING
	{
		$$ = BX_STRING;
	}
	| STREAM
	{
		$$ = BX_STREAM;
	}
	| SUBNET
	{
		$$ = BX_SUBNET;
	}
	;
	
creation_modifier
	:
	{
		$$ = BX_COMP_EXISTING;
	}
	| EXISTING
	{
		$$ = BX_COMP_EXISTING;
	}
	| NEW
	{
		$$ = BX_COMP_NEW;
	}
	;
	
iteration_statement
	: WHILE '(' expression ')'
	{
		struct bx_comp_expr *condition;
		struct bx_comp_while *while_statement;
		
		while_statement = bx_cgwh_create();
		condition = bx_cgex_cast($3, BX_BOOL);
		bx_cgex_convert_to_binary(condition);
		while_statement->condition_address = bx_cgco_append_code(current_task->code, condition->value.code);
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JEQZ);
		while_statement->label = bx_cgco_create_address_label(current_task->code);
		$1 = while_statement;
		
		bx_cgex_destroy_expression(condition);
		bx_cgex_destroy_expression($3);
	}
	statement
	{
		bx_uint16 jump_address;
		
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JUMP);
		bx_cgco_add_address(current_task->code, $1->condition_address);
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $1->label, jump_address);
		
		bx_cgwh_destroy($1);
	}
	| DO 
	{
		$1 = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
	}
	statement WHILE '(' expression ')' ';'
	{
		struct bx_comp_expr *condition;
		
		condition = bx_cgex_cast($6, BX_BOOL);
		bx_cgex_convert_to_binary(condition);
		bx_cgco_append_code(current_task->code, condition->value.code);
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JNEZ);
		bx_cgco_add_address(current_task->code, $1);
		
		bx_cgex_destroy_expression(condition);
		bx_cgex_destroy_expression($6);
	}
	| FOR '(' expression ';' expression ';' expression ')'
	{
		struct bx_comp_expr *condition;
		struct bx_comp_while *while_statement;
		
		bx_cgex_convert_to_binary($3);
		bx_cgco_append_code(current_task->code, $3->value.code);
		
		while_statement = bx_cgwh_create();
		condition = bx_cgex_cast($5, BX_BOOL);
		bx_cgex_convert_to_binary(condition);
		while_statement->condition_address = bx_cgco_append_code(current_task->code, condition->value.code);
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JEQZ);
		while_statement->label = bx_cgco_create_address_label(current_task->code);
		$1 = while_statement;
		
		bx_cgex_destroy_expression($3);
		bx_cgex_destroy_expression(condition);
		bx_cgex_destroy_expression($5);
	}
	statement
	{
		bx_uint16 jump_address;
		
		bx_cgex_convert_to_binary($7);
		bx_cgco_append_code(current_task->code, $7->value.code);
		
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JUMP);
		bx_cgco_add_address(current_task->code, $1->condition_address);
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $1->label, jump_address);
		
		bx_cgwh_destroy($1);
		bx_cgex_destroy_expression($7);
	}
	| FOR '(' expression ';' expression ';' ')'
	{
		struct bx_comp_expr *condition;
		struct bx_comp_while *while_statement;
		
		bx_cgex_convert_to_binary($3);
		bx_cgco_append_code(current_task->code, $3->value.code);
		
		while_statement = bx_cgwh_create();
		condition = bx_cgex_cast($5, BX_BOOL);
		bx_cgex_convert_to_binary(condition);
		while_statement->condition_address = bx_cgco_append_code(current_task->code, condition->value.code);
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JEQZ);
		while_statement->label = bx_cgco_create_address_label(current_task->code);
		$1 = while_statement;
		
		bx_cgex_destroy_expression($3);
		bx_cgex_destroy_expression(condition);
		bx_cgex_destroy_expression($5);
	}
	statement
	{
		bx_uint16 jump_address;
		
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JUMP);
		bx_cgco_add_address(current_task->code, $1->condition_address);
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $1->label, jump_address);
		
		bx_cgwh_destroy($1);
	}
	;
	
selection_statement
	: if_statement statement
	{
		bx_uint16 jump_address;
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $1, jump_address);
	}
	| if_statement statement ELSE 
	{
		bx_uint16 jump_address;
		
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JUMP);
		$3 = bx_cgco_create_address_label(current_task->code);
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $1, jump_address);
	}
	statement
	{
		bx_uint16 jump_address;
		
		jump_address = bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
		bx_cgco_set_address_label(current_task->code, $3, jump_address);
	}
	;
	
if_statement
	: IF '(' expression ')'
	{
		struct bx_comp_expr *condition;
		
		condition = bx_cgex_cast($3, BX_BOOL);
		bx_cgex_convert_to_binary(condition);
		bx_cgco_append_code(current_task->code, condition->value.code);
		bx_cgco_add_instruction(current_task->code, BX_INSTR_JEQZ);
		$$ = bx_cgco_create_address_label(current_task->code);
	
		bx_cgex_destroy_expression(condition);
		bx_cgex_destroy_expression($3);
	}
	;
	
conditional_execution_statement
	: AT '(' expression ')' statement
	{
	
	}
	| ON '(' event_descriptor ')' statement
	{
	
	}
	| EVERY '(' expression ')' statement
	{
	
	}
	;
	
event_descriptor
	: source_modifier event_trigger_modifier expression IDENTIFIER
	;
	
source_modifier
	:
	| LOCAL
	;
	
event_trigger_modifier
	:
	| NEW
	| CHANGE
	;
	
expression_statement
	: ';'
	{
		bx_cgco_add_instruction(current_task->code, BX_INSTR_NOP);
	}
	| expression ';'
	{
		struct bx_comp_code *code;
		
		code = bx_cgex_side_effect_code($1);
		bx_cgco_append_code(current_task->code, code);
		
		bx_cgex_destroy_expression($1);
	}
	;
	
expression
	: conditional_expression
	{
		$$ = $1;
	}
	| assignment_expression
	{
		$$ = $1;
	}
	| query_expression
	{
	
	}
	| resample_expression
	{
	
	}
	;

assignment_expression
	: postfix_expression '=' expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_ASSIGNMENT);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
primary_expression
	: IDENTIFIER
	{
		$$ = bx_cgex_create_variable(current_task->symbol_table, $1);
	}
	| INT_CONSTANT 			
	{
		$$ = bx_cgex_create_int_constant($1);
	}
	| FLOAT_CONSTANT 		
	{
		$$ = bx_cgex_create_float_constant($1);
	}
	| TRUE_CONSTANT
	{
		$$ = bx_cgex_create_bool_constant(BX_BOOLEAN_TRUE);
	}
	| FALSE_CONSTANT
	{
		$$ = bx_cgex_create_bool_constant(BX_BOOLEAN_FALSE);
	}
	| STRING_LITERAL
	{
	
	}
	| '(' expression ')'
	{
		$$ = $2;
	}
	;
	
postfix_expression
	: primary_expression
	{
		$$ = $1;
	}
	| postfix_expression '[' expression ']'
	{
	
	}
	| postfix_expression '.' IDENTIFIER
	{
	
	}
	| postfix_expression INC_OP
	{
		$$ = bx_cgex_unary_expression($1, BX_COMP_OP_POSTFIX_INC);
		bx_cgex_destroy_expression($1);
	}
	| postfix_expression DEC_OP
	{
		$$ = bx_cgex_unary_expression($1, BX_COMP_OP_POSTFIX_DEC);
		bx_cgex_destroy_expression($1);
	}
	;
	
unary_expression
	: postfix_expression
	{
		$$ = $1;
	}
	| INC_OP unary_expression
	{
		$$ = bx_cgex_unary_expression($2, BX_COMP_OP_PREFIX_INC);
		bx_cgex_destroy_expression($2);
	}
	| DEC_OP unary_expression
	{
		$$ = bx_cgex_unary_expression($2, BX_COMP_OP_PREFIX_DEC);
		bx_cgex_destroy_expression($2);
	}
	| unary_operator cast_expression
	{
		$$ = bx_cgex_unary_expression($2, $1);
		bx_cgex_destroy_expression($2);
	}
	;

unary_operator
	: '-'
	{
		$$ = BX_COMP_OP_UNARY_MINUS;
	}
	| '+'
	{
		$$ = BX_COMP_OP_UNARY_PLUS;
	}
	| '~'
	{
		$$ = BX_COMP_OP_BITWISE_COMPLEMENT;
	}
	| '!'
	{
		$$ = BX_COMP_OP_NOT;
	}
	;
	
cast_expression
	: unary_expression
	{
		$$ = $1;
	}
	| '(' type_name ')' cast_expression
	{
		$$ = bx_cgex_cast($4, $2);
		bx_cgex_destroy_expression($4);
	}
	;
	
multiplicative_expression
	: cast_expression
	{
		$$ = $1;
	}
	| multiplicative_expression '*' postfix_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_MUL);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| multiplicative_expression '/' postfix_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_DIV);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| multiplicative_expression '%' postfix_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_MOD);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
additive_expression
	: multiplicative_expression
	{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_ADD);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| additive_expression '-' multiplicative_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_SUB);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;

relational_expression
	: additive_expression
	{
		$$ = $1;
	}
	| relational_expression '>' additive_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_GT);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| relational_expression '<' additive_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_LT);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| relational_expression 'GE_OP' additive_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_GE);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| relational_expression 'LE_OP' additive_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_LE);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
equality_expression
	: relational_expression
	{
		$$ = $1;
	}
	| equality_expression 'EQ_OP' relational_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_EQ);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	| equality_expression 'NEQ_OP' relational_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_NE);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
and_expression
	: equality_expression
	{
		$$ = $1;
	}
	| and_expression '&' equality_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_BITWISE_AND);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
exclusive_or_expression
	: and_expression
	{
		$$ = $1;
	}
	| exclusive_or_expression '^' and_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_BITWISE_XOR);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
inclusive_or_expression
	: exclusive_or_expression
	{
		$$ = $1;
	}
	| inclusive_or_expression '|' exclusive_or_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_BITWISE_OR);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
logical_and_expression
	: inclusive_or_expression
	{
		$$ = $1;
	}
	| logical_and_expression 'AND_OP' equality_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_AND);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
logical_or_expression
	: logical_and_expression
	{
		$$ = $1;
	}
	| logical_or_expression 'OR_OP' logical_and_expression
	{
		$$ = bx_cgex_binary_expression($1, $3, BX_COMP_OP_OR);
		bx_cgex_destroy_expression($1);
		bx_cgex_destroy_expression($3);
	}
	;
	
conditional_expression
	: logical_or_expression
	{
		$$ = $1;
	}
	| logical_or_expression '?' expression ':' conditional_expression
	;

resample_expression
	: RESAMPLE source_modifier IDENTIFIER EVERY expression
	;
	
query_expression
	: subnet_expression
	| subnet_expression document_stream
	;
	
document_stream
	: windowed_document_stream
	| ON event_descriptor windowed_document_stream
	| EVERY conditional_expression resample_modifier windowed_document_stream
	;
	
resample_modifier
	:
	| ALLOW RESAMPLE
	;
	
windowed_document_stream
	: basic_document_stream
	| WINDOW conditional_expression EACH conditional_expression basic_document_stream
	| QUEUE conditional_expression EACH conditional_expression basic_document_stream
	;
	
basic_document_stream
	: GET document_structure_definition
	| GET document_structure_definition FILTER conditional_expression
	;
	
subnet_expression
	: FROM conditional_expression
	| FROM conditional_expression FILTER conditional_expression
	;

document_structure_definition
	: DOCUMENT '{' document_field_list '}'
	;
	
document_field_list
	: document_field
	| document_field_list ',' document_field
	;
	
document_field
	: type_name IDENTIFIER
	| type_name IDENTIFIER ':' expression
	| type_name IDENTIFIER ':' document_structure_definition
	;

%%

#include <stdio.h>

extern char yytext[];
extern int column;

int init_parser(struct bx_comp_task *main_task) {
	if (main_task == NULL) {
		return -1;
	}
	current_task = main_task;
	
	return 0;
}

int yyerror(char *error) {
	printf("Error while parsing: %s\n", error);
	return 0;
}

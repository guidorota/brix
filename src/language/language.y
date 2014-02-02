%{
int yylex(void);
int yyerror(char *);
%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT RESAMPLE AT
%token ON NEW CHANGE IDENTIFIER CONSTANT STRING_LITERAL

%start statement_list

%%

statement_list
	: code_block
	| statement_list code_block
	;
	
code_block
	: statement
	| '{' statement_list '}'
	;

statement
	: expression_statement
	| conditional_execution_statement
	;
	
conditional_execution_statement
	: AT IDENTIFIER code_block
	| ON '(' local_stream_modifier event_declaration ')' code_block
	;
	
local_stream_modifier
	:
	| LOCAL
	;
	
event_declaration
	: event_modifier IDENTIFIER IDENTIFIER
	;
	
event_modifier
	: NEW
	| CHANGE
	;
	
expression_statement
	: ';'
	| expression ';'
	;
	
expression
	: conditional_expression
	| postfix_expression '=' expression_statement 
	;
	
postfix_expression
	: primary_expression
	| primary_expression '[' expression ']'
	| primary_expression '.' IDENTIFIER
	;
	
primary_expression
	: IDENTIFIER
	| CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;
	
logical_or_expression
	: logical_and_expression
	| logical_or_expression 'OR_OP' logical_and_expression
	;
	
logical_and_expression
	: equality_expression
	| logical_and_expression 'AND_OP' equality_expression
	;
	
equality_expression
	: relational_expression
	| equality_expression 'EQ_OP' relational_expression
	| equality_expression 'NEQ_OP' relational_expression
	;
	
relational_expression
	: additive_expression
	| relational_expression '>' additive_expression
	| relational_expression '<' additive_expression
	| relational_expression 'GE_OP' additive_expression
	| relational_expression 'LE_OP' additive_expression
	;
	
additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;
	
multiplicative_expression
	: postfix_expression
	| multiplicative_expression '*' postfix_expression
	| multiplicative_expression '/' postfix_expression
	| multiplicative_expression '%' postfix_expression
	;

%%

#include <stdio.h>

extern char yytext[];
extern int column;
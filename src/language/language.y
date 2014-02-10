%{
#define YYDEBUG 1
int yylex(void);
int yyerror(char *);
%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT RESAMPLE AT
%token ON NEW CHANGE IDENTIFIER CONSTANT STRING_LITERAL IF ELSE
%token AND_OP OR_OP EQ_OP NEQ_OP LE_OP GE_OP

%union {
   int intval;
   float floatval;
   char *stringval;
} 

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
	: selection_statement
	| expression_statement
	| conditional_execution_statement
	| query_expression
	;
	
selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	
conditional_execution_statement
	: AT expression code_block
	| ON event_descriptor code_block
	;
	
event_descriptor
	: event_source_modifier event_trigger_modifier expression IDENTIFIER
	;
	
event_source_modifier
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
	| expression ';'
	;
	
expression
	: conditional_expression
	| assignment_expression
	;
	
	


query_expression
	: post_filter_clause
	;
	
post_filter_clause
	: window_clause
	| window_clause FILTER expression
	;

window_clause
	: get_clause
	| WINDOW expression EACH expression get_clause
	| QUEUE expression EACH expression get_clause
	;
	
get_clause
	: subnet_expression_clause
	| subnet_expression_clause GET IDENTIFIER
	;
	
subnet_expression_clause
	: from_clause 
	| from_clause FILTER expression
	;
	
from_clause
	: FROM basic_query_expression
	;
	
basic_query_expression
	: '(' query_expression ')'
	| IDENTIFIER
	;

	
	

	
assignment_expression
	: postfix_expression '=' expression
	;
	
postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '.' IDENTIFIER
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

int yyerror(char *error) {
	printf("Error while parsing: %s\n", error);
	return 0;
}

int main(int argc, char* argv[]) {
	yydebug = 0;
	yyparse();
}

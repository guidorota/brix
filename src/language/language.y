%{
#define YYDEBUG 1
int yylex(void);
int yyerror(char *);
%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT RESAMPLE AT
%token ON NEW CHANGE IDENTIFIER CONSTANT STRING_LITERAL
%token AND_OP OR_OP EQ_OP NEQ_OP LE_OP GE_OP

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
	| AT '(' node_set_descriptor ')' code_block
	| ON '(' event IDENTIFIER ')' code_block
	;
	
event
	: event_source_modifier event_trigger_modifier IDENTIFIER
	| event_source_modifier event_trigger_modifier '(' query_expression ')'
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
	: node_set_descriptor
	| document_stream
	;
	
node_set_descriptor
	: from_clause
	| from_clause filter_clause_list
	;
	
document_stream
	: node_set_descriptor document_stream_clause
	;
	
document_stream_clause
	: get_clause selection_clause_list execution_condition_clause
	;
	
get_clause
	: GET IDENTIFIER
	;
	
from_clause
	: FROM IDENTIFIER
	| FROM '(' query_expression ')'
	;
	
filter_clause_list
	: filter_clause
	| filter_clause_list filter_clause
	;
	
filter_clause
	: FILTER expression
	;
	
selection_clause_list
	:
	| selection_clause
	| selection_clause_list selection_clause
	;
	
selection_clause
	: filter_clause
	| get_clause
	| WINDOW expression EACH expression
	| QUEUE expression EACH expression
	;
	
execution_condition_clause
	:
	| EVERY expression
	| ON event_list
	;
	
event_list
	: event
	| event_list ',' event
	;
	
	
	
	
	
	
	
	
	
assignment_expression
	: postfix_expression '=' expression
	| postfix_expression '=' query_expression
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

int yyerror(char *error) {
	printf("Error while parsing: %s\n", error);
	return 0;
}

int main(int argc, char* argv[]) {
	yydebug = 0;
	yyparse();
}

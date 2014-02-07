%{
#define YYDEBUG 1
int yylex(void);
int yyerror(char *);
%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT RESAMPLE AT
%token ON NEW CHANGE IDENTIFIER CONSTANT STRING_LITERAL
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
	: expression_statement
	| conditional_execution_statement
	;
	
conditional_execution_statement
	: AT IDENTIFIER code_block
	| ON event_descriptor code_block
	;
	
event_descriptor
	: event_source_modifier event_trigger_modifier IDENTIFIER IDENTIFIER
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
	: filtered_document_stream
	| document_stream
	| subnet_expression
	;
	
filtered_document_stream
	: document_stream FILTER expression
	;
	
document_stream
	: queue_clause GET IDENTIFIER
	| subnet_expression GET IDENTIFIER
	| from_clause GET IDENTIFIER
	;
	
queue_clause
	: subnet_expression WINDOW expression EACH expression
	| subnet_expression QUEUE expression EACH expression
	| from_clause WINDOW expression EACH expression
	| from_clause QUEUE expression EACH expression
	
subnet_expression
	: from_clause FILTER expression
	;

from_clause
	: FROM IDENTIFIER
	| execution_condition FROM IDENTIFIER
	;
	
execution_condition
	: EVERY expression
	| ON event_descriptor
	;
	

	
	
	
assignment_expression
	: postfix_expression '=' expression
	| postfix_expression '=' query_expression
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

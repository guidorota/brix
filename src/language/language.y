%{
#define YYDEBUG 1
int yylex(void);
int yyerror(char *);
%}

%token FROM NETWORK FILTER GET EVERY QUEUE WINDOW EACH LOCAL PARENT AT DOCUMENT
%token ON NEW CHANGE IDENTIFIER CONSTANT STRING_LITERAL IF ELSE ALLOW RESAMPLE 
%token DO WHILE FOR AND_OP OR_OP EQ_OP NEQ_OP LE_OP GE_OP
%token INT FLOAT BOOL STRING STREAM SUBNET

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
	: declaration_statement
	| selection_statement
	| iteration_statement
	| expression_statement
	| conditional_execution_statement
	;
	
declaration_statement
	: type_specifier IDENTIFIER ';'
	| type_specifier IDENTIFIER '=' expression ';'
	;
	
type_specifier
	: INT
	| FLOAT
	| BOOL
	| STRING
	| STREAM
	| SUBNET
	;
	
iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;
	
selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	;
	
conditional_execution_statement
	: AT expression code_block
	| ON event_descriptor code_block
	| EVERY expression code_block
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
	| expression ';'
	;
	
expression
	: conditional_expression
	| assignment_expression
	| query_expression
	| resample_expression
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
	: type_specifier IDENTIFIER
	| type_specifier IDENTIFIER ':' expression
	| type_specifier IDENTIFIER ':' document_definition
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

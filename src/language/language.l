DIGIT		[0-9]
LETTER		[a-zA-Z_]

%top{
	#include <stdio.h>
	#include "y.tab.h"
}

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

"document"		{ count_column(); return DOCUMENT; }
"int"			{ count_column(); return INT; }
"float"			{ count_column(); return FLOAT; }
"bool"			{ count_column(); return BOOL; }
"string"		{ count_column(); return STRING; }
"stream"		{ count_column(); return STREAM; }
"subnet"		{ count_column(); return SUBNET; }

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

{DIGIT}+				{ count_column(); yylval.intval = atoi(yytext); return CONSTANT; }
{DIGIT}*"."{DIGIT}+		{ count_column(); yylval.floatval = atof(yytext); return CONSTANT; }

LETTER?\"(\\.|[^\\"])*\"		{ count_column(); yylval.stringval = strdup(yytext); return STRING_LITERAL; }

{LETTER}({LETTER}|{DIGIT})*		{ count_column(); yylval.stringval = strdup(yytext); return IDENTIFIER; }

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
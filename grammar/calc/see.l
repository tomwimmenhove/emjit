%{
#include <stdio.h>
#include "see.tab.h"
%}

digit		[0-9]
letter 		[a-zA-Z]
integer		{digit}+
identifier	[_a-zA-Z]+[_a-zA-Z0-9]*

%%

{integer}       { yylval = atoi(yytext); return TOKEN_INT; }
{identifier}    { return TOKEN_ID; }

"+"             { return TOKEN_PLUS; }
"-"             { return TOKEN_MINUS; }
"/"             { return TOKEN_DIV; }
"*"             { return TOKEN_MUL; }

"\("            { return TOKEN_OPEN; }
"\)"            { return TOKEN_CLOSE; }

"\n"			{ return TOKEN_NEWLINE; }

[ \t\r]*      ;

%%

int yywrap()
{
	return 1;
}


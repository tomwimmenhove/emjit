%{
 #include <stdio.h>
 int yylex(void);
 void yyerror(char *);
%}
%token TOKEN_INT TOKEN_ID TOKEN_OPEN TOKEN_CLOSE TOKEN_PLUS TOKEN_MINUS TOKEN_MUL TOKEN_DIV TOKEN_NEWLINE TOKEN_SEMICOLON

%left TOKEN_PLUS
%left TOKEN_MINUS
%left TOKEN_MUL
%left TOKEN_DIV

%%
lines	: lines line
	  	| line
		;

line	: expr TOKEN_SEMICOLON	{ printf("%d\n", $$); }
	 	;

expr	: expr TOKEN_PLUS expr			{ $$ = $1 + $3; }
	 	| expr TOKEN_MINUS expr			{ $$ = $1 - $3; }
	 	| expr TOKEN_MUL expr			{ $$ = $1 * $3; }
	 	| expr TOKEN_DIV expr			{ $$ = $1 / $3;  }
		| TOKEN_OPEN expr TOKEN_CLOSE	{ $$ = $2; }
		| TOKEN_INT
		;
%%

void yyerror(char *s) {
 fprintf(stderr, "%s\n", s);
}
int main(void) {
 yyparse();
 return 0;
} 

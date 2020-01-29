%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <iostream>
  #include "expression.h"
  class driver;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.h"
}

%define api.token.prefix {TOK_}
%token
  END  0    "end of file"
  COMMA		","
  ASSIGN    "="
  MINUS     "-"
  PLUS      "+"
  STAR      "*"
  SLASH     "/"
  LPAREN    "("
  RPAREN    ")"
  LBRACKET	"{"
  RBRACKET	"}"
  SEMICOLON ";"
  RETURN	"return"
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%type  <expression> exp
%type  <declaration> declaration

%printer { yyoutput << $$; } <int>;

%%
%start functions;

functions	:
			| functions function
			;

function   : IDENTIFIER "(" parameters ")" "{" statements "}"

parameter	: IDENTIFIER

parameters	:
			| parameter
			| parameters "," parameter
			;

statements : statement
           | statements statement
           ;

statement : declaration				{ drv.statements.push_back(statement($1)); }
          | "return" exp ";"		{ drv.statements.push_back(statement($2)); }
          ;
          
declaration : IDENTIFIER "=" exp ";" { $$ = declaration{drv.decl_var_id($1), $3}; drv.expression_result = $3; }
            ;

%left "+" "-";
%left "*" "/";
exp	: exp "+" exp   { $$ = expression(expr_type::add, $1, $3); }
   	| exp "-" exp   { $$ = expression(expr_type::sub, $1, $3); }
   	| exp "*" exp   { $$ = expression(expr_type::mul, $1, $3); }
   	| exp "/" exp   { $$ = expression(expr_type::div, $1, $3); }
	| "(" exp ")"   { $$ = $2; }
   	| "number"      { $$ = expression(expr_type::num, $1); }
   	| "identifier"	{ $$ = expression(expr_type::var, drv.get_var_id($1)); }
   	;

%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}

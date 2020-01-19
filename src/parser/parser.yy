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
  ASSIGN    "="
  MINUS     "-"
  PLUS      "+"
  STAR      "*"
  SLASH     "/"
  LPAREN    "("
  RPAREN    ")"
  SEMICOLON ";"
  RETURN	"return"
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%type  <expression> exp
%type  <declaration> declaration

%printer { yyoutput << $$; } <int>;
%printer { yyoutput << ($$).eval(); } <expression>;

%%
%start statements;

statements : statement
           | statements statement
           ;

statement : declaration				{ drv.statements.push_back(statement($1)); }
          | "return" exp ";"		{ drv.statements.push_back(statement($2)); }
          ;
          
declaration : IDENTIFIER "=" exp ";" { $$ = declaration{drv.decl_var_id($1), $3}; drv.add_decl($1, $3); drv.expression_result = $3; std::cout << "Got an expression\n"; }
            ;

%left "+" "-";
%left "*" "/";
exp	: exp "+" exp   { /*std::cout << "add\n";*/ $$ = expression(expr_type::add, $1, $3); }
   	| exp "-" exp   { /*std::cout << "sub\n";*/ $$ = expression(expr_type::sub, $1, $3); }
   	| exp "*" exp   { /*std::cout << "mul\n";*/ $$ = expression(expr_type::mul, $1, $3); }
   	| exp "/" exp   { /*std::cout << "div\n";*/ $$ = expression(expr_type::div, $1, $3); }
	| "(" exp ")"   { $$ = $2; }
   	| "number"      { /*std::cout << "number " << $1 << '\n';*/ $$ = expression(expr_type::num, $1); }
   	| "identifier"	{ $$ = expression(expr_type::var, drv.get_var_id($1)); }
   	;

%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}

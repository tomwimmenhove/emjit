%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include "expression.h"
  class driver;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hh"
}

%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  ASSIGN  ":="
  MINUS   "-"
  PLUS    "+"
  STAR    "*"
  SLASH   "/"
  LPAREN  "("
  RPAREN  ")"
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%type  <expression> exp

%printer { yyoutput << $$; } <int>;
%printer { yyoutput << ($$).eval(); } <expression>;

%%
%start unit;
unit:  exp  { drv.done($1); };

%left "+" "-";
%left "*" "/";
exp	: exp "+" exp   { $$ = expression(expr_type::add, $1, $3); }
   	| exp "-" exp   { $$ = expression(expr_type::sub, $1, $3); }
   	| exp "*" exp   { $$ = expression(expr_type::mul, $1, $3); }
   	| exp "/" exp   { $$ = expression(expr_type::div, $1, $3); }
	| "(" exp ")"   { $$ = $2; }
   	| "number"      { $$ = expression($1); }
   	;
%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}

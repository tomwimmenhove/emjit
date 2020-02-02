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
	END				0			"end of file"
	COMMA						","
	ASSIGN  					"="
	MINUS   					"-"
	PLUS    					"+"
	STAR    					"*"
	SLASH   					"/"
	LPAREN  					"("
	RPAREN   					")"
	LBRACKET					"{"
	RBRACKET					"}"
	SEMICOLON 					";"
	VAR							"var"
	RETURN						"return"
	<std::string>	IDENTIFIER	"identifier"
	<int>			NUMBER		"number"
;

%type  <int>							parameter
%type  <std::vector<int>>				parameters
%type  <expression>						exp
%type  <declaration>					declaration
%type  <statement>						statement
%type  <std::vector<statement>>			statements
%type  <emjit_function>					function
%type  <std::vector<emjit_function>>	functions

%%
%start functions;

functions	:									{ }
			| functions function				{ drv.add_function($2); }
			;

function 	: "identifier"						{ drv.new_var_scope(); }
			  "(" parameters ")"
			  "{" statements "}"				{ $$ = emjit_function{$1, $4, $7, drv.var_scope}; drv.destroy_var_scope(); }
			;

parameter	: "identifier"						{ $$ = drv.decl_var_id($1); }
			;

parameters	:									{ }
			| parameter							{ $$.push_back($1); }
			| parameters "," parameter			{ $1.push_back($3); $$ = $1; }
			;

statements	: statement							{ $$.push_back($1); }
			| statements statement				{ $1.push_back($2); $$ = $1; }
			;

statement	: declaration						{ $$ = statement($1); }
			| "return" exp ";"					{ $$ = statement($2); }
			;

declaration	: "identifier" "=" exp ";"			{ $$ = declaration{drv.get_var_id($1), $3}; }
			| "var" "identifier" ";"			{ $$ = declaration{drv.decl_var_id($2), expression(expr_type::num, 0)}; }
			| "var" "identifier" "=" exp ";"	{ $$ = declaration{drv.decl_var_id($2), $4}; }
			;

%left "+" "-";
%left "*" "/";
exp			: exp "+" exp						{ $$ = expression(expr_type::add, $1, $3); }
			| exp "-" exp						{ $$ = expression(expr_type::sub, $1, $3); }
			| exp "*" exp						{ $$ = expression(expr_type::mul, $1, $3); }
			| exp "/" exp						{ $$ = expression(expr_type::div, $1, $3); }
			| "(" exp ")"						{ $$ = $2; }
			| "number"							{ $$ = expression(expr_type::num, $1); }
			| "identifier"						{ $$ = expression(expr_type::var, drv.get_var_id($1)); }
			;
%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}

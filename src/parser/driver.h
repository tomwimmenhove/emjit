#ifndef DRIVER_HH
#define DRIVER_HH

#include "parser.h"
#include "vardefs.h"

#include <string>
#include <map>
#include <iostream>
#include <memory>

// Tell Flex the lexer's prototype ...
# define YY_DECL \
	yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

class driver
{
public:
	driver ();

	std::shared_ptr<var_defs> var_scope;

	void add_function(std::shared_ptr<emjit_function>& function);
	const std::shared_ptr<emjit_function> get_function(std::string name);

	void new_var_scope() { var_scope = std::shared_ptr<var_defs>(new var_defs(var_scope)); }
	void destroy_var_scope() { var_scope->get_parent_scope(); }

	int decl_var_id(std::string identifier);
	int get_var_id(std::string identifier) const;

	// Run the parser on file f.  Return 0 on success.
	int parse (const std::string& f);
	// The name of the file being parsed.
	std::string file;
	// Whether to generate parser debug traces.
	bool trace_parsing;

	// Handling the scanner.
	void scan_begin();
	void scan_end();
	// Whether to generate scanner debug traces.
	bool trace_scanning;
	// The token's location used by the scanner.
	yy::location location;

private:
	std::map<std::string, std::shared_ptr<emjit_function>> functions;
	int var_id = 0;
};
#endif // ! DRIVER_HH

#ifndef DRIVER_HH
#define DRIVER_HH
#include <string>
#include <map>
#include "parser.h"
#include <iostream>
#include <map>

// Tell Flex the lexer's prototype ...
# define YY_DECL \
	yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing of Calc++.
class driver
{
public:
	driver ();

	std::vector<statement> statements;

	inline int get_var_id() const { return var_id; }

	int decl_var_id(std::string identifier);

	int get_var_id(std::string identifier) const;
	std::string get_var_name(int id) const;

	expression expression_result;

	// Run the parser on file F.  Return 0 on success.
	int parse (const std::string& f);
	// The name of the file being parsed.
	std::string file;
	// Whether to generate parser debug traces.
	bool trace_parsing;

	// Handling the scanner.
	void scan_begin ();
	void scan_end ();
	// Whether to generate scanner debug traces.
	bool trace_scanning;
	// The token's location used by the scanner.
	yy::location location;

private:
	int var_id = 0;
	std::map<std::string, int> var_map;
	std::map<int, std::string> rev_var_map;
};
#endif // ! DRIVER_HH

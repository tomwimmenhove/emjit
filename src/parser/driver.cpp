#include <iostream>

#include "driver.h"
#include "parser.h"

using namespace std;

driver::driver ()
  : trace_parsing (false), trace_scanning (false)
{
}

int driver::parse (const std::string &f)
{
  file = f;
  location.initialize (&file);
  scan_begin ();
  yy::parser parser (*this);
  parser.set_debug_level (trace_parsing);
  int res = parser.parse ();
  scan_end ();
  return res;
}

int driver::get_var_id(std::string identifier) const
{
	auto it = var_map.find(identifier);
	if (it != var_map.end())
		return it->second;

	throw yy::parser::syntax_error(location, "Undeclared identifier: " + identifier);
}

string driver::get_var_name(int id) const
{
	auto it = rev_var_map.find(id);
	if (it != rev_var_map.end())
		return it->second;

	throw invalid_argument("Internal error: Unknown variable id");
}

int driver::decl_var_id(std::string identifier)
{
	auto it = var_map.find(identifier);
	if (it != var_map.end())
		throw yy::parser::syntax_error(location, "Identifier: " + identifier + " already declared");
		//return it->second;

	int id = var_id++;

	var_map[identifier] = id;
	rev_var_map[id] = identifier;

	return id;
}

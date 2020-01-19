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

	cerr << location << ": Undeclared identifier '" << identifier << "'\n";

	exit(1);
	return -1;
}

string driver::get_var_name(int id) const
{
	auto it = rev_var_map.find(id);
	if (it != rev_var_map.end())
		return it->second;

	throw invalid_argument("Internal error: Unknown variable name");
}

void driver::add_decl(string identifier, const expression& exp)
{
	auto var_id = decl_var_id(identifier);

	declarations.push_back(declaration{var_id, exp});
}

int driver::decl_var_id(std::string identifier)
{
	auto it = var_map.find(identifier);
	if (it != var_map.end())
		return it->second;

	int id = var_id++;

	var_map[identifier] = id;
	rev_var_map[id] = identifier;

	return id;
}

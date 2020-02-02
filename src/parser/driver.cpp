#include <iostream>

#include "driver.h"
#include "parser.h"

using namespace std;

driver::driver ()
  : trace_parsing (false), trace_scanning (false)
{
	var_scope = std::shared_ptr<var_defs>(new var_defs());
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

void driver::add_function(emjit_function function)
{
	auto it = functions.find(function.name);
	if (it != functions.end())
	{
		throw yy::parser::syntax_error(location, "function " + function.name + " has already been declared");
	}

	functions[function.name] = function;
}

emjit_function driver::get_function(std::string name)
{
	auto it = functions.find(name);
	if (it == functions.end())
	{
		throw invalid_argument("Can not find function " + name);
	}

	return functions[name];
}

int driver::decl_var_id(string identifier)
{
	try
	{
		return var_scope->decl_var_id(identifier);
	}
	catch (const invalid_argument& e)
	{
		throw yy::parser::syntax_error(location, e.what());
	}
}

int driver::get_var_id(string identifier) const
{
	try
	{
		return var_scope->get_var_id(identifier);
	}
	catch (const invalid_argument& e)
	{
		throw yy::parser::syntax_error(location, e.what());
	}
}


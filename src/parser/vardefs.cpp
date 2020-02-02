/*
 * vardefs.cpp
 *
 *  Created on: Feb 2, 2020
 *      Author: tom
 */

#include "vardefs.h"

using namespace std;

var_defs::var_defs(std::shared_ptr<var_defs> parent_scope)
 : parent_scope(parent_scope)
{
	var_id = parent_scope ? parent_scope->get_var_id() : 0;
}

int var_defs::get_var_id(std::string identifier) const
{
	auto it = var_map.find(identifier);
	if (it != var_map.end())
		return it->second;

	if (parent_scope != nullptr)
		return parent_scope->get_var_id(identifier);

	throw invalid_argument("Undeclared identifier: " + identifier);
}

int var_defs::decl_var_id(std::string identifier)
{
	auto it = var_map.find(identifier);
	if (it != var_map.end())
		throw invalid_argument("Identifier: " + identifier + " already declared");
		//return it->second;

	int id = var_id++;

	var_map[identifier] = id;
	rev_var_map[id] = identifier;

	return id;
}

string var_defs::get_var_name(int id) const
{
	auto it = rev_var_map.find(id);
	if (it != rev_var_map.end())
		return it->second;

	if (parent_scope != nullptr)
		return parent_scope->get_var_name(id);

	throw invalid_argument("Internal error: Unknown variable id");
}

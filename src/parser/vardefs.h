/*
 * vardefs.h
 *
 *  Created on: Feb 2, 2020
 *      Author: tom
 */

#ifndef PARSER_VARDEFS_H_
#define PARSER_VARDEFS_H_

#include <string>
#include <map>
#include <memory>

class var_defs
{
public:
	var_defs(std::shared_ptr<var_defs> parent_scope = nullptr);

	std::shared_ptr<var_defs> get_parent_scope() { return parent_scope; };

	int decl_var_id(std::string identifier);
	int get_var_id(std::string identifier) const;

	inline int get_var_id() const { return var_id; }
	std::string get_var_name(int id) const;

	virtual ~var_defs() { }

private:
	std::shared_ptr<var_defs> parent_scope;
	int var_id = 0;
	std::map<std::string, int> var_map;
	std::map<int, std::string> rev_var_map;
};

#endif /* PARSER_VARDEFS_H_ */

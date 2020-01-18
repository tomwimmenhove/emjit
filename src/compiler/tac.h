/*
 * tac.h
 *
 *  Created on: Jan 9, 2020
 *      Author: tom
 */

#ifndef COMPILER_TAC_H_
#define COMPILER_TAC_H_

#include <vector>
#include <string>

#include "../parser/expression.h"

enum class tac_var_type
{
	unused,
	constant,
	temp,
	local,
	param
};

class tac_entry;

struct tac_var
{
	const int id;
	const tac_var_type type;
	const int32_t value;

	tac_var() : id(-1), type(tac_var_type::unused), value(0) { }
	tac_var(tac_var_type type, int32_t i)
	 : id(type != tac_var_type::constant ? i : -1), type(type), value(type == tac_var_type::constant ? i : 0) { }
	//tac_var(tac_var_type type, int id) : id(id), type(type), value(0) { }
	//tac_var(tac_var_type type, int id, int32_t value) : id(id), type(type), value(value) { }

	//bool will_be_read(std::vector<tac_entry>::iterator from, std::vector<tac_entry>::iterator to);

	std::string var_to_string() const;
};

enum class tac_type
{
	assign,

	add,
	sub,
	mul,
	div,
};

struct tac_entry
{
	const tac_type type;
	const tac_var a, b, c;

	tac_entry(tac_type type, tac_var a) : type(type), a(a) { }
	tac_entry(tac_type type, tac_var a, tac_var b) : type(type), a(a), b(b){ }
	tac_entry(tac_type type, tac_var a, tac_var b, tac_var c) : type(type), a(a), b(b), c(c) { }
};

class tac
{

public:
	tac(const expression& exp);

	const std::vector<tac_entry>& get_entries() const { return entries; }

	static bool will_be_read(tac_var_type type, int id, std::vector<tac_entry>::iterator from, std::vector<tac_entry>::iterator to);

	void calculate_life_times();

	void experiments();
	void debug_print();

	virtual ~tac();

private:
	int add_from_exp(const expression& exp);

	int next_varid = 0;
	std::vector<tac_entry> entries;
};

#endif /* COMPILER_TAC_H_ */

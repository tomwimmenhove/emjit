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
	int id;
	tac_var_type type;
	int32_t value;

	tac_var() : id(-1), type(tac_var_type::unused), value(0) { }
	tac_var(const tac_var& other) : id(other.id), type(other.type), value(other.value) { }
	tac_var(tac_var_type type, int32_t i)
	 : id(type != tac_var_type::constant ? i : -1), type(type), value(type == tac_var_type::constant ? i : 0) { }

	inline bool operator==(const tac_var& other) const { return cmp(other) == 0; }
	inline bool operator!=(const tac_var& other) const { return cmp(other) != 0; }
	inline bool operator> (const tac_var& other) const { return cmp(other) >  0; }
	inline bool operator< (const tac_var& other) const { return cmp(other) <  0; }

private:
	inline int cmp(const tac_var& other) const
	{
		if (id > other.id) return 1;
		if (id < other.id) return -1;
		if (type > other.type) return 1;
		if (type < other.type) return -1;
		if (value > other.value) return 1;
		if (value < other.value) return -1;
		return 0;
	}
};

enum class tac_type
{
	assign,

	add,
	sub,
	mul,
	div,

	ret,
};

struct tac_entry
{
	const tac_type type;
	const tac_var a, b, c;

	tac_entry(tac_type type, tac_var a) : type(type), a(a) { }
	tac_entry(tac_type type, tac_var a, tac_var b) : type(type), a(a), b(b){ }
	tac_entry(tac_type type, tac_var a, tac_var b, tac_var c) : type(type), a(a), b(b), c(c) { }

	std::vector<bool> live_vars;
};

class tac
{
public:
	tac(const emjit_function& func);

	const std::vector<tac_entry>& get_entries() const { return entries; }

	std::string var_to_string(const tac_var& var) const;

	inline int get_num_user_vars() const { return func.var_scope->get_var_id(); }
	inline int get_num_vars() const { return next_varid; }
	inline std::string get_var_name(int id) const { return id < func.var_scope->get_var_id() ? func.var_scope->get_var_name(id) : "t" + std::to_string(id); }

	void calculate_life_times();

	void experiments();
	void debug_print();

	virtual ~tac();

private:
	const emjit_function& func;

	tac_var add_from_exp(const tac_var& result, const expression& exp);
	void add_live_range(int id, int from, int to);

	int next_varid = 0;
	std::vector<tac_entry> entries;
};

#endif /* COMPILER_TAC_H_ */

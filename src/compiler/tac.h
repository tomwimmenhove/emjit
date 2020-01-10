/*
 * tac.h
 *
 *  Created on: Jan 9, 2020
 *      Author: tom
 */

#ifndef COMPILER_TAC_H_
#define COMPILER_TAC_H_

#include <vector>

#include "../parser/expression.h"

enum class tac_type
{
	assign,

	add,
	sub,
	mul,
	div,
};

struct tac_var
{
	const int id;
	const bool is_constant;
	const int32_t value;

	tac_var() : id(-1), is_constant(false), value(0) { }
	tac_var(int id) : id(id), is_constant(false), value(0) { }
	tac_var(int id, int32_t value) : id(id), is_constant(true), value(value) { }
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

	virtual ~tac();

private:
	int add_from_exp(const expression& exp);

	int next_varid = 0;
	std::vector<tac_entry> entries;
};

#endif /* COMPILER_TAC_H_ */

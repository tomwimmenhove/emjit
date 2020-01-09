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
	int id;
};

struct tac_entry
{
	tac_type type;

	std::vector<tac_var> args;
};

class tac
{

public:
	tac(const expression& exp);
	virtual ~tac();

private:
	int add_from_exp(const expression& exp);

	int next_varid = 0;
};

#endif /* COMPILER_TAC_H_ */

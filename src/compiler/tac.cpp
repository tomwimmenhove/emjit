/*
 * tac.cpp
 *
 *  Created on: Jan 9, 2020
 *      Author: tom
 */

#include <iostream>
#include <string>

#include "tac.h"

using namespace std;

tac::tac(const expression& exp)
{
	add_from_exp(exp);
}

int tac::add_from_exp(const expression& exp)
{
	int a, b;

	if (exp.type != expr_type::num)
	{
		a = add_from_exp(exp.expressions[0]);
		b = add_from_exp(exp.expressions[1]);
	}

	int varid = next_varid++;

	cout << "t" << varid << " = ";

	switch(exp.type)
	{
	case expr_type::num:
		entries.push_back(tac_entry(tac_type::assign, tac_var(varid, exp.num)));
		cout << exp.num << '\n';
		break;

	case expr_type::add:
		entries.push_back(tac_entry(tac_type::add, tac_var(a), tac_var(b)));
		cout << 't' << a << " + " << 't' << b << '\n';
		break;
	case expr_type::sub:
		entries.push_back(tac_entry(tac_type::sub, tac_var(a), tac_var(b)));
		cout << 't' << a << " - " << 't' << b << '\n';
		break;
	case expr_type::mul:
		entries.push_back(tac_entry(tac_type::mul, tac_var(a), tac_var(b)));
		cout << 't' << a << " * " << 't' << b << '\n';
		break;
	case expr_type::div:
		entries.push_back(tac_entry(tac_type::div, tac_var(a), tac_var(b)));
		cout << 't' << a << " / " << 't' << b << '\n';
		break;

	case expr_type::invalid: throw invalid_argument("Internal error: Invalid expression.");
	}

	return varid;
}


tac::~tac()
{
	// TODO Auto-generated destructor stub
}


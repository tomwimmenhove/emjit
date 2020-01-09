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
		cout << exp.num << '\n';
		break;

	case expr_type::add:
		cout << 't' << a << " + " << 't' << b << '\n';
		break;
	case expr_type::sub:
		cout << 't' << a << " - " << 't' << b << '\n';
		break;
	case expr_type::mul:
		cout << 't' << a << " * " << 't' << b << '\n';
		break;
	case expr_type::div:
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


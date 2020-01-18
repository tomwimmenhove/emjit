/*
 * tac.cpp
 *
 *  Created on: Jan 9, 2020
 *      Author: tom
 */

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>

#include "tac.h"

using namespace std;

tac::tac(const expression& exp)
{
	experiments();
	//add_from_exp(exp);
	debug_print();

	calculate_life_times();
}

void tac::calculate_life_times()
{
	int* last_write = static_cast<int*>(alloca(next_varid * sizeof(int)));
	int* last_read = static_cast<int*>(alloca(next_varid * sizeof(int)));
	memset(last_write, 0xff, next_varid * sizeof(int));
	memset(last_read, 0xff, next_varid * sizeof(int));

	//for(auto& entry: entries)
	for(size_t i = 0; i < entries.size(); i++)
	{
		auto& entry = entries[i];

		if (entry.a.type == tac_var_type::temp)
		{
			if (last_read[entry.a.id] != -1)
			{
				cout << "Life time of t" << entry.a.id << ": " << last_write[entry.a.id] << ".." << last_read[entry.a.id] << '\n';
				last_read[entry.a.id] = -1;
			}
			last_write[entry.a.id] = i;
			continue;
		}

		if (entry.b.type == tac_var_type::temp)
			last_read[entry.b.id] = i;

		if (entry.c.type == tac_var_type::temp)
			last_read[entry.c.id] = i;
	}

	for(auto i = 0; i < next_varid; i++)
	{
		if (last_read[i] != -1)
		{
			cout << "Life time of t" << i << ": " << last_write[i] << ".." << last_read[i] << '\n';
			last_read[i] = -1;
		}
	}
}

//bool tac_var::will_be_read(vector<tac_entry>::iterator from, vector<tac_entry>::iterator to)
//{
//	for(auto it = from; it != to; ++it)
//	{
//		if ((it->b.type == type && it->b.id == id) ||
//				(it->c.type == type && it->c.id == id))
//			return true;
//	}
//
//	return false;
//}

string tac_var::var_to_string() const
{
	string s;

	switch(type)
	{
	case tac_var_type::constant: return to_string(value);
	case tac_var_type::local: s = "l"; break;
	case tac_var_type::param: s = "p"; break;
	case tac_var_type::temp: s = "t"; break;
	case tac_var_type::unused: return "unused";
	}

	s += to_string(id);

	return s;
}

void tac::debug_print()
{
	//for(auto& entry: entries)
	for (size_t i = 0; i < entries.size(); i++)
	{
		auto& entry = entries[i];
//		if (entry.a.id != -1) cout << "Wt" << entry.a.id << ' ';
//		if (entry.b.id != -1) cout << "Rt" << entry.b.id << ' ';
//		if (entry.c.id != -1) cout << "Rt" << entry.c.id << ' ';

		cout << '@' << i << ": " << entry.a.var_to_string() << " = ";

		switch(entry.type)
		{
		case tac_type::assign:
			cout << entry.b.var_to_string() << '\n';
			break;
		case tac_type::add:
			cout << entry.b.var_to_string() << " + " << entry.c.var_to_string() << '\n';
			break;
		case tac_type::sub:
			cout << entry.b.var_to_string() << " - " << entry.c.var_to_string() << '\n';
			break;
		case tac_type::mul:
			cout << entry.b.var_to_string() << " * " << entry.c.var_to_string() << '\n';
			break;
		case tac_type::div:
			cout << entry.b.var_to_string() << " / " << entry.c.var_to_string() << '\n';
			break;
		}
	}
}

void tac::experiments()
{
//	entries.push_back(tac_entry(tac_type::assign, tac_var(tac_var_type::temp, 0), tac_var(tac_var_type::constant, 0x100)));
//	entries.push_back(tac_entry(tac_type::assign, tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::constant, 0x200)));
//	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 2), tac_var(tac_var_type::temp, 0), tac_var(tac_var_type::temp, 1)));
//	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 3), tac_var(tac_var_type::temp, 2), tac_var(tac_var_type::temp, 1)));

	entries.push_back(tac_entry(tac_type::assign, tac_var(tac_var_type::temp, 0), tac_var(tac_var_type::constant, 0x100)));
	entries.push_back(tac_entry(tac_type::assign, tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::constant, 0x200)));
	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 2), tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::temp, 0)));
	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 3), tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::temp, 2)));



	next_varid = 100;
}

int tac::add_from_exp(const expression& exp)
{
	int b, c;

	if (exp.type != expr_type::num)
	{
		b = add_from_exp(exp.expressions[0]);
		c = add_from_exp(exp.expressions[1]);
	}

	int a = next_varid++;
	switch(exp.type)
	{
	case expr_type::num:
		entries.push_back(tac_entry(tac_type::assign, tac_var(tac_var_type::temp, a), tac_var(tac_var_type::constant, exp.num)));
		break;
	case expr_type::add:
		entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, a), tac_var(tac_var_type::temp, b), tac_var(tac_var_type::temp, c)));
		break;
	case expr_type::sub:
		entries.push_back(tac_entry(tac_type::sub, tac_var(tac_var_type::temp, a), tac_var(tac_var_type::temp, b), tac_var(tac_var_type::temp, c)));
		break;
	case expr_type::mul:
		entries.push_back(tac_entry(tac_type::mul, tac_var(tac_var_type::temp, a), tac_var(tac_var_type::temp, b), tac_var(tac_var_type::temp, c)));
		break;
	case expr_type::div:
		entries.push_back(tac_entry(tac_type::div, tac_var(tac_var_type::temp, a), tac_var(tac_var_type::temp, b), tac_var(tac_var_type::temp, c)));
		break;

	case expr_type::invalid: throw invalid_argument("Internal error: Invalid expression.");
	}

	return a;
}


tac::~tac()
{
	// TODO Auto-generated destructor stub
}


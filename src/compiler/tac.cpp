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

tac::tac(const driver& drv)
 :drv(drv)
{
	//experiments();
	//int id = add_from_exp(drv.declarations[0].exp);

//	int id = 0;
	for(auto& decl: drv.declarations)
	{
//		if (decl.result_var != -1 && decl.exp.type == expr_type::num)
//		{
//			entries.push_back(tac_entry(tac_type::assign,
//					tac_var(tac_var_type::local, decl.result_var),
//					tac_var(tac_var_type::constant, decl.exp.num)));
//			continue;
//		}
		add_from_exp(tac_var(tac_var_type::local, decl.result_var), decl.exp);
//
//		if (decl.result_var != -1)
//			entries.push_back(tac_entry(tac_type::assign,
//					tac_var(tac_var_type::local, decl.result_var),
//					tac_var(tac_var_type::temp, id)));
	}

	// HACK!
//	entries.push_back(tac_entry(tac_type::ret, tac_var(/*void*/), tac_var(tac_var_type::temp, id)));

	debug_print();

	calculate_life_times();
}

void tac::add_live_range(int id, int from, int to)
{
	if (from == -1)
	{
		from = 0;
		cerr << "Warning: varid" << id << " is being used uninitialized.\n";
	}
	cout << "Life time of t" << id << ": " << from << ".." << to << '\n';

	for (int i = from; i < to; i++)
		entries[i].live_vars[id] = true;
}

void tac::calculate_life_times()
{
	int* last_write = static_cast<int*>(alloca(next_varid * sizeof(int)));
	int* last_read = static_cast<int*>(alloca(next_varid * sizeof(int)));
	memset(last_write, 0xff, next_varid * sizeof(int));
	memset(last_read, 0xff, next_varid * sizeof(int));

	for(size_t i = 0; i < entries.size(); i++)
	{
		auto& entry = entries[i];

		entry.live_vars.resize(next_varid, false);

		//if (entry.a.type == tac_var_type::temp)
		if (entry.a.id != -1)
		{
			if (last_read[entry.a.id] != -1)
			{
				add_live_range(i, last_write[entry.a.id] + 1, last_read[entry.a.id]);
				last_read[entry.a.id] = -1;
			}
			last_write[entry.a.id] = i;
			//continue;
		}

		//if (entry.b.type == tac_var_type::temp)
		if (entry.b.id != -1)
			last_read[entry.b.id] = i;

		//if (entry.c.type == tac_var_type::temp)
		if (entry.c.id != -1)
			last_read[entry.c.id] = i;
	}

	for(auto i = 0; i < next_varid; i++)
	{
		if (last_read[i] != -1)
		{
			add_live_range(i, last_write[i] + 1, last_read[i]);
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

string tac::var_to_string(const tac_var& var) const
{
	string s;

	switch(var.type)
	{
	case tac_var_type::constant: return to_string(var.value);
	case tac_var_type::local: return drv.get_var_name(var.id);
	case tac_var_type::param: s = "p"; break;
	case tac_var_type::temp: s = "t"; break;
	case tac_var_type::unused: return "unused";
	}

	s += to_string(var.id);

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

		cout << '@' << i << ": ";

		switch(entry.type)
		{
		case tac_type::assign:
			cout << var_to_string(entry.a) << " = " << var_to_string(entry.b) << '\n';
			break;
		case tac_type::add:
			cout << var_to_string(entry.a) << " = " << var_to_string(entry.b) << " + " << var_to_string(entry.c) << '\n';
			break;
		case tac_type::sub:
			cout << var_to_string(entry.a) << " = " << var_to_string(entry.b) << " - " << var_to_string(entry.c) << '\n';
			break;
		case tac_type::mul:
			cout << var_to_string(entry.a) << " = " << var_to_string(entry.b) << " * " << var_to_string(entry.c) << '\n';
			break;
		case tac_type::div:
			cout << var_to_string(entry.a) << " = " << var_to_string(entry.b) << " / " << var_to_string(entry.c) << '\n';
			break;

		case tac_type::ret:
			cout << "return " << var_to_string(entry.b) << '\n';
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

	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 4), tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::temp, 2)));

	entries.push_back(tac_entry(tac_type::ret, tac_var(/*void*/), tac_var(tac_var_type::temp, 4)));



	next_varid = 100;
}

void tac::add_from_exp(const tac_var& result, const expression& exp)
{
	if (exp.type == expr_type::invalid)
		throw invalid_argument("Internal error: invalid expression type");

	tac_var b, c;

	if (exp.type != expr_type::num && exp.type != expr_type::var)
	{
		b = tac_var(tac_var_type::temp, next_varid++);
		c = tac_var(tac_var_type::temp, next_varid++);

		add_from_exp(b, exp.expressions[0]);
		add_from_exp(c, exp.expressions[1]);
	}

	if (exp.type == expr_type::var)
		cout << "Got a var\n";

	//int a = next_varid++;
	//tac_var a;
	switch(exp.type)
	{
	case expr_type::var:
		entries.push_back(tac_entry(tac_type::assign, result, tac_var(tac_var_type::local, exp.num)));
		//throw invalid_argument("Not implemented yet");

		break;
	case expr_type::num:
		entries.push_back(tac_entry(tac_type::assign, result, tac_var(tac_var_type::constant, exp.num)));
		break;
	case expr_type::add:
		entries.push_back(tac_entry(tac_type::add, result, b, c));
		break;
	case expr_type::sub:
		entries.push_back(tac_entry(tac_type::sub, result, b, c));
		break;
	case expr_type::mul:
		entries.push_back(tac_entry(tac_type::mul, result, b, c));
		break;
	case expr_type::div:
		entries.push_back(tac_entry(tac_type::div, result, b, c));
		break;

	case expr_type::invalid: throw invalid_argument("Internal error: Invalid expression.");
	}
}


tac::~tac()
{
	// TODO Auto-generated destructor stub
}


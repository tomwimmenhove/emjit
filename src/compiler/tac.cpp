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
#if 1
	next_varid = drv.get_var_id();

	for(auto& stmt: drv.statements)
	{
		switch(stmt.type)
		{
		case statement_type::decl:
			add_from_exp(tac_var(tac_var_type::local, stmt.decl.result_var), stmt.decl.exp);
			break;

		case statement_type::ret:
			auto ret_exp = add_from_exp(tac_var(tac_var_type::temp, -1), stmt.ret_exp);
			entries.push_back(tac_entry(tac_type::ret, tac_var(/* void */), ret_exp));
			break;
		}
	}
#else
	experiments();
#endif

	debug_print();
	calculate_life_times();
	rig_generate();
	rig_debug_print();

	cout << "---XXXX Removing ID 4 and 5! XXXX----\n";
	rig_push_stack(4);
	rig_push_stack(5);

	rig_debug_print();
	cout << "---XXXX Restoring ID 4 and 5! XXXX----\n";
	rig_pop_stack();
	rig_pop_stack();

	rig_debug_print();
}

void tac::add_live_range(int id, int from, int to)
{
	if (from == -1)
	{
		from = 0;
		cerr << "Warning: varid" << id << " is being used uninitialized.\n";
	}
	cout << "Life time of t" << id << ": " << from << ".." << to << '\n';

	for (int i = from; i < to + 1; i++)
		entries[i].live_vars[id] = true;
}

//#include <list>
//#include <stack>
//
//struct tac_rig_node
//{
//	int id;
//
//	std::list<tac_rig_node> nodes;
//};
//
//struct tac_rig
//{
//	std::list<tac_rig_node> nodes;
//	std::stack<tac_rig_node> node_stack;
//
//
//};


void tac::calculate_life_times()
{
	vector<int> last_read(next_varid, -1);

	for(ssize_t i = static_cast<ssize_t>(entries.size()) - 1; i >= 0; i--)
	{
		auto& entry = entries[i];

		entry.live_vars.resize(next_varid, false);

		if (entry.a.id != -1 && last_read[entry.a.id] != -1)
		{
			add_live_range(entry.a.id, i + 1, last_read[entry.a.id]);
			last_read[entry.a.id] = -1;
		}

		if (entry.b.id != -1 && last_read[entry.b.id] == -1)
			last_read[entry.b.id] = i;

		if (entry.c.id != -1 && last_read[entry.c.id] == -1)
			last_read[entry.c.id] = i;
	}
}

void tac::rig_generate()
{
	rig.resize(next_varid);
	for (auto i = 0; i < next_varid; i++)
	{
		auto& node = rig[i];

		node.resize(next_varid, false);
		for (auto& entry: entries)
		{
			if (entry.live_vars[i])
			{
				for (auto j = 0; j < next_varid; j++)
				{
					if (j == i)
						continue;

					if (entry.live_vars[j])
						node[j] = true;
				}
			}
		}
	}

	rig_stack.reserve(next_varid);
	rig_stack_set.resize(next_varid, false);
	rig_stack.clear();
}

void tac::rig_push_stack(int id)
{
	rig_stack.push_back(id);
	rig_stack_set[id] = true;
}

int tac::rig_pop_stack()
{
	int id = rig_stack.back();
	rig_stack.pop_back();
	rig_stack_set[id] = false;

	return id;
}

bool tac::rig_interferes(int id1, int id2)
{
	return !rig_stack_set[id1] && !rig_stack_set[id2] && rig[id1][id2];
}

int tac::rig_interference_nodes(int id)
{
	if (rig_stack_set[id])
		return 0;

	int cnt = 0;
	auto& node = rig[id];
	for (auto i = 0; i < next_varid; i++)
	{
		if (rig_stack_set[i])
			continue;

		if (node[i])
			cnt++;
	}

	return cnt;
}

void tac::rig_debug_print()
{
	for (auto i = 0; i < next_varid; i++)
	{
		int n = rig_interference_nodes(i);
		cout << "Var id " << i << " interferes with " << n << " nodes: ";
		for (auto j = 0; j < next_varid; j++)
			if (rig_interferes(i, j))
				cout << j << ", ";
		cout << '\n';
	}
}

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

		//cout << '@' << i << ": ";

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

	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 4), tac_var(tac_var_type::temp, 1), tac_var(tac_var_type::temp, 3)));


	entries.push_back(tac_entry(tac_type::add, tac_var(tac_var_type::temp, 5), tac_var(tac_var_type::temp, 0), tac_var(tac_var_type::temp, 3)));
	entries.push_back(tac_entry(tac_type::ret, tac_var(/*void*/), tac_var(tac_var_type::temp, 4)));


	next_varid = 6;
}

tac_var tac::add_from_exp(const tac_var& result, const expression& exp)
{
	if (exp.type == expr_type::invalid)
		throw invalid_argument("Internal error: invalid expression type");

	/* Are we placing it in a temporary, or a variable? */
	auto r = exp.type != expr_type::var && result.id == -1 ?
			tac_var(tac_var_type::temp, next_varid++) :
			result;

	switch(exp.type)
	{
	case expr_type::var:
		return tac_var(tac_var_type::local, exp.num);
	case expr_type::num:
		entries.push_back(tac_entry(tac_type::assign, r,
				tac_var(tac_var_type::constant, exp.num)));
		break;
	case expr_type::add:
		entries.push_back(tac_entry(tac_type::add, r,
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[0]),
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[1])));
		break;
	case expr_type::sub:
		entries.push_back(tac_entry(tac_type::sub, r,
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[0]),
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[1])));
		break;
	case expr_type::mul:
		entries.push_back(tac_entry(tac_type::mul, r,
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[0]),
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[1])));
		break;
	case expr_type::div:
		entries.push_back(tac_entry(tac_type::div, r,
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[0]),
				add_from_exp(tac_var(tac_var_type::temp, -1), exp.expressions[1])));
		break;

	case expr_type::invalid: throw invalid_argument("Internal error: Invalid expression.");
	}

	return r;
}


tac::~tac()
{
	// TODO Auto-generated destructor stub
}


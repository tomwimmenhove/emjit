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
#include <unordered_set>

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

//	cout << "---XXXX Removing ID 1 XXXX----\n";
//	rig_push_stack(1);
//	rig_debug_print();


//	cout << "---XXXX Removing ID 4 and 5! XXXX----\n";
//	rig_push_stack(4);
//	rig_push_stack(5);
//
//	rig_debug_print();
//	cout << "---XXXX Restoring ID 4 and 5! XXXX----\n";
//	rig_pop_stack();
//	rig_pop_stack();


	auto m = rig_color(7);


	//rig_debug_print();
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
	rig_removed.resize(next_varid, false);
	rig_stack.clear();
}

void tac::rig_push_stack(int id)
{
	rig_stack.push_back(id);
	rig_removed[id] = true;
}

int tac::rig_pop_stack()
{
	if (rig_stack.empty())
			return -1;

	int id = rig_stack.back();
	rig_stack.pop_back();
	rig_removed[id] = false;

	return id;
}

int tac::rig_find_lt_k(int k)
{
	for (auto i = 0; i < next_varid; i++)
	{
		if (rig_removed[i])
			continue;

		if (rig_interference_nodes(i) < k)
			return i;
	}

	return -1;
}

int tac::rig_try_add_reuse(map<int, int>& reg_map, int id, int max_color)
{
	vector<bool> can_use(max_color, true);

	/* Mark all colors that we can't use because interfering variables already use the color */
	for (auto i = 0; i < next_varid; i++)
	{
		if (id == i)
			continue;

		if (rig_interferes(id, i))
		{
			auto it = reg_map.find(i);
			if (it == reg_map.end())
				throw invalid_argument("Internal error: Can't find id in reg_map! << REMOVE ME!");

			int color = reg_map[i];
			if (color >= 0) /* Don't worry about already-spilled variables */
				can_use[color] = false;
		}
	}

	/* Find if there's anything left that we can use */
	for (auto i = 0; i < max_color; i++)
		if (can_use[i])
			return i;

	return -1;
}

map<int, int> tac::rig_color(int k)
{
	map<int, int> reg_map;

	unordered_set<int> spills;

	/* Stage 1: removing nodes */
	while (true)
	{
		int found;
		int left;

		/* Remove nodes with <k edges */
		do
		{
			rig_debug_print();

			found = left = 0;
			for (auto i = 0; i < next_varid; i++)
			{
				if (rig_interference_nodes_lt(i, k))
				{
					cout << "Removing " << i << '\n';
					rig_push_stack(i);
					found++;
				}

				if (!rig_removed[i])
					left++;
			}
		} while (found);

		/* Anything left? */
		if (!left)
			break;

		/* Ok, potential spilling */
		int max = 0;
		int maxid = -1;
		for (auto i = 0; i < next_varid; i++)
		{
			/* Find the node with the most edges. XXX: This should be optimized! */
			int n = rig_interference_nodes(i);
			if (n > max)
			{
				max = n;
				maxid = i;
			}
		}

		if (maxid == -1)
			throw invalid_argument("Internal error: RIG coloring error");

		rig_removed[maxid] = true;
		spills.insert(maxid);
		cout << "Possibly spilling " << maxid << '\n';
	}

	/* Stage 2: coloring */
	int color = 0;
	while (true)
	{
		/* Pop nodes back from the stack */
		int id = rig_pop_stack();
		if (id == -1)
			break;

		cout << "Popping back " << id << '\n';

		/* Try to assign existing color, otherwise assign a new one */
		int c = rig_try_add_reuse(reg_map, id, color);
		if (c != -1)
			reg_map[id] = c;
		else
			reg_map[id] = color++;
	}

	/* Optimistically try to assign existing color to possibly spilled variables,
	 * otherwise assign a spill (negative) value. */
	int used_colors = color;
	color = 0;
	for(auto id: spills)
	{
		rig_removed[id] = false;

		cout << "Trying to assign color to id " << id << '\n';
		rig_debug_print();

		int c = rig_try_add_reuse(reg_map, id, used_colors);
		if (c != -1)
		{
			cout << "Was able to assign color " << c << " to id " << id << '\n';
			reg_map[id] = c;
		}
		else
			reg_map[id] = --color;
	}

	for(auto it = reg_map.begin(); it != reg_map.end(); ++it)
		cout << "Mapping: " << it->first << ": " << it->second << '\n';

	cout << "DONE\n";

	rig_debug_print();

	return reg_map;
}


bool tac::rig_interferes(int id1, int id2)
{
	return !rig_removed[id1] && !rig_removed[id2] && rig[id1][id2];
}

int tac::rig_interference_nodes(int id)
{
	if (rig_removed[id])
		return 0;

	int cnt = 0;
	auto& node = rig[id];
	for (auto i = 0; i < next_varid; i++)
	{
		if (rig_removed[i])
			continue;

		if (node[i])
			cnt++;
	}

	return cnt;
}

bool tac::rig_interference_nodes_lt(int id, int lt)
{
	if (rig_removed[id])
		return false;

	int cnt = 0;
	auto& node = rig[id];
	for (auto i = 0; i < next_varid; i++)
	{
		if (rig_removed[i])
			continue;

		if (node[i])
			if (++cnt >= lt)
				return false;
	}

	return true;
}

void tac::rig_debug_print()
{
	for (auto i = 0; i < next_varid; i++)
	{
		if (rig_removed[i])
			continue;

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
	case tac_var_type::local: return drv.get_var_name(var.id);// + '(' + to_string(var.id) + ')';
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


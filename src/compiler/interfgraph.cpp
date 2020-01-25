/*
 * rig.cpp
 *
 *  Created on: Jan 24, 2020
 *      Author: tom
 */

#include "interfgraph.h"

#include <iostream>
#include <unordered_set>

using namespace std;

void interf_graph::generate(const std::vector<tac_entry>& entries)
{
	graph.resize(n_vars);
	for (auto i = 0; i < n_vars; i++)
	{
		auto& node = graph[i];

		node.resize(n_vars, false);
		for (auto& entry: entries)
		{
			if (entry.live_vars[i])
			{
				for (auto j = 0; j < n_vars; j++)
				{
					if (j == i)
						continue;

					if (entry.live_vars[j])
						node[j] = true;
				}
			}
		}
	}

	stack.reserve(n_vars);
	removed.resize(n_vars, false);
	stack.clear();
}

void interf_graph::push_stack(int id)
{
	stack.push_back(id);
	removed[id] = true;
}

int interf_graph::pop_stack()
{
	if (stack.empty())
			return -1;

	int id = stack.back();
	stack.pop_back();
	removed[id] = false;

	return id;
}

int interf_graph::find_lt_nodes(int nodes)
{
	for (auto i = 0; i < n_vars; i++)
	{
		if (removed[i])
			continue;

		if (interference_nodes(i) < nodes)
			return i;
	}

	return -1;
}

int interf_graph::try_add_reuse(map<int, int>& color_map, int id, int max_color)
{
	vector<bool> can_use(max_color, true);

	/* Mark all colors that we can't use because interfering variables already use the color */
	for (auto i = 0; i < n_vars; i++)
	{
		if (id != i && !removed[i] && graph[id][i])
		{
			auto it = color_map.find(i);
			if (it == color_map.end())
				throw invalid_argument("Internal error: RIG: Can't find id in color_map!");

			int color = it->second;
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

map<int, int> interf_graph::color(int k)
{
	if (!k)
		throw invalid_argument("Internal error: k must be > 0 for graph coloring");

	map<int, int> color_map;

	unordered_set<int> spills;

	/* Stage 1: removing nodes with <k edges */
	while (true)
	{
		int found;
		int left;

		do
		{
			found = left = 0;
			for (auto i = 0; i < n_vars; i++)
			{
				if (interference_nodes_lt(i, k))
				{
					push_stack(i);
					found++;
				}

				if (!removed[i])
					left++;
			}
		} while (found);

		/* Anything left? */
		if (!left)
			break;

		/* Ok, potential spilling */
		int max = 0;
		int maxid = -1;
		for (auto i = 0; i < n_vars; i++)
		{
			/* Find the node with the most edges. XXX: This should be optimized! */
			int n = interference_nodes(i);
			if (n > max)
			{
				max = n;
				maxid = i;
			}
		}

		if (maxid == -1)
			throw invalid_argument("Internal error: RIG coloring error");

		removed[maxid] = true;
		spills.insert(maxid);
		cout << "Possibly spilling " << maxid << '\n';
	}

	/* Stage 2: coloring */
	int color = 0;
	while (true)
	{
		/* Pop nodes back from the stack */
		int id = pop_stack();
		if (id == -1)
			break;

		/* Try to assign existing color, otherwise assign a new one */
		int c = try_add_reuse(color_map, id, color);
		if (c != -1)
			color_map[id] = c;
		else
			color_map[id] = color++;
	}

	/* Optimistically try to assign existing color to possibly spilled variables,
	 * otherwise assign a spill (negative) value. */
	//int used_colors = color;
	color = 0;
	for(auto id: spills)
	{
		removed[id] = false;

		//int c = try_add_reuse(color_map, id, used_colors);
		int c = try_add_reuse(color_map, id, k);
		if (c != -1)
		{
			color_map[id] = c;
			cout << "Was able to assign color " << c << " to id " << id << '\n';
		}
		else
		{
			color_map[id] = --color;
			cout << "Spilled " << id << " to " << color << '\n';
		}
	}

	n_spills = -color;

	return color_map;
}

int interf_graph::interference_nodes(int id)
{
	if (removed[id])
		return 0;

	int cnt = 0;
	auto& node = graph[id];
	for (auto i = 0; i < n_vars; i++)
	{
		if (removed[i])
			continue;

		if (node[i])
			cnt++;
	}

	return cnt;
}

bool interf_graph::interference_nodes_lt(int id, int lt)
{
	if (removed[id])
		return false;

	int cnt = 0;
	auto& node = graph[id];
	for (auto i = 0; i < n_vars; i++)
	{
		if (removed[i])
			continue;

		if (node[i])
			if (++cnt >= lt)
				return false;
	}

	return true;
}

void interf_graph::debug_print()
{
	for (auto i = 0; i < n_vars; i++)
	{
		if (removed[i])
			continue;

		int n = interference_nodes(i);
		cout << "Var id " << i << " interferes with " << n << " nodes: ";
		for (auto j = 0; j < n_vars; j++)
			if (!removed[j] && graph[i][j])
				cout << j << ", ";
		cout << '\n';
	}
}


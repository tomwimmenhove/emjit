/*
 * rig.h
 *
 *  Created on: Jan 24, 2020
 *      Author: tom
 */

#ifndef COMPILER_INTERFGRAPH_H_
#define COMPILER_INTERFGRAPH_H_

#include <map>
#include <vector>

#include "tac.h"

class tac_entry;

class interf_graph
{
public:
	interf_graph() : n_vars(0) { }
	interf_graph(int n_vars) : n_vars(n_vars) { }

	inline int get_n_vars() const { return n_vars; }
	inline void set_n_vars(int n) { n_vars = n; }

	void generate(const std::vector<tac_entry>& entries);
	void debug_print();

	std::map<int, int> color(int k);

	virtual ~interf_graph() { }

private:
	void push_stack(int id);
	int pop_stack();
	int find_lt_nodes(int nodes);
	int try_add_reuse(std::map<int, int>& color_map, int id, int max_color);
	bool interferes(int id1, int id2);
	int interference_nodes(int id);
	bool interference_nodes_lt(int id, int lt);

	std::vector<std::vector<bool>> graph; /* Register interference graph */
	std::vector<int> stack;
	std::vector<bool> removed;

	int n_vars;
};

#endif /* COMPILER_INTERFGRAPH_H_ */

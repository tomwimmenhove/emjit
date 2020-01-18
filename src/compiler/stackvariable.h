/*
 * stackvariable.h
 *
 *  Created on: Jan 15, 2020
 *      Author: tom
 */

#ifndef COMPILER_STACKVARIABLE_H_
#define COMPILER_STACKVARIABLE_H_

#include <map>

class stack_variable
{
public:
	stack_variable() { }

	int alloc(int size);
	template<typename T> inline int alloc() { return alloc(sizeof(T)); }
	inline void free(int pos) { stack_map.erase(pos); }
	inline int highest() { return max; }

	virtual ~stack_variable() { }

private:
	std::map<int, int> stack_map;
	int max = 0;
};

#endif /* COMPILER_STACKVARIABLE_H_ */

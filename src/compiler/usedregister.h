/*
 * usedregister.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_USEDREGISTER_H_
#define COMPILER_USEDREGISTER_H_

#include <vector>
#include <algorithm>

class used_register
{
public:
	used_register(int n)
	{
		used.resize(n);
	}

	used_register(int n, std::initializer_list<int> dont_use)
	 : used_register(n)
	{
		/* Mark the entries in the dont_use list as used, so we can't use them. */
		for(int x: dont_use)
			use(x);
	}

	inline void use(int reg) { used[reg] = true; }
	inline void release(int reg) { used[reg] = false; }
	inline bool is_used(int reg) { return used[reg]; }

	int get();

	virtual ~used_register() { }

private:
	std::vector<bool> used;
};

#endif /* COMPILER_USEDREGISTER_H_ */

/*
 * usedregister.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_USEDREGISTER_H_
#define COMPILER_USEDREGISTER_H_

#include <vector>
#include <array>
#include <algorithm>

template<typename T>
class used_register
{
public:
	used_register(int n)
	{
		used.resize(n);
		clobbered.resize(n);
		illegal.resize(n);
	}

	used_register(int n, std::initializer_list<T> dont_use)
	 : used_register(n)
	{
		for(const T& x: dont_use)
			illegal[x.value] = true;
	}

	inline void use(T reg) { used[reg.value] = clobbered[reg.value] = true; }
	inline void release(T reg) { used[reg.value] = false; }
	inline bool is_used(T reg) { return used[reg.value]; }
	inline bool is_clobbered(T reg) { return clobbered[reg.value]; }
	inline bool is_illegal(T reg) { return illegal[reg.value]; }
	inline bool is_free(T reg) { return !is_used(reg) && !is_illegal(reg); }

	T get(std::initializer_list<T> exclude = {})
	{
		for (int i = 0; i < (int) used.size(); i++)
		{
			if (is_free(i))
			{
				if (is_exluced(exclude, T(i)))
					continue;

				use(T(i));
				return T(i);
			}
		}

		return T(-1);
	}

	virtual ~used_register() { }

	static bool is_exluced(std::initializer_list<T> exclude, T reg)
	{
		for (auto ex: exclude)
		{
			if (reg.value == ex.value)
			{
				return true;
			}
		}

		return false;
	}

private:
	std::vector<bool> used;
	std::vector<bool> clobbered;
	std::vector<bool> illegal;
};

#endif /* COMPILER_USEDREGISTER_H_ */

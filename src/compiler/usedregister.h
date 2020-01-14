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

#include <iostream>

template<typename T>
class locked_registers
{
public:
	locked_registers(int n)
	{
		locks.resize(n, 0);
	}

	locked_registers(int n, std::initializer_list<T> locks)
	 : locked_registers(n)
	{
		for(const T& x: locks)
			this->locks[x.value]++;
	}

	inline void lock  (const T& reg) { locks[reg.value]++; }
	inline void unlock(const T& reg) { locks[reg.value]--; }
	inline bool locked(const T& reg) const { return locks[reg.value] != 0; }

	void debug()
	{
		for (int i = 0; i < T::n; i++)
		{
			std::cout << T::names[i] << " locked: " << locks[i] << '\n';
		}
	}
private:
	std::vector<int> locks;
};

template<typename T>
class lock_register
{
public:
	lock_register(locked_registers<T>& lr)
	 : lr(lr)
	{ }

	lock_register(locked_registers<T>& lr, const T& reg)
	 : lock_register(lr)
	{
		lock(reg);
	}

	inline void lock(const T& reg)
	{
		locked = true;
		lr.lock(reg);
		reg_idx = reg.value;
	}

	inline void unlock()
	{
		if (locked)
		{
			lr.unlock(T(reg_idx));
			locked = false;
		}
	}

	virtual ~lock_register()
	{
		unlock();
	}

private:
	locked_registers<T>& lr;
	bool locked = false;
	int reg_idx = 0;
};

template<typename T>
class used_register
{
public:
	used_register(int n, const locked_registers<T>& lr)
	 : lr(lr)
	{
		used.resize(n, false);
	}

	inline void use(T reg) { used[reg.value] = true; }
	inline void release(T reg) { used[reg.value] = false; }
	inline bool is_used(T reg) { return used[reg.value]; }
	inline bool is_free(T reg) { return !used[reg.value] && !lr.locked(reg); }

	T get()
	{
		for (int i = 0; i < (int) used.size(); i++)
		{
			if (is_free(i))
			{
				use(T(i));
				return T(i);
			}
		}

		return T(-1);
	}

	virtual ~used_register() { }

private:
	std::vector<bool> used;
	const locked_registers<T>& lr;
};

#endif /* COMPILER_USEDREGISTER_H_ */

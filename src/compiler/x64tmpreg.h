/*
 * x64tmpreg.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_X64TMPREG_H_
#define COMPILER_X64TMPREG_H_

#include "../arch/instructionstream.h"
#include "../arch/x64/x64instruction.h"
#include "usedregister.h"

template<typename T>
class x64_push_pop_reg
{
public:
	x64_push_pop_reg(instruction_stream& s)
	 : s(s)
	{ }

	x64_push_pop_reg(instruction_stream& s, T reg)
	 : s(s), reg_idx(reg)
	{
		push(reg);
	}

	inline bool is_pushed() { return pushed; }

	void push(T reg)
	{
		reg_idx = reg.value;
		push();
	}

	void pop()
	{
		if (pushed)
		{
			s << x64_pop(x64_reg64(reg_idx));
			pushed = false;
		}
	}

	~x64_push_pop_reg() { pop(); }

private:
	void push()
	{
		s << x64_push(x64_reg64(reg_idx)); /* Can't push 32-bit registers on x64 */
		pushed = true;
	}

	instruction_stream& s;
	int reg_idx = 0;

	bool pushed = false;
};

template<typename T>
class x64_tmp_reg
{
public:
	x64_tmp_reg(instruction_stream& s, used_register<T>& dr)
	 : s(s), dr(dr), push_pop(s)
	{ }

	T take(T default_reg = T(0))
	{
		auto tmp = dr.get();
		if (tmp.value == -1)
		{
			/* No free register. Store the default_reg and use that */
			push_pop.push(default_reg);
			reg_idx = default_reg.value;
		}
		else
			reg_idx = tmp.value;

		return T(reg_idx);
	}

	T take(std::initializer_list<T> exclude)
	{
		auto tmp = dr.get(exclude);
		if (tmp.value == -1)
		{
			for (int i = 0; i < T::n; i++)
			{
				auto reg = T(i);
				if (used_register<T>::is_exluced(exclude, reg) ||
						dr.is_illegal(reg))
					continue;

				push_pop.push(reg);
				reg_idx = reg.value;
				return reg;
			}

			throw std::invalid_argument("Too many excluded registers");
		}
		else
			reg_idx = tmp.value;

		return T(reg_idx);
	}

	void release() { push_pop.pop(); }

	inline T reg() { return T(reg_idx); }

	virtual ~x64_tmp_reg() { release(); }

private:
	instruction_stream& s;
	used_register<T>& dr;

	x64_push_pop_reg<T> push_pop;
	int reg_idx = 0;
};

template<typename T>
class x64_steal_reg
{
public:
	x64_steal_reg(instruction_stream& s, used_register<T>& dr)
	 : s(s), dr(dr), push_pop(s)
	{ }

	x64_steal_reg(instruction_stream& s, used_register<T>& dr, T reg)
	 : s(s), dr(dr), push_pop(s)
	{
		take(reg);
	}

	void take(T reg, std::initializer_list<T> exclude)
	{
		reg_idx = reg.value;
		used = dr.is_used(reg_idx);
		if (used)
		{
			/* Can we temporarily store it somewhere else? */
			auto tmp_reg = dr.get(exclude);
			tmp_reg_idx = tmp_reg.value;
			if (tmp_reg_idx != -1)
				/* Yes. Just store it in some temporary register */
				s << x64_mov(T(tmp_reg_idx), reg);
			else
				/* Nope. We'll have to push it */
				push_pop.push(reg);
		}
		else
		{
			dr.use(reg);
		}

		stolen = 1;
	}

	~x64_steal_reg() { release(); }

	void release()
	{
		if (!stolen)
			return;

		if (used)
		{
			if (push_pop.is_pushed())
				push_pop.pop();
			else
				s << x64_mov(T(reg_idx), T(tmp_reg_idx));

			used = false;
		}
		else
		{
			dr.release(T(reg_idx));
		}
	}

private:
	instruction_stream& s;
	used_register<T>& dr;

	int reg_idx = 0;
	bool stolen = 0;
	int tmp_reg_idx = 0;
	bool used = false;
	x64_push_pop_reg<T> push_pop;
};

#endif /* COMPILER_X64TMPREG_H_ */

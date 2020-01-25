/*
 * tac2x64.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_TAC2X64_H_
#define COMPILER_TAC2X64_H_

#include <map>
#include <signal.h>
#include <ucontext.h>
#include <cstring>

#include "../arch/instructionstream.h"
#include "../parser/expression.h"
#include "../arch/x64/x64instruction.h"
#include "tac.h"
#include "usedregister.h"
#include "x64tmpreg.h"
#include "stackvariable.h"
#include "interfgraph.h"

/* Debug classes XXX: Move this shit somewhere else.*/
class exprcted_regs_exception: public std::exception
{
public:
	explicit exprcted_regs_exception() { }

	virtual const char* what() const throw() { return "Registers didn't match expected_registers"; }
};

struct registers_dump
{
	uint64_t rax = 0,rcx = 0,rdx = 0,rbx = 0;
	uint64_t rsp = 0,rbp = 0,rsi = 0,rdi = 0;
	uint64_t r8  = 0,r9  = 0,r10 = 0,r11 = 0;
	uint64_t r12 = 0,r13 = 0,r14 = 0,r15 = 0;

	registers_dump() { }
	registers_dump(ucontext_t *context);

	uint64_t &operator[] (int index) { return arr[index & 0x0f]; }

	inline bool pass(const registers_dump& other) {
		return	rax == other.rax && rbx == other.rbx && rcx == other.rcx && rdx == other.rdx &&
				rsi == other.rsi && rdi == other.rdi && r8  == other.r8  && r9  == other.r9  &&
				r10 == other.r10 && r11 == other.r11 && r12 == other.r12 && r13 == other.r13 &&
				r14 == other.r14 && r15 == other.r15;
	}

	void print();

	virtual ~registers_dump() { }

private:
	uint64_t* arr = &rax;
};


template<typename T>
class get_reg
{
public:
	get_reg(used_registers<T>& ur, tac_var_type type)
	 : ur(ur), type(type)
	{

	}

	void store_var_in(int id, const T& reg)
	{
		auto it = regs.find(id);
		if (it != regs.end())
			regs.erase(it);

		regs[id] = reg.value;
	}

	T reg_for_var(int id, const tac_entry& entry)
	{
		/* If it's already in the map, return that */
		auto it = regs.find(id);
		if (it != regs.end())
			return T(regs[id]);

		/* Otherwise, check if there's anything free */
		auto reg = ur.get();
		if (reg.value != -1)
		{
			regs[id] = reg.value;
			return reg;
		}

		/* Still nothing? Let's see if there's anything in the map that we can re-use */
		for(auto it = regs.begin(); it != regs.end(); ++it)
		{
			if (!entry.live_vars[it->first])
			{
				/* This guy won't be used. We'll steal it's register */
				int red_idx = it->second;
				regs.erase(it);

				regs[id] = red_idx;

				return T(red_idx);
			}
		}

		/* For now, give up. We should request a temporary register here */
		throw std::exception();
	}

private:
	used_registers<T>& ur;
	tac_var_type type;

	std::map<int, int> regs; /* <var_id, reg_idx> */
};

/* Actual class */
class tac2x64
{
public:
	tac2x64(instruction_stream& s);

	void compile_expression(const tac& t);

	virtual ~tac2x64();

private:
	interf_graph rig;
	std::map<int, int> color_map;
	inline int var_color(const tac_var& var) { return color_map[var.id]; }
	inline int32_t get_stack_pos(int color) { return color * sizeof(int32_t); } // First stack color is -1

	std::array<int, 4> reg_avail {
			//x64_regs::eax.value,	/* Reserved for certain ALU instructions */
			x64_regs::ecx.value,
			//x64_regs::edx.value,	/* Reserved for idiv, until we can mark edx as live for these instructions */
			x64_regs::ebx.value,
			//x64_regs::esp.value,	/* Stack pointer */
			//x64_regs::ebp.value,	/* Frame pointer */
			x64_regs::esi.value,
			x64_regs::edi.value,
//			x64_regs::r8d.value,
//			x64_regs::r9d.value,
//			x64_regs::r10d.value,
			//x64_regs::r11d.value,	/* Load/store temp register */
//			x64_regs::r12d.value,
//			x64_regs::r13d.value,
//			x64_regs::r14d.value,
//			x64_regs::r15d.value,
	};
	const x64_reg32 temp_reg = x64_regs::r11d;

	void add_tac_var(const tac_var& var);

	struct sigaction saved_action;
	static void handler(int sig, siginfo_t *si, void *context);
	static registers_dump* expected_registers;

	locked_registers<x64_reg32> lr{ x64_reg32::n, { x64_regs::esp, x64_regs::ebp }};
	used_registers<x64_reg32> dr{ x64_reg32::n, lr};

	instruction_stream& inst_stream;

	void prologue(int32_t stack_size);
	void epilogue();

	enum variable_type
	{
		reg,
		stack,
		constant,
	};

	struct var
	{
		var() { }
		var(const var&) = default;
		var(variable_type type, int32_t i) : type(type), i(i) { }
		var(x64_reg32 reg) : type(variable_type::reg), i(reg.value) { }

		template<typename T>
		inline T reg() const { return T(type == variable_type::reg ? i : -1); }

		template<typename T>
		inline bool is_reg(const T& reg) const { return type == variable_type::reg && i == reg.value; }

		variable_type type = variable_type::reg;
		int32_t i = 0;
	};

	const var var_from_tac_var(const tac_var& tv);

	void op_assign(const tac_entry& entry);
	void op_add(const tac_entry& entry);
	void op_sub(const tac_entry& entry);
	void op_mul(const tac_entry& entry);
	void op_div(const tac_entry& entry);
	void op_ret(const tac_entry& entry);

	std::map<int, var> var_map;
//	int stack_pos = 0;

	template<typename T>
	void stack_to_reg(x64_reg32 dst, int32_t src_offset)
	{
		if (src_offset < -128 || src_offset > 127)
			inst_stream << T(dst, x64_address(x64_regs::rbp, src_offset));
		else
			inst_stream << T(dst, x64_address(x64_regs::rbp, static_cast<int8_t>(src_offset)));
	}

	template<typename T>
	void reg_to_stack(int32_t dst_offset, x64_reg32 src)
	{
		if (dst_offset < -128 || dst_offset > 127)
			inst_stream << T(x64_address(x64_regs::rbp, dst_offset), src);
		else
			inst_stream << T(x64_address(x64_regs::rbp, static_cast<int8_t>(dst_offset)), src);
	}

	template<typename T>
	void single_op_stack(int32_t dst_offset)
	{
		if (dst_offset < -128 || dst_offset > 127)
			inst_stream << T(x64_address(x64_regs::rbp, dst_offset));
		else
			inst_stream << T(x64_address(x64_regs::rbp, static_cast<int8_t>(dst_offset)));
	}

	template<typename T>
	void src_dest(const var& dst, const var& src)
	{
		switch(dst.type)
		{
		case variable_type::constant:
			throw std::invalid_argument("Destination can't be a constant");

		/* Register destination */
		case variable_type::reg:
		{
			switch(src.type)
			{
			/* reg to reg */
			case variable_type::reg:
				inst_stream << T(x64_reg32(dst.i), x64_reg32(src.i));
				break;

			/* stack to reg */
			case variable_type::stack:
				stack_to_reg<T>(x64_reg32(dst.i), src.i);
				break;

			/* constant to reg */
			case variable_type::constant:
				inst_stream << T(x64_reg32(dst.i), static_cast<uint32_t>(src.i));
				break;
			}
			break;
		}

		/* Memory destination */
		case variable_type::stack:
		{
			switch(src.type)
			{
			/* reg to stack */
			case variable_type::reg:
				reg_to_stack<T>(dst.i, src.i);
				break;

			/* stack to stack */
			case variable_type::stack:
			{
				//x64_tmp_reg<x64_reg32> tmp(inst_stream, dr, lr);
				//stack_to_reg<x64_mov>(tmp.take(), src.i);
				//reg_to_stack<T>(dst.i, tmp.reg());
				stack_to_reg<x64_mov>(temp_reg, src.i);
				reg_to_stack<T>(dst.i, temp_reg);
				break;
			}

			/* constant to stack */
			case variable_type::constant:
			{
				//x64_tmp_reg<x64_reg32> tmp(inst_stream, dr, lr);
				//inst_stream << x64_mov(tmp.take(), static_cast<uint32_t>(src.i));
				//reg_to_stack<T>(dst.i, tmp.reg());
				inst_stream << x64_mov(temp_reg, static_cast<uint32_t>(src.i));
				reg_to_stack<T>(dst.i, temp_reg);
				break;
			}
			}
			break;
		}
		}
	}

};

#endif /* COMPILER_TAC2X64_H_ */

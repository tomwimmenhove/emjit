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

	T reg_for_var(int id, std::vector<tac_entry>::const_iterator from, std::vector<tac_entry>::const_iterator to)
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
			if (!will_be_read(regs[it->first], from, to))
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

	bool will_be_read(int id, std::vector<tac_entry>::const_iterator from, std::vector<tac_entry>::const_iterator to)
	{
		for(auto it = from; it != to; ++it)
		{
			/* If the next access is a write, we're good */
			if (it->a.type == type && it->a.id == id)
				return false;

			/* If it's a read, we can't use is */
			if ((it->b.type == type && it->b.id == id) ||
					(it->c.type == type && it->c.id == id))
				return true;
		}

		return false;
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

	void op_assign(get_reg<x64_reg32>& gr, std::vector<tac_entry>::const_iterator at, std::vector<tac_entry>::const_iterator to);
	void op_add(get_reg<x64_reg32>& gr, std::vector<tac_entry>::const_iterator at, std::vector<tac_entry>::const_iterator to);
	void op_sub(get_reg<x64_reg32>& gr, std::vector<tac_entry>::const_iterator at, std::vector<tac_entry>::const_iterator to);
	void op_mul(get_reg<x64_reg32>& gr, std::vector<tac_entry>::const_iterator at, std::vector<tac_entry>::const_iterator to);
	void op_div(get_reg<x64_reg32>& gr, std::vector<tac_entry>::const_iterator at, std::vector<tac_entry>::const_iterator to);

	std::map<int, var> var_map;
	int stack_pos = 0;

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


//	template<typename T>
//	void div(x64_reg32 dst, x64_reg32 divident, x64_reg32 divisor)
//	{
//		/* Careful: order matters! */
//		x64_tmp_reg<x64_reg32> tmp(inst_stream, dr);
//		x64_steal_reg<x64_reg32> steal_edx(inst_stream, dr);
//		x64_steal_reg<x64_reg32> steal_eax(inst_stream, dr);
//
//		int divisor_idx;
//		/* If the divisor is edx, we'll have to use a
//		 * temporary register, because edx should be zero */
//		if (divisor.value == x64_regs::edx.value)
//		{
//			/* Make sure we don't end up with our tmp in edx :) */
//			auto tmp_reg = tmp.take({dst, x64_regs::edx, x64_regs::eax, divident});
//
//			/* Move the divisor into our tmp register */
//			inst_stream << x64_mov(tmp_reg, x64_regs::edx);
//			divisor_idx = tmp_reg.value;
//		}
//		else
//		{
//			/* Otherwise, take edx so that we can safely set it to 0 within this scope*/
//			steal_edx.take(x64_regs::edx, {dst, x64_regs::edx});
//			divisor_idx = divisor.value;
//		}
//
//		/* Zero edx */
//		inst_stream << x64_xor(x64_regs::edx, x64_regs::edx);
//
//		/* Move the divident into eax. Store it if we have to */
//		if (divident.value != x64_regs::eax.value)
//		{
//			if (dst.value != x64_regs::eax.value)
//				steal_eax.take(x64_regs::eax, {dst, x64_regs::eax});
//
//			inst_stream << x64_mov(x64_regs::eax, divident);
//		}
//
//		inst_stream << T(x64_reg32(divisor_idx));
//
//		/* Move the result into the correct register, if not already there */
//		if (dst.value != x64_regs::eax.value)
//			inst_stream << x64_mov(dst, x64_regs::eax);
//
//		/* Do we need to restore edx ? */
//		if (divisor.value == x64_regs::edx.value && dr.is_used(x64_regs::edx))
//			inst_stream << x64_mov(divisor, tmp.reg());
//	}

	void div_test(const var& dst, const var& dividend, const var& divisor, int i_dividend, int i_divisor);

	/* X64 IDIV instruction:
	 * Result (dst) _always_ in EAX
	 * Dividend is _always_ in EAX
	 * Divisor can be either a register or memory
	 * EDX will be set to zero before issueing the instruction
	 */
	template<typename T>
	void div(const var& dst, const var& dividend, const var& divisor)
	{
		auto divisor_internal = divisor;

		/* Lock variables that we can't use as temporaries. dst is added here because if it's not locked, it
		 * might get popped at the end (when going out of scope), overwriting the result. We also can't check if it's in
		 * use because, if it's not, it might be used as a temporary later. */
		lock_register<x64_reg32> lock_dst(lr, dst.reg<x64_reg32>(),dst.type == variable_type::reg);
		lock_register<x64_reg32> lock_dividend(lr, dividend.reg<x64_reg32>(), dividend.type == variable_type::reg);
		lock_register<x64_reg32> lock_divisor(lr,  divisor_internal.reg<x64_reg32>(),  divisor_internal.type == variable_type::reg);
		lock_register<x64_reg32> lock_eax(lr, x64_regs::eax);
		lock_register<x64_reg32> lock_edx(lr, x64_regs::edx);

		/* Move the dividend into eax. Store it if we have to */
		x64_steal_reg<x64_reg32> steal_eax(inst_stream, dr, lr);
		x64_tmp_reg<x64_reg32> tmp(inst_stream, dr, lr);
		if (!dividend.is_reg(x64_regs::eax))
		{
			if (!dst.is_reg(x64_regs::eax))
				steal_eax.take(x64_regs::eax);

			/* If the divisor was in eax, put it in a temporary before overwriting eax */
			if (divisor_internal.is_reg(x64_regs::eax))
			{
				divisor_internal = var(tmp.take());
				inst_stream << x64_mov(divisor_internal.reg<x64_reg32>(), x64_regs::eax);
			}

			src_dest<x64_mov>(var(variable_type::reg, x64_regs::eax.value), dividend);
		}

		/* Check if we need to use a temporary register for the divisor */
		int divisor_idx;
		x64_steal_reg<x64_reg32> steal_edx(inst_stream, dr, lr);
		if (divisor_internal.type == variable_type::constant || divisor_internal.is_reg(x64_regs::edx))
		{
			auto tmp_reg = tmp.take();

			/* Move the divisor into our tmp register. */
			src_dest<x64_mov>(var(variable_type::reg, tmp_reg.value), divisor_internal);
			divisor_idx = tmp_reg.value;
		}
		else /* Otherwise, take edx so that we can safely set it to 0 within this scope*/
		{
			steal_edx.take(x64_regs::edx);
			divisor_idx = divisor_internal.i;
		}

		/* Zero edx */
		inst_stream << x64_xor(x64_regs::edx, x64_regs::edx);

		/* IDIV supports either dividing by a register, or address */
		if (divisor_internal.type == variable_type::stack)
			single_op_stack<T>(divisor_internal.i);
		else
			inst_stream << T(x64_reg32(divisor_idx));

		/* Move the result into the correct register, if not already there */
		if (!dst.is_reg(x64_regs::eax))
			src_dest<x64_mov>(dst, var(variable_type::reg, x64_regs::eax.value));

		/* Do we need to restore edx ? */
		if (divisor_internal.is_reg(x64_regs::edx) && dr.is_used(x64_regs::edx))
			inst_stream << x64_mov(divisor_internal.reg<x64_reg32>(), tmp.reg());
	}

	var tac_var_to_var(const tac_var& v);

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
				x64_tmp_reg<x64_reg32> tmp(inst_stream, dr, lr);
				stack_to_reg<x64_mov>(tmp.take(), src.i);
				reg_to_stack<T>(dst.i, tmp.reg());
				break;
			}

			/* constant to stack */
			case variable_type::constant:
			{
				x64_tmp_reg<x64_reg32> tmp(inst_stream, dr, lr);
				inst_stream << x64_mov(tmp.take(), static_cast<uint32_t>(src.i));
				reg_to_stack<T>(dst.i, tmp.reg());
				break;
			}
			}
			break;
		}
		}
	}

};

#endif /* COMPILER_TAC2X64_H_ */

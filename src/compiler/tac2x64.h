/*
 * tac2x64.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_TAC2X64_H_
#define COMPILER_TAC2X64_H_

#include <map>

#include "../arch/instructionstream.h"
#include "../parser/expression.h"
#include "../arch/x64/x64instruction.h"
#include "tac.h"
#include "usedregister.h"
#include "x64tmpreg.h"

class tac2x64
{
public:
	tac2x64(instruction_stream& s)
: inst_stream(s)
{ }

	void compile_expression(const tac& t);

	virtual ~tac2x64()
	{ }

private:
	void add_tac_var(const tac_var& var);

	used_register<x64_reg32> dr{ x64_reg32::n, { x64_regs::eax, x64_regs::esp, x64_regs::ebp }};

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

		template<typename T>
		inline T reg() const { return T(type == variable_type::reg ? i : -1); }

		template<typename T>
		inline bool is_reg(const T& reg) const { return type == variable_type::reg && i == reg.value; }

		variable_type type = variable_type::reg;
		int32_t i = 0;
	};

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


	template<typename T>
	void div(x64_reg32 dst, x64_reg32 divident, x64_reg32 divisor)
	{
		/* Careful: order matters! */
		x64_tmp_reg<x64_reg32> tmp(inst_stream, dr);
		x64_steal_reg<x64_reg32> steal_edx(inst_stream, dr);
		x64_steal_reg<x64_reg32> steal_eax(inst_stream, dr);

		int divisor_idx;
		/* If the divisor is edx, we'll have to use a
		 * temporary register, because edx should be zero */
		if (divisor.value == x64_regs::edx.value)
		{
			/* Make sure we don't end up with our tmp in edx :) */
			auto tmp_reg = tmp.take({dst, x64_regs::edx, x64_regs::eax, divident});

			/* Move the divisor into our tmp register */
			inst_stream << x64_mov(tmp_reg, x64_regs::edx);
			divisor_idx = tmp_reg.value;
		}
		else
		{
			/* Otherwise, take edx so that we can safely set it to 0 within this scope*/
			steal_edx.take(x64_regs::edx, {dst, x64_regs::edx});
			divisor_idx = divisor.value;
		}

		/* Zero edx */
		inst_stream << x64_xor(x64_regs::edx, x64_regs::edx);

		/* Move the divident into eax. Store it if we have to */
		if (divident.value != x64_regs::eax.value)
		{
			if (dst.value != x64_regs::eax.value)
				steal_eax.take(x64_regs::eax, {dst, x64_regs::eax});

			inst_stream << x64_mov(x64_regs::eax, divident);
		}

		inst_stream << T(x64_reg32(divisor_idx));

		/* Move the result into the correct register, if not already there */
		if (dst.value != x64_regs::eax.value)
			inst_stream << x64_mov(dst, x64_regs::eax);

		/* Do we need to restore edx ? */
		if (divisor.value == x64_regs::edx.value && dr.is_used(x64_regs::edx))
			inst_stream << x64_mov(divisor, tmp.reg());
	}


	/* X64 IDIV instruction:
	 * Result (dst) _always_ in EAX
	 * Dividend is _always_ in EAX
	 * Divisor can be either a register or memory
	 * EDX will be set to zero before issueing the instruction
	 */
	template<typename T>
	void div(const var& dst, const var& dividend, const var& divisor)
	{
		/* Careful: order matters! */
		x64_tmp_reg<x64_reg32> tmp(inst_stream, dr);
		x64_steal_reg<x64_reg32> steal_edx(inst_stream, dr);
		x64_steal_reg<x64_reg32> steal_eax(inst_stream, dr);

		int divisor_idx;
		/* Check if we need to use a temporary register for the divisor */
		if (divisor.type == variable_type::constant || divisor.is_reg(x64_regs::edx))
		{
			/* Make sure we don't end up with our tmp in edx :) */
			auto tmp_reg = tmp.take({x64_regs::edx, x64_regs::eax,
				dst.reg<x64_reg32>(), dividend.reg<x64_reg32>()});

			/* Move the divisor into our tmp register.
			 * We can safely do this, because it won't use any temporaries */
			src_dest<x64_mov>(var(variable_type::reg, tmp_reg.value), divisor);
			divisor_idx = tmp_reg.value;
		}
		else
		{
			/* Otherwise, take edx so that we can safely set it to 0 within this scope*/
			steal_edx.take(x64_regs::edx, {dst.reg<x64_reg32>(), x64_regs::edx});
			divisor_idx = divisor.i;
		}

		/* Zero edx */
		inst_stream << x64_xor(x64_regs::edx, x64_regs::edx);

		/* Move the divident into eax. Store it if we have to */
		if (!dividend.is_reg(x64_regs::eax))
		{
			if (!dst.is_reg(x64_regs::eax))
				steal_eax.take(x64_regs::eax, {dst.reg<x64_reg32>(), x64_regs::eax});

			src_dest<x64_mov>(var(variable_type::reg, x64_regs::eax.value), dividend);
		}

		/* IDIV supports either dividing by a register, or address */
		if (divisor.type == variable_type::stack)
			single_op_stack<T>(divisor.i);
			//inst_stream << T(x64_address(x64_regs::rbp, divisor.i));
			//inst_stream << x64_idivl(x64_address(x64_regs::rbp, divisor.i));
		else
			inst_stream << T(x64_reg32(divisor_idx));

		/* Move the result into the correct register, if not already there */
		if (!dst.is_reg(x64_regs::eax))
			src_dest<x64_mov>(dst, var(variable_type::reg, x64_regs::eax.value));

		/* Do we need to restore edx ? */
		if (divisor.is_reg(x64_regs::edx) && dr.is_used(x64_regs::edx))
			inst_stream << x64_mov(divisor.reg<x64_reg32>(), tmp.reg());
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
				inst_stream << T(x64_reg32(dst.i), src.i);
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
				x64_tmp_reg<x64_reg32> tmp(inst_stream, dr);
				stack_to_reg<T>(tmp.take(), src.i);
				reg_to_stack<T>(dst.i, tmp.reg());
				break;
			}

			/* constant to stack */
			case variable_type::constant:
			{
				x64_tmp_reg<x64_reg32> tmp(inst_stream, dr);
				inst_stream << T(tmp.take(), src.i);
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

/*
 * tac2x64.h
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#ifndef COMPILER_TAC2X64_H_
#define COMPILER_TAC2X64_H_

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
	used_register dr{ x64_reg32::n, { x64_regs::eax.value, x64_regs::esp.value, x64_regs::ebp.value }};

	instruction_stream& inst_stream;

	enum variable_type
	{
		reg,
		stack,
		constant,
	};

	struct var
	{
		var(variable_type type, x64_reg32 reg) : type(type), reg(reg) { }
		var(variable_type type, int32_t i) : type(type), i(i) { }

		variable_type type;
		union
		{
			x64_reg32 reg;
			int32_t i;
		};
	};

	template<typename T>
	void stack_to_reg(x64_reg32 dst, int32_t src_offset)
	{
		if (src_offset < -128 || src_offset > 127)
			inst_stream << T(dst, x64_reg_addr(x64_regs::rsp), src_offset);
		else
			inst_stream << T(dst, x64_reg_addr(x64_regs::rsp), static_cast<int8_t>(src_offset));
	}

	template<typename T>
	void reg_to_stack(int32_t dst_offset, x64_reg32 src)
	{
		if (dst_offset < -128 || dst_offset > 127)
			inst_stream << T(x64_reg_addr(x64_regs::rsp), dst_offset, src);
		else
			inst_stream << T(x64_reg_addr(x64_regs::rsp), static_cast<int8_t>(dst_offset), src);
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
				inst_stream << T(dst.reg, src.reg);
				break;

			/* stack to reg */
			case variable_type::stack:
				stack_to_reg<T>(dst.reg, src.i);
				break;

			/* constant to reg */
			case variable_type::constant:
				inst_stream << T(dst.reg, src.i);
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
				reg_to_stack<T>(dst.i, src.reg);
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

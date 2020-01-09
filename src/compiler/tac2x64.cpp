/*
 * tac2x64.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#include "tac2x64.h"

void tac2x64::compile_expression(const tac&)
{

	for (auto i = 0; i < 16; i++)
	{
		//dr.use(i);
	}
	//inst_stream << x64_mov(x64_regs::eax, 42);

//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax),
//			var(variable_type::stack, 12));
//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax),
//			var(variable_type::stack, 0x1200));
//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax),
//			var(variable_type::constant, 0x1200));
//
//
//	src_dest<x64_mov>(
//			var(variable_type::stack, 0x1200),
//			var(variable_type::reg, x64_regs::eax)
//			);

	inst_stream << x64_nop1();

	src_dest<x64_mov>(
			var(variable_type::stack, 0x1200),
			var(variable_type::reg, x64_regs::eax)
			);

	inst_stream << x64_nop1();

	src_dest<x64_mov>(
			var(variable_type::stack, 0x1200),
			var(variable_type::constant, 0x1211)
			);

	inst_stream << x64_nop1();

	src_dest<x64_mov>(
			var(variable_type::stack, 0x1200),
			var(variable_type::stack, 0x1211)
			);

	inst_stream << x64_ret();
}

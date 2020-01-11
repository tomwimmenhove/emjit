/*
 * tac2x64.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#include <iostream>

#include "tac2x64.h"

using namespace std;

void tac2x64::prologue(int32_t stack_size)
{
	inst_stream << x64_push(x64_regs::rbp);
	inst_stream << x64_mov(x64_regs::rbp, x64_regs::rsp);
	if (stack_size)
		inst_stream << x64_sub(x64_regs::rsp, static_cast<uint32_t>(stack_size));
}

void tac2x64::epilogue()
{
	inst_stream << x64_leave();
}

void tac2x64::add_tac_var(const tac_var& tv)
{
	if (tv.id == -1)
		return;

	if (var_map.find(tv.id) != var_map.end())
		return;

	auto reg = dr.get();
	if (reg.value != -1)
	{
		var_map[tv.id] = var(variable_type::reg, reg.value);
	}
	else
	{
		var_map[tv.id] = var(variable_type::stack, stack_pos);

		stack_pos += 8;
	}
}

void tac2x64::compile_expression(const tac& t)
{
	auto& entries = t.get_entries();

	/* Create a table of variables */
	for(auto entry: entries)
	{
		/* In the dumbest fucking way possible */
		add_tac_var(entry.a);
		add_tac_var(entry.b);
		add_tac_var(entry.c);
	}

	for(auto entry: entries)
	{


	}

	for (auto i = 0; i < 16; i++)
	{
		dr.use(i);
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
//
//	inst_stream << x64_nop1();
//
//	src_dest<x64_mov>(
//			var(variable_type::stack, 0x1200),
//			var(variable_type::reg, x64_regs::eax.value)
//			);
//
//	inst_stream << x64_nop1();
//
//	src_dest<x64_mov>(
//			var(variable_type::stack, 0x1200),
//			var(variable_type::constant, 0x1211)
//			);
//
//	inst_stream << x64_nop1();
//
//	src_dest<x64_mov>(
//			var(variable_type::stack, 0x1200),
//			var(variable_type::stack, 0x1211)
//			);

	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(i);
		if (reg.is_sp() || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_push(reg);
		inst_stream << x64_mov(reg, (uint32_t) 42);
	}

	inst_stream << x64_nop1();
	prologue(0);
	inst_stream << x64_nop1();


	inst_stream << x64_mov(x64_regs::ecx, 1000);
	inst_stream << x64_mov(x64_regs::edx, 100);

	inst_stream << x64_nop1();

	div<x64_div>(x64_regs::ebx, x64_regs::ecx, x64_regs::edx);

	inst_stream << x64_nop1();

	inst_stream << x64_mov(x64_regs::eax, x64_regs::ebx);


	inst_stream << x64_nop1();
	epilogue();
	inst_stream << x64_nop1();

	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(x64_reg32::n - i - 1);
		if (reg.is_sp() || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_pop(reg);
	}

	inst_stream << x64_ret();


	auto div_fn = inst_stream.entry_point<int(int, int)>();
	div<x64_div>(x64_regs::eax, x64_regs::edi, x64_regs::esi);
	inst_stream << x64_ret();

	cout << "bla: " << div_fn(42 * 3, 3) << '\n';


	inst_stream << x64_imul(x64_regs::rbx, x64_regs::rcx);
	inst_stream << x64_imul(x64_regs::ebx, x64_regs::ecx);
	inst_stream << x64_imul(x64_regs::bx, x64_regs::cx);

	inst_stream << x64_imul(x64_regs::rbx, x64_regs::r12);
	inst_stream << x64_imul(x64_regs::ebx, x64_regs::r12d);
	inst_stream << x64_imul(x64_regs::bx, x64_regs::r12w);

}



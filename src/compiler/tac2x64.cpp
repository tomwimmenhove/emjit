/*
 * tac2x64.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#include <iostream>

#include "tac2x64.h"

#include "../arch/x64/x64disassembler.h"

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

//	for(auto entry: entries)
//	{
//
//
//	}

	for (auto i = 0; i < 16; i++)
	{
		dr.use(i);
	}

	auto program = inst_stream.entry_point<int()>();

//	inst_stream << x64_mov(x64_regs::eax, 42);
//
//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax.value),
//			var(variable_type::stack, 12));
//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax.value),
//			var(variable_type::stack, 0x1200));
//	src_dest<x64_mov>(
//			var(variable_type::reg, x64_regs::eax.value),
//			var(variable_type::constant, 0x1200));
//
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


	prologue(0);

	/* Push all registers and set them to 42 */
	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(i);
		if (reg.is_sp() || reg.is_bp() || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_push(reg);
		inst_stream << x64_mov(reg, (uint32_t) 42);
	}

//	auto result = var(variable_type::stack,  -64 + 0);
//	auto dvidend = var(variable_type::stack, -64 + 8);
//	auto divisor = var(variable_type::stack, -64 + 16);

	auto result = var(variable_type::reg,  x64_regs::eax.value);
	auto dvidend = var(variable_type::reg, x64_regs::eax.value);
	auto divisor = var(variable_type::reg, x64_regs::ecx.value);


	src_dest<x64_mov>(dvidend, var(variable_type::constant, 1000));
	src_dest<x64_mov>(divisor, var(variable_type::constant, 100));

	div<x64_idivl>(result, dvidend, divisor);

	inst_stream << x64_nop1();

	src_dest<x64_mov>(var(variable_type::reg, x64_regs::eax.value), result);

	/* Restore all register */
	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(x64_reg32::n - i - 1);
		if (reg.is_sp() || reg.is_bp() || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_pop(reg);
	}

	epilogue();

	inst_stream << x64_ret();

	cout << x64_disassembler::disassemble(inst_stream, "intel", true);

	auto res = program();
	cout << "Result: " << res << '\n';

//	auto div_fn = inst_stream.entry_point<int(int, int)>();
//	div<x64_div>(x64_regs::eax, x64_regs::edi, x64_regs::esi);
//	inst_stream << x64_ret();
//
//	cout << "bla: " << div_fn(42 * 3, 3) << '\n';



}



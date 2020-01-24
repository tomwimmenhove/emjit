/*
 * tac2x64.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#include <iostream>
#include <iomanip>
#include <map>

#include "tac2x64.h"
#include "../arch/x64/x64disassembler.h"

using namespace std;

registers_dump* tac2x64::expected_registers = nullptr;

tac2x64::tac2x64(instruction_stream& s) : inst_stream(s)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = &handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGTRAP, &sa, &saved_action);
}

tac2x64::~tac2x64()
{
	sigaction(SIGSEGV, &saved_action, nullptr);
}

/* For debugging */
registers_dump::registers_dump(ucontext_t *context)
{
	rax = context->uc_mcontext.gregs[REG_RAX];
	rbx = context->uc_mcontext.gregs[REG_RBX];
	rcx = context->uc_mcontext.gregs[REG_RCX];
	rdx = context->uc_mcontext.gregs[REG_RDX];
	rsi = context->uc_mcontext.gregs[REG_RSI];
	rdi = context->uc_mcontext.gregs[REG_RDI];
	rbp = context->uc_mcontext.gregs[REG_RBP];
	rsp = context->uc_mcontext.gregs[REG_RSP];
	r8  = context->uc_mcontext.gregs[REG_R8 ];
	r9  = context->uc_mcontext.gregs[REG_R9 ];
	r10 = context->uc_mcontext.gregs[REG_R10];
	r11 = context->uc_mcontext.gregs[REG_R11];
	r12 = context->uc_mcontext.gregs[REG_R12];
	r13 = context->uc_mcontext.gregs[REG_R13];
	r14 = context->uc_mcontext.gregs[REG_R14];
	r15 = context->uc_mcontext.gregs[REG_R15];;
}

void registers_dump::print()
{
	cout << setfill('0') << hex
			<< "rax: 0x" << setw(16) << rax << '\n'
			<< "rbx: 0x" << setw(16) << rbx << '\n'
			<< "rcx: 0x" << setw(16) << rcx << '\n'
			<< "rdx: 0x" << setw(16) << rdx << '\n'
			<< "rsi: 0x" << setw(16) << rsi << '\n'
			<< "rdi: 0x" << setw(16) << rdi << '\n'
			<< "rbp: 0x" << setw(16) << rbp << '\n'
			<< "rsp: 0x" << setw(16) << rsp << '\n'
			<< "r8:  0x" << setw(16) << r8  << '\n'
			<< "r9:  0x" << setw(16) << r9  << '\n'
			<< "r10: 0x" << setw(16) << r10 << '\n'
			<< "r11: 0x" << setw(16) << r11 << '\n'
			<< "r12: 0x" << setw(16) << r12 << '\n'
			<< "r13: 0x" << setw(16) << r13 << '\n'
			<< "r14: 0x" << setw(16) << r14 << '\n'
			<< "r15: 0x" << setw(16) << r15 << '\n';
}

void tac2x64::handler(int, siginfo_t*, void* context_p)
{
	ucontext_t *context = reinterpret_cast<ucontext_t*>(context_p);

	registers_dump dump(context);

	dump.print();

	if (expected_registers != nullptr && !expected_registers->pass(dump))
	{
		cerr << "Register expectation failed:\n";

		for (int i = 0; i < x64_reg64::n; i++)
		{
			if (x64_reg64(i).is_sp() || x64_reg64(i).is_bp())
				continue;

			if ((*expected_registers)[i] != dump[i])
				cerr << setfill('0') << hex << x64_reg64::names[i] << ": 0x" << setw(16) << dump[i] <<
					" -- expected: 0x" << setw(16) << (*expected_registers)[i] << '\n';
		}

		throw exprcted_regs_exception();
	}
}

void tac2x64::prologue(int32_t stack_size)
{
	inst_stream << x64_push(x64_regs::rbp) << x64_mov(x64_regs::rbp, x64_regs::rsp);
	if (stack_size)
		inst_stream << x64_sub(x64_regs::rsp, static_cast<uint32_t>(stack_size));
}

void tac2x64::epilogue()
{
	inst_stream << x64_leave() << x64_ret();
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

extern "C" void breakout_and_die()
{
	cerr << "Didn't make it. Sad\n";
	exit(1);
}


void tac2x64::div_test(const var& dst, const var& dividend, const var& divisor, int i_dividend, int i_divisor)
{
	int i_result = i_dividend / i_divisor;

	prologue(0);

	/* Push all registers and set them to 42 */
	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(i);
		if (reg.is_sp() || reg.is_bp())// || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_push(reg);
		inst_stream << x64_mov(reg, (uint32_t) i);
	}

//	lock_register<x64_reg32> lock_dst(lr);
//	lock_register<x64_reg32> lock_dividend(lr);
//	lock_register<x64_reg32> lock_divisor(lr);
	/* Setup a test for the register dump test */
	static registers_dump test;
	for (int i = 0; i < x64_reg32::n; i++)
		test[i] = i;
	if (dst.type == variable_type::reg)
	{
		dr.use(dst.reg<x64_reg32>());
		test[dst.i] = i_result;
	}
	if (dividend.type == variable_type::reg)
	{
		dr.use(dividend.reg<x64_reg32>());
		test[dividend.i] = i_result;
	}
	if (divisor.type == variable_type::reg)
	{
		dr.use(divisor.reg<x64_reg32>());
		test[divisor.i] = i_divisor;
	}

	inst_stream << x64_nop1();
	src_dest<x64_mov>(dividend, var(variable_type::constant, i_dividend));
	inst_stream << x64_nop1();
	inst_stream << x64_nop1();
	src_dest<x64_mov>(divisor, var(variable_type::constant, i_divisor));
	inst_stream << x64_nop1();
	inst_stream << x64_nop1();
	expected_registers = &test;

	div<x64_idivl>(dst, dividend, divisor);

	src_dest<x64_cmp>(dividend, var(variable_type::constant, i_dividend));
	inst_stream << x64_jz(12);
	inst_stream << x64_mov(x64_regs::rax, reinterpret_cast<uint64_t>(&breakout_and_die));
	inst_stream << x64_call(x64_regs::rax);

	src_dest<x64_cmp>(divisor, var(variable_type::constant, i_divisor));
	inst_stream << x64_jz(12);
	inst_stream << x64_mov(x64_regs::rax, reinterpret_cast<uint64_t>(&breakout_and_die));
	inst_stream << x64_call(x64_regs::rax);

	src_dest<x64_cmp>(dst, var(variable_type::constant, i_result));
	inst_stream << x64_jz(12);
	inst_stream << x64_mov(x64_regs::rax, reinterpret_cast<uint64_t>(&breakout_and_die));
	inst_stream << x64_call(x64_regs::rax);

	inst_stream << x64_int3(); // Dump registers

	//src_dest<x64_mov>(var(variable_type::reg, x64_regs::eax.value), dst);

	/* Restore all register */
	for (int i = 0; i < x64_reg32::n; i++)
	{
		auto reg = x64_reg64(x64_reg32::n - i - 1);
		if (reg.is_sp() || reg.is_bp())// || reg.value == x64_regs::eax.value)
			continue;
		inst_stream << x64_pop(reg);
	}

	epilogue();

	inst_stream << x64_ret();
}

//tac2x64::var tac2x64::tac_var_to_var(const tac_var& v)
//{
//	switch(v.type)
//	{
//	case tac_var_type::unused:
//		throw invalid_argument("Internal error: invalid TAC variable");
//		break;
//	case tac_var_type::constant:
//		return var(variable_type::constant, v.value);
//		break;
//	case tac_var_type::temp:
//		break;
//	case tac_var_type::local:
//		break;
//	case tac_var_type::param:
//		break;
//	}
//
//}

void tac2x64::op_assign(const tac_entry& entry)
{
	if (entry.b.type != tac_var_type::constant)
		throw invalid_argument("Assignments of non-constants not yet supported");

	//auto reg = gr.reg_for_var(entry.a.id, entry);
	auto reg = x64_reg32(reg_avail[color_map[entry.a.id]]);

	inst_stream << x64_mov(reg, entry.b.value);
}

void tac2x64::op_add(const tac_entry& entry)
{
	auto rega = x64_reg32(reg_avail[color_map[entry.a.id]]);
	auto regb = x64_reg32(reg_avail[color_map[entry.b.id]]);
	auto regc = x64_reg32(reg_avail[color_map[entry.c.id]]);

	if (rega.value != regb.value)
		inst_stream << x64_mov(rega, regb);

	inst_stream << x64_add(rega, regc);

//
//	auto regb = gr.reg_for_var(entry.b.id, entry);
//	auto regc = gr.reg_for_var(entry.c.id, entry);
//
//	/* TODO:
//	 *  Always just do: x64_add(regb, regc);
//	 *  AFTER calling some "we're storing this this in register x", that
//	 *  function can then decide wether to move the old storage place to
//	 *  some new register, or memory
//	 *
//	 *  ALSO:
//	 *  reg_for_var() should be able to get spilled registers back into registers
//	 *
//	 *  ^^ Fuck all that. Implement: https://www.youtube.com/watch?v=4eHdo8GaICY&list=PLEUhcCFlouiXsqOfvZlQ6ORkQZ7GwAab9&index=16&t=0s
//	 */
//
//	/* Can we use b to store the result? */
//	if (!entry.live_vars[entry.b.id])
//	{
//		inst_stream << x64_add(regb, regc);
//		gr.store_var_in(entry.a.id, regb); /* Result (a) is now in regb */
//		return;
//	}
//
//	/* Can we use c to store the result? */
//	if (!entry.live_vars[entry.c.id])
//	{
//		inst_stream << x64_add(regc, regb);
//		gr.store_var_in(entry.a.id, regc); /* Result (a) is now in regc */
//		return;
//	}
//
//	auto rega = gr.reg_for_var(entry.a.id, entry);
//	inst_stream << x64_mov(rega, regb) << x64_add(rega, regc);
}

void tac2x64::op_sub(const tac_entry& entry)
{
	auto rega = x64_reg32(reg_avail[color_map[entry.a.id]]);
	auto regb = x64_reg32(reg_avail[color_map[entry.b.id]]);
	auto regc = x64_reg32(reg_avail[color_map[entry.c.id]]);

	if (rega.value != regb.value)
		inst_stream << x64_mov(rega, regb);

	inst_stream << x64_sub(rega, regc);
//	auto regb = gr.reg_for_var(entry.b.id, entry);
//	auto regc = gr.reg_for_var(entry.c.id, entry);
//
//	if (!entry.live_vars[entry.b.id])
//	{
//		inst_stream << x64_sub(regb, regc);
//		gr.store_var_in(entry.a.id, regb); /* Result (a) is now in regb */
//		return;
//	}
//
//	auto rega = gr.reg_for_var(entry.a.id, entry);
//	inst_stream << x64_mov(rega, regb) << x64_sub(rega, regc);
}

void tac2x64::op_mul(const tac_entry& entry)
{
	auto rega = x64_reg32(reg_avail[color_map[entry.a.id]]);
	auto regb = x64_reg32(reg_avail[color_map[entry.b.id]]);
	auto regc = x64_reg32(reg_avail[color_map[entry.c.id]]);

	if (rega.value != regb.value)
		inst_stream << x64_mov(rega, regb);
	inst_stream << x64_imul(rega, regc);

//	auto regb = gr.reg_for_var(entry.b.id, entry);
//	auto regc = gr.reg_for_var(entry.c.id, entry);
//
//	if (!entry.live_vars[entry.b.id])
//	{
//		inst_stream << x64_imul(regb, regc);
//		gr.store_var_in(entry.a.id, regb); /* Result (a) is now in regb */
//		return;
//	}
//
//	auto rega = gr.reg_for_var(entry.a.id, entry);
//	inst_stream << x64_mov(rega, regb) << x64_imul(rega, regc);
}

void tac2x64::op_div(const tac_entry& entry)
{
	auto rega = x64_reg32(reg_avail[color_map[entry.a.id]]);
	auto regb = x64_reg32(reg_avail[color_map[entry.b.id]]);
	auto regc = x64_reg32(reg_avail[color_map[entry.c.id]]);

	inst_stream << x64_mov(x64_regs::eax, regb) << x64_xor(x64_regs::edx, x64_regs::edx)
			<< x64_idiv(regc) << x64_mov(rega, x64_regs::eax);

//	auto rega = gr.reg_for_var(entry.a.id, entry);
//	auto regb = gr.reg_for_var(entry.b.id, entry);
//	auto regc = gr.reg_for_var(entry.c.id, entry);
//
//	inst_stream << x64_mov(x64_regs::eax, regb) << x64_xor(x64_regs::edx, x64_regs::edx)
//			<< x64_idiv(regc) << x64_mov(rega, x64_regs::eax);
}

void tac2x64::op_ret(const tac_entry& entry)
{
	if (entry.b.id != -1)
	{
		auto result_reg = x64_reg32(reg_avail[color_map[entry.b.id]]);
		if (result_reg.value != x64_regs::eax.value)
			inst_stream << x64_mov(x64_regs::eax, result_reg);
	}
	epilogue();

//	if (entry.b.id != -1)
//	{
//		auto result_reg = gr.reg_for_var(entry.b.id, entry);
//		if (result_reg.value != x64_regs::eax.value)
//			inst_stream << x64_mov(x64_regs::eax, result_reg);
//	}
//	epilogue();
}

void tac2x64::compile_expression(const tac& t)
{
	rig.set_n_vars(t.get_num_vars());
	rig.generate(t.get_entries());
	rig.debug_print();

	color_map = rig.color(reg_avail.size());

	for(auto it = color_map.begin(); it != color_map.end(); ++it)
		cout << "Mapping: " << it->first << ": " << it->second << '\n';


	auto program = inst_stream.entry_point<int(int, int, int, int, int, int, int, int, int)>();

	/* For now, to make mul/div simple */
	lr.lock(x64_regs::eax);
	lr.lock(x64_regs::edx);


//	for (auto i = 3; i < 16; i++)
//	{
//		dr.use(i);
//	}

	//get_reg<x64_reg32> gr(dr, tac_var_type::temp);

	auto& entries = t.get_entries();

	prologue(0);

	bool last_was_ret;
	for(auto& entry: entries)
	{
		last_was_ret = false;

		switch(entry.type)
		{
		case tac_type::assign:
			op_assign(entry);
			break;
		case tac_type::add:
			op_add(entry);
			break;
		case tac_type::sub:
			op_sub(entry);
			break;
		case tac_type::mul:
			op_mul(entry);
			break;
		case tac_type::div:
			op_div(entry);
			break;
		case tac_type::ret:
			op_ret(entry);
			last_was_ret = true;
		break;
		}
	}
	if (!last_was_ret)
		epilogue();

	cout << x64_disassembler::disassemble(inst_stream, "intel", true);

	auto res = program(42, 42, 42, 42, 42, 42, 0, 0, 0);
	cout << "Result: " << dec << res << '\n';

	cout << "Made it out alive\n";
}



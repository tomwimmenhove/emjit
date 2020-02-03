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
	{
		if (stack_size < 256)
			inst_stream << x64_sub(x64_regs::rsp, static_cast<uint8_t>(stack_size));
		else
			inst_stream << x64_sub(x64_regs::rsp, static_cast<uint32_t>(stack_size));
	}
}

void tac2x64::epilogue()
{
	inst_stream << x64_leave() << x64_ret();
}

extern "C" void breakout_and_die()
{
	cerr << "Didn't make it. Sad\n";
	exit(1);
}

const tac2x64::var tac2x64::var_from_tac_var(const tac_var& tv)
{
	switch (tv.type)
	{
	case tac_var_type::unused:
		throw invalid_argument("Internal error: var_from_tac_var(): unused tac_var");

	case tac_var_type::constant:
		return var(variable_type::constant, tv.value);

	case tac_var_type::temp:
	case tac_var_type::local:
	case tac_var_type::param:
		int color = var_color(tv);
		if (color >= 0)
			return var(x64_reg32(reg_avail[color]));
		else
			return var(variable_type::stack, get_stack_pos(color));
	}

	throw invalid_argument("Internal error: var_from_tac_var(): invalid type");
}

void tac2x64::op_assign(const tac_entry& entry)
{
	src_dest<x64_mov>(var_from_tac_var(entry.a), var_from_tac_var(entry.b));
}

void tac2x64::op_add(const tac_entry& entry)
{
	auto tva = var_from_tac_var(entry.a);
	auto tvb = var_from_tac_var(entry.b);
	auto tvc = var_from_tac_var(entry.c);

	if (var_color(entry.a) == var_color(entry.b))
	{
		src_dest<x64_add>(tva, tvc);
		return;
	}

	if (var_color(entry.a) == var_color(entry.c))
	{
		src_dest<x64_add>(tva, tvb);
		return;
	}

	src_dest<x64_mov>(tva, tvb);
	src_dest<x64_add>(tva, tvc);
}

void tac2x64::op_sub(const tac_entry& entry)
{
	auto tva = var_from_tac_var(entry.a);
	auto tvb = var_from_tac_var(entry.b);
	auto tvc = var_from_tac_var(entry.c);

	if (var_color(entry.a) == var_color(entry.b))
	{
		src_dest<x64_sub>(tva, tvc);
		return;
	}

	if (var_color(entry.a) == var_color(entry.c))
	{
		src_dest<x64_sub>(tva, tvb);
		return;
	}

	src_dest<x64_mov>(tva, tvb);
	src_dest<x64_sub>(tva, tvc);
}

void tac2x64::op_mul(const tac_entry& entry)
{
	if (entry.c.type == tac_var_type::constant)
		throw invalid_argument("Multiplying by constants not yet implemented");

	// XXX: Fucking shit-show!

	int a_idx;
	int a_color = var_color(entry.a);
	if (a_color < 0)
		a_idx = temp_reg_idx;
	else
		a_idx = reg_avail[a_color];

	int b_idx;
	int b_color = var_color(entry.b);
	if (b_color < 0)
	{
		b_idx = temp_reg_idx;
		src_dest<x64_mov>(var(x64_reg32(temp_reg_idx)), var_from_tac_var(entry.b));
	}
	else
		b_idx = reg_avail[b_color];

	auto rega = x64_reg32(a_idx);
	auto regb = x64_reg32(b_idx);

	if (a_idx != b_idx)
		inst_stream << x64_mov(rega, regb);

	int c_color = var_color(entry.c);
	if (c_color >= 0)
		inst_stream << x64_imul(rega, x64_reg32(reg_avail[c_color]));
	else
		inst_stream << x64_imul(rega, x64_address(x64_regs::rbp, get_stack_pos(c_color)));

	if (a_color < 0)
		src_dest<x64_mov>(var_from_tac_var(entry.a), var(x64_reg32(temp_reg_idx)));

	inst_stream << x64_nop1();
}

void tac2x64::op_div(const tac_entry& entry)
{
	auto tva = var_from_tac_var(entry.a);
	auto tvb = var_from_tac_var(entry.b);

	src_dest<x64_mov>(var(x64_regs::eax), tvb);

	inst_stream << x64_xor(x64_regs::edx, x64_regs::edx);

	int c_color = var_color(entry.c);
	if (c_color >= 0)
		inst_stream << x64_idiv(x64_reg32(reg_avail[c_color]));
	else
		inst_stream << x64_idivl(x64_address(x64_regs::rbp, get_stack_pos(c_color)));

	src_dest<x64_mov>(tva, var(x64_regs::eax));
}

void tac2x64::op_ret(const tac_entry& entry)
{
	if (entry.b.id != -1)
	{
		auto eax_var = var(x64_regs::eax);
		auto result_var = var_from_tac_var(entry.b);
		if (eax_var != result_var)
			src_dest<x64_mov>(eax_var, result_var);
	}
	epilogue();
}

void tac2x64::debug_print_mapping(const tac& t)
{
	for(auto it = color_map.begin(); it != color_map.end(); ++it)
	{
		string var_name = t.get_var_name(it->first);
		string storage_name = it->second >= 0 ?							/* Where? */
				x64_reg32::names[reg_avail[it->second]] :				/* Register : */
				("[rbp" + to_string(get_stack_pos(it->second)) + ']');	/* Stack */

		cout << "Mapping: " << var_name << ": " << storage_name << '\n';
	}
}

/* XXX: This is terrible. There's probably a better way to just re-build the whole color map in 1 go */
bool tac2x64::try_swap_colors(int id, int new_color)
{
	int old_color = color_map[id];
	if (old_color < 0)
		return false;

	vector<int> restores;
	restores.reserve(color_map.size());

	for(auto it = color_map.begin(); it != color_map.end(); ++it)
	{
		if (it->second == old_color)
			restores.push_back(it->first);

		if (it->second == new_color)
			color_map[it->first] = old_color;
	}

	for(auto i: restores)
		color_map[i] = new_color;
	
	return true;
}

bool tac2x64::is_reg_live(const tac_entry& entry, int reg_idx)
{
	for (size_t i = 0; i < entry.live_vars.size(); i++)
	{
		auto color = color_map[i];
		if (color < 0)
			continue;

		if (reg_idx == reg_avail[color])
			return true;
	}

	return false;
}

void tac2x64::compile_expression(const tac& t)
{
	rig.set_n_vars(t.get_num_vars());
	rig.generate(t.get_entries());

	//color_map = rig.color(reg_avail.size());
	color_map = rig.color(reg_avail.size());

	/* If we couldn't do it without spilling, we're going to need
	 * a temporary register to use for loading and storing.
	 */
	if (rig.get_n_spills() != 0)
	{
		cout << "Awww... Couldn't do it without spilling. Re-coloring while reserving a load/store register.\n";
		temp_reg_idx = reg_avail.back();
		temp_var = var(x64_reg32(temp_reg_idx));
		reg_avail.pop_back();

		/* Re-color it */
		color_map = rig.color(reg_avail.size());
	}

	rig.debug_print();

	debug_print_mapping(t);

	cout << "Yankee swap\n";

	/* Experiment: Try to give var id 3 color 0 (ecx) */
	try_swap_colors(3, 0);

	rig.debug_print();

	debug_print_mapping(t);

	auto program = inst_stream.entry_point<int(int, int, int, int, int, int, int, int, int)>();

	/* For now, to make mul/div simple */
//	lr.lock(x64_regs::eax);
//	lr.lock(x64_regs::edx);

	auto& entries = t.get_entries();

	prologue(rig.get_n_spills() * sizeof(int32_t));

	auto func = t.get_func();

	for (size_t i = 0; i < func->parameters.size(); i++)
	{
		if (i < reg_args.size())
		{
			int param_id = func->parameters[i];

//			int color = color_map[param_id];
//
//			cout << "moving color " << color << " into " << x64_reg32::names[reg_args[i]] << '\n';

			src_dest<x64_mov>(
					var_from_tac_var(tac_var(tac_var_type::param, param_id)),
					var(x64_reg32(reg_args[i])));
		}
	}

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

	/* Implicit return */
	if (!last_was_ret)
		epilogue();

	cout << x64_disassembler::disassemble(inst_stream, "intel", true);

	auto res = program(1, 1, 10, 42, 42, 42, 0, 0, 0);
	cout << "Result: " << dec << res << '\n';

	cout << "Made it out alive\n";
}



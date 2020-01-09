/*
 * x64instruction.cpp
 *
 *  Created on: Dec 25, 2019
 *      Author: tom
 */

#include "x64instruction.h"

constexpr x64_reg64_0 x64_regs::rax;
constexpr x64_reg64 x64_regs::rcx;
constexpr x64_reg64 x64_regs::rdx;
constexpr x64_reg64 x64_regs::rbx;
constexpr x64_reg64 x64_regs::rsp;
constexpr x64_reg64 x64_regs::rbp;
constexpr x64_reg64 x64_regs::rsi;
constexpr x64_reg64 x64_regs::rdi;

constexpr x64_reg64 x64_regs::r8;
constexpr x64_reg64 x64_regs::r9;
constexpr x64_reg64 x64_regs::r10;
constexpr x64_reg64 x64_regs::r11;
constexpr x64_reg64 x64_regs::r12;
constexpr x64_reg64 x64_regs::r13;
constexpr x64_reg64 x64_regs::r14;
constexpr x64_reg64 x64_regs::r15;
const std::string x64_reg64::names[16] = {
		"rax",
		"rcx",
		"rdx",
		"rbx",
		"rsp",
		"rbp",
		"rsi",
		"rdi",

		"r8",
		"r9",
		"r10",
		"r11",
		"r12",
		"r13",
		"r14",
		"r15",
};

constexpr x64_reg32_0 x64_regs::eax;
constexpr x64_reg32 x64_regs::ecx;
constexpr x64_reg32 x64_regs::edx;
constexpr x64_reg32 x64_regs::ebx;
constexpr x64_reg32 x64_regs::esp;
constexpr x64_reg32 x64_regs::ebp;
constexpr x64_reg32 x64_regs::esi;
constexpr x64_reg32 x64_regs::edi;
const std::string x64_reg32::names[8] =
{
		"eax",
		"ecx",
		"edx",
		"ebx",
		"esp",
		"ebp",
		"esi",
		"edi",
};

constexpr x64_reg16_0 x64_regs::ax;
constexpr x64_reg16 x64_regs::cx;
constexpr x64_reg16 x64_regs::dx;
constexpr x64_reg16 x64_regs::bx;
constexpr x64_reg16 x64_regs::sp;
constexpr x64_reg16 x64_regs::bp;
constexpr x64_reg16 x64_regs::si;
constexpr x64_reg16 x64_regs::di;
const std::string x64_reg16::names[8] =
{
		"ax",
		"cx",
		"dx",
		"bx",
		"sp",
		"bp",
		"si",
		"di",
};

constexpr x64_reg8l_0 x64_regs::al;
constexpr x64_reg8l x64_regs::cl;
constexpr x64_reg8l x64_regs::dl;
constexpr x64_reg8l x64_regs::bl;

constexpr x64_reg8h x64_regs::ah;
constexpr x64_reg8h x64_regs::ch;
constexpr x64_reg8h x64_regs::dh;
constexpr x64_reg8h x64_regs::bh;
const std::string x64_reg8::names[8] =
{
		"al",
		"cl",
		"dl",
		"bl",
		"ah",
		"ch",
		"dh",
		"bh",
};

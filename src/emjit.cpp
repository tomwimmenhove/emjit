#include <iostream>
#include <vector>
#include <functional>
#include <cstdint>
#include <sys/mman.h>
#include <memory>

#include "segvcatcher.h"
#include "autoallocator.h"
#include "x64instruction.h"
#include "instructionstream.h"
#include "x64disassembler.h"

using namespace std;

void print_stream_debug(const instruction_stream& s)
{
	auto sd = s.data();
	for (size_t i = 0; i < s.size(); i++)
	{
		cout << i << ": " << std::hex << ((int) sd[i]) << " (" << std::dec << ((int) sd[i]) << ")\n";
	}
	cout << '\n';
}

int main()
{
	uintptr_t start = 65536;
	size_t len = 1024*1024*1024;

	auto allocator = std::make_shared<auto_allocator> (start, len, PROT_READ | PROT_WRITE | PROT_EXEC);

	instruction_stream s(allocator);

	auto entry_fn = s.entry_point<uint64_t()>();

	/* Assemble some shit */
//	s << x64_nop1();
//	s << x64_mov(x64_reg64::rax, static_cast<uint64_t>(0x12345678));
//	s << x64_mov(x64_reg64::rax, static_cast<uint32_t>(0x12345678));
//	s << x64_mov(x64_reg32::esi, static_cast<uint32_t>(0x12345678));
//	s << x64_mov(x64_reg16::dx, static_cast<uint16_t>(0x1234));
//	s << x64_mov(x64_reg8h::dh, static_cast<uint8_t>(0x12));
//	s << x64_mov(x64_reg8l::cl, static_cast<uint8_t>(0x34));
//
//
//
//	s << x64_mov(x64_reg64::rcx, x64_reg64::rdx);
//	s << x64_mov(x64_reg32::ecx, x64_reg32::edx);
//	s << x64_mov(x64_reg16::cx, x64_reg16::dx);
//	s << x64_mov(x64_reg8l::dl, x64_reg8l::bl);
//	s << x64_mov(x64_reg8h::dh, x64_reg8h::bh);
//
//
//    s << x64_mov(x64_reg64::rax, static_cast<uint64_t>(0x12345678));
//	s << x64_mov(x64_reg64::rax, static_cast<uint32_t>(0x12345678));
//
//	auto save_pos = s.pos();											// Save address of next instruction
//	//x64_mov mov_eax(x64_reg32::eax, static_cast<uint32_t>(12345678));
//	x64_mov mov_eax(x64_reg64::rax, static_cast<uint64_t>(0x123456789abcdef0));
//	s << mov_eax;
//
//	s << x64_mov(x64_reg8l::al, x64_addr_ptr<uint32_t>(65536));
//	s << x64_mov(x64_reg8h::ah, x64_addr_ptr<uint32_t>(65536));
//	s << x64_mov(x64_reg16::dx, x64_addr_ptr<uint32_t>(65536));
//	s << x64_mov(x64_reg32::eax, x64_addr_ptr<uint32_t>(65536));
//	s << x64_mov(x64_reg64::rax, x64_addr_ptr<uint32_t>(65536));
//
//	s << x64_nop1();
//
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg8l::al);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg8h::ah);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg16::dx);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg32::eax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg64::rax);
//
//	s << x64_mov(static_cast<uint64_t>(0x12345678), x64_reg64::rax);
//	s << x64_mov(static_cast<uint32_t>(0x12345678), x64_reg32::esi);
//	s << x64_mov(static_cast<uint16_t>(0x1234), x64_reg16::dx);
//	s << x64_mov(static_cast<uint8_t>(0x12), x64_reg8h::dh);
//	s << x64_mov(static_cast<uint8_t>(0x34), x64_reg8l::cl);
//
//
//	s << x64_mov(x64_reg64::rax, static_cast<uint64_t>(0));
//
////	uint64_t i64 = 0x123456789abcdef0;
////	uint32_t i32 = 0x12345678;
////	uint16_t i16 = 0x1234;
////	uint8_t i8 = 0x12;
////	s << x64_mov(x64_reg64::rax, x64_addr_ptr<uint64_t*>(&i64));
////	s << x64_mov(x64_reg32::eax, x64_addr_ptr<uint32_t*>(&i32));
////	s << x64_mov(x64_reg16::ax, x64_addr_ptr<uint16_t*>(&i16));
////	s << x64_mov(x64_reg8l::al, x64_addr_ptr<uint8_t*>(&i8));
//
//
	s << x64_mov((uint64_t*)0x123456789abcdef0, x64_regs::rax);
	s << x64_mov((uint32_t*)0x123456789abcdef0, x64_regs::eax);
//	s << x64_mov((uint16_t*)0x123456789abcdef0, x64_reg16::ax);
//	s << x64_mov(( uint8_t*)0x123456789abcdef0, x64_reg8l::al);

//
//
//	s << x64_mov(x64_reg64::rax, (uint64_t*)0x123456789abcdef0);
//	s << x64_mov(x64_reg32::eax, (uint32_t*)0x123456789abcdef0);
//	s << x64_mov(x64_reg16::ax,  (uint16_t*)0x123456789abcdef0);
//	s << x64_mov(x64_reg8l::al,  ( uint8_t*)0x123456789abcdef0);
//
//
////	s << x64_mov(x64_addr_ptr<uint64_t*>(&i64), x64_reg64::rax);
////	s << x64_mov(x64_addr_ptr<uint32_t*>(&i32), x64_reg32::eax);
////	s << x64_mov(x64_addr_ptr<uint16_t*>(&i16), x64_reg16::ax);
////	s << x64_mov(x64_addr_ptr<uint8_t*>(&i8), x64_reg8l::al);
//
//	s << x64_nop1() << x64_nop1();
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg8l::al);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg8h::ah);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg16::dx);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg32::eax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg64::rax);
//	s << x64_nop1() << x64_nop1();


//	s << x64_mov(x64_addr_ptr<uint32_t>(65536), x64_reg64::rdx);


	s << x64_mov(x64_regs::ax, x64_regs::ax);
	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::rax);
	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::r15);
	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::rax);
	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::r15);


	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::dx); // ERROR! FIXED, BITCH
	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::dx); // ERROR! FIXED, BITCH

	s << x64_mov(x64_regs::al, 12);


	//s << x64_mov(x64_addr_ptr(x64_regs::r15), x64_regs::ax);

//	s << x64_mov(x64_addr_ptr(x64_regs::rax), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr(x64_regs::rax), x64_regs::r15);
//	s << x64_mov(x64_addr_ptr(x64_regs::r15), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr(x64_regs::r15), x64_regs::r15);
//
//	s << x64_mov(x64_addr_ptr(x64_regs::r15), x64_regs::dx);


	//x64_addr_ptr flapprak(x64_regs::r15);

//	s << x64_mov(x64_addr_ptr<x64_reg64>(x64_reg64::rax), x64_reg32::edx);
//	s << x64_mov(x64_addr_ptr<x64_reg64>(x64_reg64::rax), x64_reg16::dx); // ERROR!
//	s << x64_mov(x64_addr_ptr<x64_reg64>(x64_reg64::rax), x64_reg8h::ch);
//	s << x64_mov(x64_addr_ptr<x64_reg64>(x64_reg64::r15), x64_reg8l::dl);

	//s << x64_mov(x64_addr_ptr<x64_reg64_base>(x64_regs::rax), x64_reg16::ax);

//	s << x64_mov(x64_regs::rax, x64_regs::rax);
//	s << x64_mov(x64_regs::rax, x64_regs::r15);
//	s << x64_mov(x64_regs::r15, x64_regs::rax);
//	s << x64_mov(x64_regs::r15, x64_regs::r15);

//	s << x64_mov(x64_reg64::rax, x64_reg64e::r15);// : x64_instruction(std::array<uint8_t, 2> { x64_rex::w, 0x89 }, x64_modrm{dst, src, 3}) {}
//	s << x64_mov(x64_reg64e::r15, x64_reg64::rax);// : x64_instruction(std::array<uint8_t, 2> { x64_rex::w, 0x89 }, x64_modrm{dst, src, 3}) {}
//	s << x64_mov(x64_reg64e::r15, x64_reg64e::r15);// : x64_instruction(std::array<uint8_t, 2> { x64_rex::w, 0x89 }, x64_modrm{dst, src, 3}) {}

//	s << x64_mov(x64_reg32::eax, x64_reg32::eax);// : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{dst, src, 3}) {}
//	s << x64_mov(x64_reg16::ax, x64_reg16);// : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{dst, src, 3}) {}
//	s << x64_mov(x64_reg8l::al, x64_reg8l);// : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{dst, src, 3}) {}
//	s << x64_mov(x64_reg8h::ah, x64_reg8h);// : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{static_cast<unsigned int>(dst) + 4, static_cast<unsigned int>(src) + 4, 3}) {}



//	s << x64_mov(x64_addr_ptr<x64_reg32>(x64_reg32::eax), x64_reg32::eax);
//	s << x64_mov(x64_addr_ptr<x64_reg32>(x64_reg32::eax), x64_reg32::ecx);
//	s << x64_mov(x64_addr_ptr<x64_reg32>(x64_reg32::ecx), x64_reg32::eax);
//	s << x64_mov(x64_addr_ptr<x64_reg32>(x64_reg32::ecx), x64_reg32::ecx);

//	s << x64_mov(x64_reg64::rdx, x64_addr_ptr<uint32_t> (65536));
//	s << x64_mov(x64_reg32::edx, x64_addr_ptr<uint32_t> (65536));
//	s << x64_mov(x64_reg16::dx, x64_addr_ptr<uint32_t> (65536));
//	s << x64_mov(x64_reg8l::dl, x64_addr_ptr<uint32_t> (65536));
//	s << x64_mov(x64_reg8h::dh, x64_addr_ptr<uint32_t> (65536));
//
//	/* 64 bit pointers */
//	s << x64_mov(x64_reg64::rax, x64_addr_ptr<uint64_t*>((uint64_t*)65536));
//	s << x64_mov(x64_reg32::eax, x64_addr_ptr<uint32_t*>((uint32_t*)65536));
//	s << x64_mov(x64_reg16::ax, x64_addr_ptr<uint16_t*> ((uint16_t*)65536));
//	s << x64_mov(x64_reg8l::al, x64_addr_ptr<uint8_t*>  ((uint8_t*)65536));
//
//
//	s << x64_mov(x64_addr_ptr<uint32_t> (65536), x64_reg64::rdx);
//	s << x64_mov(x64_addr_ptr<uint32_t> (65536), x64_reg32::edx);
//	s << x64_mov(x64_addr_ptr<uint32_t> (65536), x64_reg16::dx);
//	s << x64_mov(x64_addr_ptr<uint32_t> (65536), x64_reg8l::dl);
//	s << x64_mov(x64_addr_ptr<uint32_t> (65536), x64_reg8h::dh);
//
//	/* 64 bit pointers */
//	s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)65536), x64_reg64::rax);
//	s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)65536), x64_reg32::eax);
//	s << x64_mov(x64_addr_ptr<uint16_t*> ((uint16_t*)65536), x64_reg16::ax);
//	s << x64_mov(x64_addr_ptr<uint8_t*>  ((uint8_t*)65536), x64_reg8l::al);

//	s << x64_ret();
//
//	//auto die_fn = s.entry_point<int()>();
//	s << x64_ud2();
//
//	s << x64_lret();

	/* Run the function as int() */
	//auto r = entry_fn();
	//cout << "r: 0x" << hex << r << dec << '\n';

	/* Change the immediate value of the move instruction */
	//mov_eax.immediate<uint64_t>() = 42;

	/* Insert it in place of the 'old' instruction */
	//s.insert_at(save_pos, mov_eax);

	//r = entry_fn();
	//cout << "r: " << r << '\n';

	x64_disassembler::disassemble(s);

	cout << "finished\n";

//	die_fn(); // UD2
//
//	cout << "Won't see this\n";

	return 0;
}

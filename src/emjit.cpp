#include <iostream>
#include <vector>
#include <functional>
#include <cstdint>
#include <sys/mman.h>
#include <memory>
#include <exception>
#include <cassert>
#include <regex>
#include <vector>

#include "segvcatcher.h"
#include "autoallocator.h"
#include "x64instruction.h"
#include "instructionstream.h"
#include "x64disassembler.h"
#include "x64testing.h"

#include "x64instr.h"

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

	x64_instr inst1;

	inst1.add_opcode(0x42);

	inst1.set_imm((uint8_t) 0xdead);

	auto bla = inst1.get_imm<uint64_t>();

	s << x64_instr_mov(x64_regs::rbx, x64_regs::r15);

	cout << "Size: " << inst1.size() << '\n';
	cout << "imm: " << hex << (int) bla << '\n';

	auto disassmebly = x64_disassembler::disassemble(s, "intel");

	cout << disassmebly;

	return 0;
	x64_testing testing;

	try
	{
//		mov_unit_tests(allocator);
		testing.run_tests();
	}
	catch( const unit_test_exception& ex )
	{
		cerr << "Unit test failed:\n" << ex.what() << '\n';

		return 1;
	}

	return 0;



	//auto entry_fn = s.entry_point<uint64_t()>();


//	/* TESTS */
//	/* Move register into register */
//	s << x64_mov(x64_regs::rax, x64_regs::rbx);
//	s << x64_mov(x64_regs::rax, x64_regs::r10);
//	s << x64_mov(x64_regs::r15, x64_regs::rbx);
//	s << x64_mov(x64_regs::r15, x64_regs::r10);
//	s << x64_mov(x64_regs::eax, x64_regs::ebx);
//	s << x64_mov(x64_regs::bx, x64_regs::dx);
//	s << x64_mov(x64_regs::bl, x64_regs::dl);
//	s << x64_mov(x64_regs::bl, x64_regs::dh);
//	// PASSED
//
//	/* Move immediate into register */
//	s << x64_mov(x64_regs::eax, (uint32_t) 0x123);
//	s << x64_mov(x64_regs::rax, (uint64_t) 0x123);
//	s << x64_mov(x64_regs::rax, (uint32_t) 0x123);
//	s << x64_mov(x64_regs::r15, (uint64_t) 0x123);
//	s << x64_mov(x64_regs::r15, (uint32_t) 0x123);
//	s << x64_mov(x64_regs::ax, (uint16_t) 0x123);
//	s << x64_mov(x64_regs::ah, (uint8_t)  0x123);
//	s << x64_mov(x64_regs::al, (uint8_t)  0x123);
//	// PASSED
//
//	/* Move immediate address into register */
//	/* 32 bit pointers */
//	s << x64_mov(x64_regs::rax, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::r15, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::esp, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::si, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::bx, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::dl, x64_addr_ptr<uint32_t>(0x42));
//	s << x64_mov(x64_regs::ch, x64_addr_ptr<uint32_t>(0x42));
//	// PASSED
//
//	/* 64 bit pointers */
//	s << x64_mov(x64_regs::rax, x64_addr_ptr<uint64_t*>((uint64_t*)0x42));
//	s << x64_mov(x64_regs::eax, x64_addr_ptr<uint32_t*>((uint32_t*)0x42));
//	s << x64_mov(x64_regs::ax, x64_addr_ptr<uint16_t*>((uint16_t*)0x42));
//	s << x64_mov(x64_regs::al, x64_addr_ptr<uint8_t*> ((uint8_t*)0x42));
//	// PASSED
//
//	/* Move register into immediate address */
//	/* 32 bit pointers */
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::r15);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::esp);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::si);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::bx);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::dl);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x42), x64_regs::ch);
//	// PASSED
//
//	/* 64 bit pointers */
//	s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)0x42), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x42), x64_regs::eax);
//	s << x64_mov(x64_addr_ptr<uint16_t*>((uint16_t*)0x42), x64_regs::ax);
//	s << x64_mov(x64_addr_ptr<uint8_t*> ((uint8_t*)0x42), x64_regs::al);
//	// PASSED
//
//	/* Move register into register pointer */
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::rax);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::r15);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::rax);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::r15);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ebx);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::ebx);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cx);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cx);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cl);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cl);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ch);
//	s << x64_mov(x64_reg_addr(x64_regs::eax), x64_regs::r15);
//	s << x64_mov(x64_reg_addr(x64_regs::ebx), x64_regs::ebx);
//	s << x64_mov(x64_reg_addr(x64_regs::ecx), x64_regs::ebx);
//	s << x64_mov(x64_reg_addr(x64_regs::edx), x64_regs::cx);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cx);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::ch);
//	/* PASSED */
//
//	/* Move register pointer into register */
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::r15));
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::r15));
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::r15));
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::r15));
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::r15));
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::rax));
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::eax));
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ebx));
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ecx));
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edx));
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edi));
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::edi));
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::edi));
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edi));
//	/* PASSED */
//
//	/* experiments */
//	//s << x64_mov(x64_regs::eax, x64_reg_addr(x64_regs::rax), (int8_t) -12);
//	/* Move register into register pointer + 8 bit offset */
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::rax, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::r15, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::rax, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::r15, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ebx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::ebx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cl, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cl, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ch, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::eax), x64_regs::r15, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::ebx), x64_regs::ebx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::ecx), x64_regs::ebx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edx), x64_regs::cx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cx, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl, (int8_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::ch, (int8_t) -0x42);
//	/* PASSED */
//
//	/* Move register pointer + 8 bit offset into register */
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::r15), (int8_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::r15), (int8_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::r15), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::r15), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::r15), (int8_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::rax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::eax), (int8_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ebx), (int8_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ecx), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edx), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edi), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::edi), (int8_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::edi),(int8_t)  -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edi), (int8_t) -0x42);
//	/* PASSED */

	/* Move register into register pointer + 32 bit offset */
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::rax, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::r15, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::rax, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::r15, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ebx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::ebx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::cl, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::cl, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::ch, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::eax), x64_regs::r15, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::ebx), x64_regs::ebx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::ecx), x64_regs::ebx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edx), x64_regs::cx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cx, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::cl, (int32_t) -0x42);
//	s << x64_mov(x64_reg_addr(x64_regs::edi), x64_regs::ch, (int32_t) -0x42);
	/* PASSED */

	/* Move register pointer + 32 bit offset into register */
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::rsp), (int32_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::rax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::rax, x64_reg_addr(x64_regs::r15), (int32_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::r15), (int32_t) -0x42);
	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::rax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::r15), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::rax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::r15), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::rax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::r15), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::rsp), (int32_t) -0x42);
//	s << x64_mov(x64_regs::r15, x64_reg_addr(x64_regs::eax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ebx), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ebx, x64_reg_addr(x64_regs::ecx), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edx), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cx, x64_reg_addr(x64_regs::edi), (int32_t) -0x42);
//	s << x64_mov(x64_regs::cl, x64_reg_addr(x64_regs::edi), (int32_t) -0x42);


//
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::eax), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::ecx), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edx), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::ebx), (int32_t) -0x42);
//	//s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::esp), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::ebp), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::esi), (int32_t) -0x42);
//	s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edi), (int32_t) -0x42);

	//s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edi), (int32_t) -0x42);
	//s << x64_mov(x64_regs::ch, x64_reg_addr(x64_regs::edx), (int32_t) -0x42);


	/* PASSED */
	/* Assemble some shit */
//
//	s << x64_mov((uint64_t*)0x123456789abcdef0, x64_regs::rax);
//	s << x64_mov((uint32_t*)0x123456789abcdef0, x64_regs::eax);
////	s << x64_mov((uint16_t*)0x123456789abcdef0, x64_reg16::ax);
////	s << x64_mov(( uint8_t*)0x123456789abcdef0, x64_reg8l::al);
//
////	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::dx); // ERROR! FIXED, BITCH
////	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::dx); // ERROR! FIXED, BITCH
//	s << x64_mov(x64_reg_addr(x64_regs::rax), x64_regs::eax);
//	s << x64_mov(x64_reg_addr(x64_regs::r15), x64_regs::eax);
//
//	s << x64_mov(x64_regs::r15, x64_regs::rax);
//
//	s << x64_mov(x64_regs::al, x64_regs::ah);
//	s << x64_mov(x64_regs::ah, x64_regs::al);
//	s << x64_mov(x64_regs::al, x64_regs::al);
//	s << x64_mov(x64_regs::ah, x64_regs::ah);
//

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

	auto disassembly = x64_disassembler::disassemble(s, "intel", true);

	cout << disassembly;

	cout << "finished\n";

//	die_fn(); // UD2
//
//	cout << "Won't see this\n";

	return 0;
}

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

#include "memory/segvcatcher.h"
#include "memory/autoallocator.h"
#include "arch/x64/x64instruction.h"
#include "arch/x64/x64disassembler.h"
#include "arch/x64/x64testing.h"
#include "arch/instructionstream.h"

#include "parser/driver.h"
#include "compiler/tac.h"
#include "compiler/tac2x64.h"

using namespace std;


int main()
{
	uintptr_t start = 65536;
	size_t len = 1024*1024*1024;

	auto allocator = std::make_shared<auto_allocator> (start, len, PROT_READ | PROT_WRITE | PROT_EXEC, 4096);

	instruction_stream s(allocator);


	/* Parse the code */
	driver drv;
	//drv.trace_parsing = true;
	//drv.trace_scanning = true;
	if (drv.parse ("src/parser/parseme.txt") != 0)
		return 1;

	/* Get the entry point */
	//auto program = s.entry_point<int(int, int)>();

	/* Convert the expression into TAC */
	tac t(drv);

	/* Compile TAC to machine code */
	tac2x64 t2e(s);

	t2e.compile_expression(t);

	return 0;
}

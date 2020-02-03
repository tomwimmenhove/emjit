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
#include "parser/expression.h"

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

	/* Find the 'main' function */
	auto main = drv.get_function("main");
	if (!main)
	{
		cerr << "Can't find main() function.\n";
		return 1;
	}

	/* Convert the expression into TAC */
	tac t(main);

	/* Compile TAC to machine code */
	tac2x64 t2e(s);

	auto main_fn = s.entry_point<int(int, int, int, int, int, int, int, int, int)>();
	t2e.compile_expression(t);

	auto res = main_fn(1, 1, 1, 1, 1, 1, 1, 10, 1);
	cout << "Result: " << dec << res << '\n';

	cout << "Made it out alive\n";

	return 0;
}

/*
 * x64disassembler.cpp
 *
 *  Created on: Dec 21, 2019
 *      Author: tom
 */

#include "x64disassembler.h"

#include <cstdio>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;

void x64_disassembler::disassemble(const uint8_t* data, std::size_t len)
{
	char filename[PATH_MAX] = "/tmp/x64.asmXXXXXX";
	int fd = mkstemp(filename);

	write(fd, data, len);

	close(fd);

	string command = "objdump -b binary -D -m i386:x86-64 --adjust-vma=";
	//string command = "objdump -b binary -D -m i386:x86-64 -M intel --adjust-vma=";

	command += to_string((intptr_t) data);
	command += " ";
	command += filename;

	system(command.c_str());

	unlink(filename);
}

void x64_disassembler::disassemble(instruction_stream s)
{
	disassemble(s.data(), s.size());
}

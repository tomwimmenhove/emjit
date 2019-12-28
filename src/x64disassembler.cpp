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

string x64_disassembler::disassemble(const uint8_t* data, std::size_t len, std::string format, bool wide)
{
	char filename[PATH_MAX] = "/tmp/x64.asmXXXXXX";
	int fd = mkstemp(filename);

	write(fd, data, len);

	close(fd);

	string command = "objdump -b binary -D -m i386:x86-64 --adjust-vma=";

	command += to_string((intptr_t) data);
	command += " -M ";
	command += format;
	command += " ";
	command += filename;

	if (wide)
		command += " --wide";

	auto disassembly = exec(command);

//	unlink(filename);

	return disassembly;
}

string x64_disassembler::disassemble(const instruction_stream& s, std::string format, bool wide)
{
	return disassemble(s.data(), s.size(), format, wide);
}

string x64_disassembler::exec(string command)
{
    array<char, 4096> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

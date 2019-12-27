/*
 * x64disassembler.h
 *
 *  Created on: Dec 21, 2019
 *      Author: tom
 */

#ifndef X64DISASSEMBLER_H_
#define X64DISASSEMBLER_H_

#include "instructionstream.h"

#include <cstdint>
#include <string>

class x64_disassembler
{
public:
	static std::string disassemble(const uint8_t* data, std::size_t len, std::string format = "att", bool wide = false);
	static std::string disassemble(const instruction_stream& s, std::string format = "att", bool wide = false);

private:
	static std::string exec(std::string cmd);
};
#endif /* X64DISASSEMBLER_H_ */

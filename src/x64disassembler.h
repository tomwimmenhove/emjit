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

class x64_disassembler
{
public:
	static void disassemble(const uint8_t* data, std::size_t len);
	static void disassemble(instruction_stream s);
};
#endif /* X64DISASSEMBLER_H_ */

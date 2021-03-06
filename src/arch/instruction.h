/*
 * instruction.h
 *
 *  Created on: Dec 20, 2019
 *      Author: tom
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <cstdint>

class instruction
{
public:
	virtual std::size_t size() const = 0;
	virtual bool emit_to(uint8_t* p) const = 0;

	virtual ~instruction() { }
};

#endif /* INSTRUCTION_H_ */

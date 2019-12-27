/*
 * instructionstream.h
 *
 *  Created on: Dec 20, 2019
 *      Author: tom
 */

#ifndef INSTRUCTIONSTREAM_H_
#define INSTRUCTIONSTREAM_H_

#include <cstdint>
#include <memory>
#include <functional>

#include "instruction.h"
#include "autoallocator.h"

class instruction_stream
{
public:
	inline instruction_stream(const std::shared_ptr<auto_allocator>& allocator)
	 : data_ptr((uint8_t*) allocator->start()), max_size(allocator->size())
	{ }

	inline uint8_t* data() const { return data_ptr; }
	inline std::size_t size() const { return data_size; }
	inline uint8_t* pos() const { return &data_ptr[data_size]; }

	template<typename PROTOTYPE>
	inline PROTOTYPE* entry_point() { return (PROTOTYPE*) pos(); }

	inline void insert_at(uint8_t* address, const instruction& inst)
	{
		size_t inst_size = inst.size();

		if (address < data_ptr ||
				address + inst_size > data_ptr + data_size)
			throw std::out_of_range("Address was out of range for instruction");

		/* Copy the instruction */
		const uint8_t* id = inst.data();
		for (size_t i = 0; i < inst_size; i++)
					*address++ = id[i];
	}

	//__attribute__((optimize("unroll-loops"))) // <=- Makes it WORSE
    inline friend instruction_stream& operator<<(instruction_stream& output, const instruction& inst)
    {
		size_t inst_size = inst.size();

		if (inst_size + output.data_size > output.max_size)
			throw std::overflow_error("Tried to stream instruction beyond end of buffer");

		/* Copy the instruction */
		const uint8_t* id = inst.data();
		for (size_t i = 0; i < inst_size; i++)
			output.data_ptr[output.data_size++] = id[i];

       return output;
    }

private:
    uint8_t* data_ptr;
    std::size_t max_size;
    std::size_t data_size = 0;
};

#endif /* INSTRUCTIONSTREAM_H_ */

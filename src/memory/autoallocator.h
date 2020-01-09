/*
 * autoallocator.h
 *
 *  Created on: Dec 14, 2019
 *      Author: tom
 */

#ifndef AUTOALLOCATOR_H_
#define AUTOALLOCATOR_H_

#include <cstdint>
#include <cstddef>
#include <sys/mman.h>

class auto_allocator
{
public:
	auto_allocator(uintptr_t start, size_t len, int prot = PROT_READ | PROT_WRITE);
	virtual ~auto_allocator();

	inline uintptr_t start() { return start_addr; }
	inline size_t size() { return len; }

private:
	uintptr_t start_addr;
	size_t len;
	int prot;
	size_t psize;

	bool handler(uintptr_t addr);
};

#endif /* AUTOALLOCATOR_H_ */

/*
 * autoallocator.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: tom
 */

#include "autoallocator.h"
#include "segvcatcher.h"

#include <functional>
#include <unistd.h>

#include <iostream>

using namespace std;

auto_allocator::auto_allocator(uintptr_t start, size_t len, int prot, size_t pre_alloc)
	: start_addr(start), len(len), prot(prot), psize(getpagesize())
{
	segv_catcher::Instance().set_handler(start, len,
			std::bind(&auto_allocator::handler, this, placeholders::_1));

	for (auto i = start; i < start + pre_alloc; i += psize)
	{
		handler(i);
	}
}

auto_allocator::~auto_allocator()
{
	segv_catcher::Instance().remove_handler(start_addr);
}

bool auto_allocator::handler(uintptr_t addr)
{
	addr &= ~(psize - 1);

	auto r = mmap((void*) addr, 1, prot,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

	if (r == MAP_FAILED)
	{
		perror("mmap");

		return false;
	}

	return (uintptr_t) r == addr;
}


/*
 * usedregister.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: tom
 */

#include "usedregister.h"

int used_register::get()
{
	std::vector<bool>::iterator location = std::find(used.begin(), used.end(), false);

	if (location == used.end())
		return -1;

	int idx = location - used.begin();

	use(idx);

	return idx;
}

